/**************************************************************************//**
 * @file
 * @brief ECDSA example program
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

/*
 * This file is a modified version of the ecdsa.c example program
 * included in mbedtls-2.1.0 (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "retargetserial.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define mbedtls_printf     printf

/*
 * Select specific curve to use
 */
#define ECPARAMS    MBEDTLS_ECP_DP_SECP256R1

#if !defined(ECPARAMS)
#define ECPARAMS    mbedtls_ecp_curve_list()->grp_id
#endif

static void dump_buf( const char *title, unsigned char *buf, size_t len )
{
    size_t i;

    mbedtls_printf( "%s", title );
    for( i = 0; i < len; i++ )
        mbedtls_printf("%c%c", "0123456789ABCDEF" [buf[i] / 16],
                       "0123456789ABCDEF" [buf[i] % 16] );
    mbedtls_printf( "\n" );
}

static void dump_pubkey( const char *title, mbedtls_ecdsa_context *key )
{
    unsigned char buf[300];
    size_t len;

    if( mbedtls_ecp_point_write_binary( &key->grp, &key->Q,
                MBEDTLS_ECP_PF_UNCOMPRESSED, &len, buf, sizeof buf ) != 0 )
    {
        mbedtls_printf("internal error\n");
        return;
    }

    dump_buf( title, buf, len );
}

/* Enable DWT for clock cycle count timestamping. */
static void clk_timestamp_init(void)
{
    /* Enable debug clock AUXHFRCO */
    CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);

    /* Enable trace in core debug */
    CoreDebug->DHCSR |= CoreDebug_DHCSR_C_DEBUGEN_Msk;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* Unlock ITM and output data */
    ITM->LAR = 0xC5ACCE55;
    ITM->TCR = 0x10009 | ITM_TCR_DWTENA_Pos;

    /* Enable PC and IRQ sampling output */
    DWT->CTRL = 0x400113FF;
    DWT->CYCCNT = 0;
}

static inline uint32_t clk_timestamp_get(void)
{
  return DWT->CYCCNT;
}

int main( void )
{
    EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
    CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;
    int ret;
    mbedtls_ecdsa_context ctx_sign, ctx_verify;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    unsigned char hash[] = "This should be the hash of a message.";
    unsigned char sig[512];
    size_t sig_len;
    const char *pers = "ecdsa";
    uint32_t cycles;
    uint32_t freqKHz;

    /* Chip errata */
    CHIP_Init();

    /* Init DCDC regulator and HFXO with kit specific parameters */
    EMU_DCDCInit(&dcdcInit);
    CMU_HFXOInit(&hfxoInit);

    /* Switch HFCLK to HFXO and disable HFRCO */
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
    freqKHz = CMU_ClockFreqGet(cmuClock_CORE) / 1000;

    /* Enable timestamping */
    clk_timestamp_init();

    /* Initialize LEUART/USART and map LF to CRLF */
    RETARGET_SerialInit();
    RETARGET_SerialCrLf(1);

    mbedtls_ecdsa_init( &ctx_sign );
    mbedtls_ecdsa_init( &ctx_verify );
    mbedtls_ctr_drbg_init( &ctr_drbg );

    memset(sig, 0, sizeof( sig ) );
    ret = 1;

    /*
     * Generate a key pair for signing
     */
    mbedtls_printf( "\n  Core running at %" PRIu32 " kHz", freqKHz );
    mbedtls_printf( "\n  . Seeding the random number generator..." );

    mbedtls_entropy_init( &entropy );
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    mbedtls_printf( " ok\n  . Generating key pair..." );

    cycles = clk_timestamp_get();
    if( ( ret = mbedtls_ecdsa_genkey( &ctx_sign, ECPARAMS,
                              mbedtls_ctr_drbg_random, &ctr_drbg ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ecdsa_genkey returned %d\n", ret );
        goto exit;
    }
    cycles = clk_timestamp_get()-cycles;

    mbedtls_printf( " ok (key size: %d bits, clks: %" PRIu32 " time: %" PRIu32 " ms)\n",
                    (int) ctx_sign.grp.pbits, cycles, cycles / freqKHz );

    dump_pubkey( "  + Public key: ", &ctx_sign );

    /*
     * Sign some message hash
     */
    mbedtls_printf( "  . Signing message..." );

    cycles = clk_timestamp_get();
    if( ( ret = mbedtls_ecdsa_write_signature( &ctx_sign, MBEDTLS_MD_SHA256,
                                       hash, sizeof( hash ),
                                       sig, &sig_len,
                                       mbedtls_ctr_drbg_random, &ctr_drbg ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ecdsa_genkey returned %d\n", ret );
        goto exit;
    }
    cycles = clk_timestamp_get()-cycles;
    mbedtls_printf( " ok (signature length = %u, clks: %" PRIu32 " time: %" PRIu32 " ms)\n",
                    (unsigned int) sig_len, cycles, cycles / freqKHz);

    dump_buf( "  + Hash: ", hash, sizeof hash );
    dump_buf( "  + Signature: ", sig, sig_len );

    /*
     * Transfer public information to verifying context
     *
     * We could use the same context for verification and signatures, but we
     * chose to use a new one in order to make it clear that the verifying
     * context only needs the public key (Q), and not the private key (d).
     */
    mbedtls_printf( "  . Preparing verification context..." );

    if( ( ret = mbedtls_ecp_group_copy( &ctx_verify.grp, &ctx_sign.grp ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ecp_group_copy returned %d\n", ret );
        goto exit;
    }

    if( ( ret = mbedtls_ecp_copy( &ctx_verify.Q, &ctx_sign.Q ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ecp_copy returned %d\n", ret );
        goto exit;
    }

    ret = 0;

    /*
     * Verify signature
     */
    mbedtls_printf( " ok\n  . Verifying signature..." );

    cycles = clk_timestamp_get();
    if( ( ret = mbedtls_ecdsa_read_signature( &ctx_verify,
                                      hash, sizeof( hash ),
                                      sig, sig_len ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ecdsa_read_signature returned %d\n", ret );
        goto exit;
    }
    cycles = clk_timestamp_get()-cycles;

    mbedtls_printf( " ok  (clks: %" PRIu32 " time: %" PRIu32 " ms)\n", cycles, cycles / freqKHz);

exit:

    mbedtls_ecdsa_free( &ctx_verify );
    mbedtls_ecdsa_free( &ctx_sign );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

    while (1);
}
