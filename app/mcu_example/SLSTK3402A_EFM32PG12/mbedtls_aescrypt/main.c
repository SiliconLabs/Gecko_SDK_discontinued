/**************************************************************************//**
 * @file
 * @brief AES-256 encryption example program
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
 * This file is a modified version of the aescrypt2.c example program
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
#include "em_gpio.h"
#include "retargetserial.h"
#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define mbedtls_fprintf            fprintf
#define mbedtls_printf              printf

#define MODE_ENCRYPT                   (1)
#define MODE_DECRYPT                   (2)

#define AES_BLOCK_SIZE                (16)
#define IV_SIZE                       (16)
#define TAG_SIZE                      (32)
#define MAX_MESSAGE_SIZE_ENCRYPTION (1024)
#define MAX_MESSAGE_SIZE_DECRYPTION                             \
  (2*MAX_MESSAGE_SIZE_ENCRYPTION + 2*IV_SIZE + 2*TAG_SIZE + 1)

#define STDIN_FILENO                   (0)
#define STDOUT_FILENO                  (1)

static char message[MAX_MESSAGE_SIZE_DECRYPTION];

static const char *key256bits = "603DEB10 15CA71BE 2B73AEF0 857D7781"
                                "1F352C07 3B6108D7 2D9810A3 0914DFF4";

/**************************************************************************//**
 * @brief Convert ascii hexadecimal text into binary
 *****************************************************************************/
static int hextext2bin(uint8_t *binbuf, unsigned int binbuflen, const char *hexstr)
{
    uint32_t ret = 0;
    int      i;
    uint8_t  tmp;
    uint8_t  val;

    while (ret < binbuflen)
    {
        val = 0;
        for (i = 1; i >= 0; i--)
        {
            tmp = *(hexstr++);
            /* Skip spaces */
            while (tmp == ' ')
            {
                tmp = *(hexstr++);
            }
            /* Reached end of string? */
            if (!tmp)
              goto done;

            if (tmp > '9')
            {
                /* Ensure uppercase hex */
                tmp &= ~0x20;

                val |= ((tmp - 'A') + 10) << (4 * i);
            }
            else
            {
                val |= (tmp - '0') << (4 * i);
            }
        }
        *(binbuf++) = val;
        ret++;
    }
 done:
    return ret;
}

/**************************************************************************//**
 * @brief Convert binary data to ascii hexadecimal text string
 *****************************************************************************/
void bin2hextext(char* hexstr, uint8_t* binbuf, unsigned int binbuflen)
{
    uint32_t i;
    uint8_t nibble;

    for (i = 0; i < binbuflen; i++)
    {
      nibble = (binbuf[i] >> 4) & 0xF;
      *hexstr++ = nibble > 9 ? nibble - 10 + 'A' : nibble + '0';
      nibble = (binbuf[i] >> 0) & 0xF;
      *hexstr++ = nibble > 9 ? nibble - 10 + 'A' : nibble + '0';
    }
    /* Null terminate at end of string. */
    *hexstr = 0;
}

/**************************************************************************//**
 * @brief Initialize timestamp
 *****************************************************************************/
static void timestamp_init(void)
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

/**************************************************************************//**
 * @brief  Get clock cycle timestamp
 *****************************************************************************/
static inline uint32_t clk_timestamp_get(void)
{
    return DWT->CYCCNT;
}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main( void )
{
    EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
    CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;
    int ret;
    int i, n;
    int mode = 0, lastn;
    size_t keylen = 256/8;
    char *p;
    unsigned char IV[AES_BLOCK_SIZE];
    unsigned char key[256/8];
    unsigned char digest[32];
    unsigned char buffer[64];
    unsigned char diff;
    char initphrase[16];

    mbedtls_aes_context aes_ctx;
    mbedtls_md_context_t sha_ctx;

    long message_size, max_message_size, offset;
    uint32_t cycles;

    /* Chip errata */
    CHIP_Init();

    /* Init DCDC regulator and HFXO with kit specific parameters */
    EMU_DCDCInit(&dcdcInit);
    CMU_HFXOInit(&hfxoInit);

    /* Switch HFCLK to HFXO and disable HFRCO */
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

    /* Enable timestamping */
    timestamp_init();

    /* Initialize LEUART/USART and map LF to CRLF */
    RETARGET_SerialInit();
    RETARGET_SerialCrLf(1);

    mbedtls_aes_init( &aes_ctx );
    mbedtls_md_init( &sha_ctx );

    ret = mbedtls_md_setup( &sha_ctx,
                            mbedtls_md_info_from_type( MBEDTLS_MD_SHA256 ),
                            1 );
    if( ret != 0 )
    {
        mbedtls_printf( "  ! mbedtls_md_setup() returned -0x%04x\n", -ret );
        goto exit;
    }

    memset(message, 0, sizeof(message));
    memset(initphrase, 0, sizeof(initphrase));
    memset(IV, 0, sizeof(IV));
    memset(key, 0, sizeof(key));
    memset(digest, 0, sizeof(digest));
    memset(buffer, 0, sizeof(buffer));

    hextext2bin((uint8_t *) key, 256/8, key256bits);

    printf("Welcome to AESCRYPT.\n");
    n = 1;
    while (0 == mode)
    {
        if (n > 0)
        {
            printf("Please type 'e' to encrypt or 'd' to decrypt: \n");
        }
        n = getchar();
        if ('e' == n)
        {
            mode = MODE_ENCRYPT;
            max_message_size = MAX_MESSAGE_SIZE_ENCRYPTION;

            printf("Thanks. Please type a short phrase to be used as input to "
                   "generate the initial vector of the encryption: ");
            p = initphrase;
            while ((13 != n) &&
                   (p-initphrase < (int)(sizeof(initphrase)-1)))
            {
                n = getchar();
                if (n > 0)
                {
                  /* Local echo */
                  putchar(n);
                  *p++ = (unsigned char) n;
                }
            }
            printf("\nThanks. Please send a message to be encrypted, "
                   "terminated by hitting <enter>, i.e. a newline "
                   "character. The ciphertext will be printed in the following "
                   "format:\n");
            /* Make sure n is not 'd' */
            n = 'e';
        }
        if ('d' == n)
        {
            mode = MODE_DECRYPT;
            max_message_size = MAX_MESSAGE_SIZE_DECRYPTION;

            printf("Thanks. Please send the message (ciphertext) to be "
                   "decrypted. The format must be:\n");
        }
    }

    printf("Initial Vector(16 bytes) | Ciphertext | "
           "Message Digest Tag(32 bytes)\n");

    p = message;
    message_size = 0;

    while ( message_size < max_message_size )
    {
        n = getchar();
        if (13 == n)  /* newline marks end of message */
            break;
        if (n > 0)
        {
            *p++ = n;
            message_size++;
            /* Local echo */
            putchar(n);
        }
    }

    /* Newline after plain text message input, and ciphertext. */
    printf( "\n" );

    if( mode == MODE_ENCRYPT )
    {
        char hexbuf[2*TAG_SIZE + 1];

        /*
         * Generate the initialization vector as:
         * IV = SHA-256( message_size || initphrase )[0..15]
         */
        for( i = 0; i < 8; i++ )
            buffer[i] = (unsigned char)( message_size >> ( i << 3 ) );

        p = initphrase;

        mbedtls_md_starts( &sha_ctx );
        mbedtls_md_update( &sha_ctx, buffer, 8 );
        mbedtls_md_update( &sha_ctx, (unsigned char*)p, strlen(initphrase) );
        mbedtls_md_finish( &sha_ctx, digest );

        memcpy( IV, digest, sizeof(IV) );

        /*
         * The last four bits in the IV are actually used
         * to store the file size modulo the AES block size.
         */
        lastn = (int)( message_size & 0x0F );

        IV[15] = (unsigned char) ( ( IV[15] & 0xF0 ) | lastn );

        /*
         * Append the IV at the beginning of the output.
         */
        bin2hextext( hexbuf, IV, sizeof(IV) );
        printf( hexbuf );

        /*
         * Hash the IV and the secret key together 8192 times
         * using the result to setup the AES context and HMAC.
         */
        memset( digest, 0,  sizeof(digest) );
        memcpy( digest, IV, IV_SIZE );

        for( i = 0; i < 8192; i++ )
        {
            mbedtls_md_starts( &sha_ctx );
            mbedtls_md_update( &sha_ctx, digest, TAG_SIZE );
            mbedtls_md_update( &sha_ctx, key, keylen );
            mbedtls_md_finish( &sha_ctx, digest );
        }

        memset( key, 0, sizeof( key ) );
        mbedtls_aes_setkey_enc( &aes_ctx, digest, 256 );
        mbedtls_md_hmac_starts( &sha_ctx, digest, TAG_SIZE );

        /*
         * Encrypt and write the ciphertext.
         */
        cycles = clk_timestamp_get();
        for( p = message, offset = 0;
             offset < message_size;
             offset += AES_BLOCK_SIZE, p += AES_BLOCK_SIZE )
        {
            n = ( message_size - offset > AES_BLOCK_SIZE ) ?
              AES_BLOCK_SIZE : (int) ( message_size - offset );

            memset (buffer, 0, AES_BLOCK_SIZE);
            memcpy (buffer, p, n);

            for( i = 0; i < AES_BLOCK_SIZE; i++ )
                buffer[i] = (unsigned char)( buffer[i] ^ IV[i] );

            mbedtls_aes_crypt_ecb( &aes_ctx, MBEDTLS_AES_ENCRYPT, buffer, buffer );

            mbedtls_md_hmac_update( &sha_ctx, buffer, AES_BLOCK_SIZE );

            bin2hextext( hexbuf, buffer, AES_BLOCK_SIZE );
            printf( hexbuf );

            memcpy( IV, buffer, AES_BLOCK_SIZE );
        }
        cycles = clk_timestamp_get()-cycles;

        /*
         * Finally write the HMAC.
         */
        mbedtls_md_hmac_finish( &sha_ctx, digest );

        bin2hextext( hexbuf, digest, TAG_SIZE );
        printf( "%s\n", hexbuf );
    }

    if( mode == MODE_DECRYPT )
    {
        unsigned char tmp[AES_BLOCK_SIZE];

        /*
         *  The encrypted file must be structured as follows:
         *
         *        00 .. 15              Initialization Vector
         *        16 .. 31              AES Encrypted Block #1
         *           ..
         *      N*16 .. (N+1)*16 - 1    AES Encrypted Block #N
         *  (N+1)*16 .. (N+1)*16 + 32   HMAC-SHA-256(ciphertext)
         */
        if( message_size <  (2*IV_SIZE + 2*TAG_SIZE) )
        {
            mbedtls_printf( "File too short to be encrypted.\n" );
            goto exit;
        }

        if( ( message_size & 0x0F ) != 0 )
        {
            mbedtls_printf(  "File size not a multiple of 16.\n" );
            goto exit;
        }

        /*
         * Subtract the IV + HMAC length.
         */
        message_size -= ( 2*IV_SIZE + 2*TAG_SIZE );

        /*
         * Read the IV and original message_size modulo 16.
         */
        hextext2bin(IV, IV_SIZE, message);
        lastn = IV[15] & 0x0F;

        /*
         * Hash the IV and the secret key together 8192 times
         * using the result to setup the AES context and HMAC.
         */
        memset( digest, 0,  TAG_SIZE );
        memcpy( digest, IV, IV_SIZE );

        for( i = 0; i < 8192; i++ )
        {
            mbedtls_md_starts( &sha_ctx );
            mbedtls_md_update( &sha_ctx, digest, TAG_SIZE );
            mbedtls_md_update( &sha_ctx, key, keylen );
            mbedtls_md_finish( &sha_ctx, digest );
        }

        memset( key, 0, sizeof( key ) );
        mbedtls_aes_setkey_dec( &aes_ctx, digest, 256 );
        mbedtls_md_hmac_starts( &sha_ctx, digest, TAG_SIZE );

        /*
         * Decrypt ciphertext and write the plaintext.
         */
        cycles = clk_timestamp_get();
        for( p = &message[2*IV_SIZE], offset = 0;
             offset < message_size;
             offset += 2*AES_BLOCK_SIZE, p += 2*AES_BLOCK_SIZE )
        {
            hextext2bin( buffer, AES_BLOCK_SIZE, p );

            memcpy( tmp, buffer, AES_BLOCK_SIZE );

            mbedtls_md_hmac_update( &sha_ctx, buffer, AES_BLOCK_SIZE );
            mbedtls_aes_crypt_ecb( &aes_ctx, MBEDTLS_AES_DECRYPT, buffer, buffer );

            for( i = 0; i < AES_BLOCK_SIZE; i++ )
                buffer[i] = (unsigned char)( buffer[i] ^ IV[i] );

            memcpy( IV, tmp, AES_BLOCK_SIZE );

            n = ( lastn > 0 && offset == message_size - 2*AES_BLOCK_SIZE )
                ? lastn : AES_BLOCK_SIZE;

            buffer[n] = 0;
            printf( (char*) buffer );
        }
        cycles = clk_timestamp_get()-cycles;

        /* Newline after printing plaintext. */
        printf( "\n" );

        /*
         * Verify the message authentication code.
         */
        mbedtls_md_hmac_finish( &sha_ctx, digest );

        hextext2bin( buffer, TAG_SIZE, p );

        /* Use constant-time buffer comparison */
        diff = 0;
        for( i = 0; i < TAG_SIZE; i++ )
            diff |= digest[i] ^ buffer[i];

        if( diff != 0 )
        {
            mbedtls_printf( "HMAC check failed: wrong key, "
                             "or file corrupted.\n" );
            goto exit;
        }
        else
          printf("Message digest tag OK.\n");
    }

    ret = 0;

exit:
    memset( buffer, 0, sizeof( buffer ) );
    memset( digest, 0, sizeof( digest ) );

    mbedtls_aes_free( &aes_ctx );
    mbedtls_md_free( &sha_ctx );

    while (1);
}
