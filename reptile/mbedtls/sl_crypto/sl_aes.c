/*
 *  FIPS-197 compliant AES implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * This file includes alternative plugin implementations of various
 * functions in aes.c using the CRYPTO hardware accelerator incorporated
 * in MCU devices from Silicon Laboratories.
 */

/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_AES_C)

#include <string.h>

#include "mbedtls/aes.h"

#include "em_crypto.h"
#include "em_bitband.h"

#if defined(MBEDTLS_PADLOCK_C)
#include "mbedtls/padlock.h"
#endif
#if defined(MBEDTLS_AESNI_C)
#include "mbedtls/aesni.h"
#endif

#if defined(MBEDTLS_AES_ALT)

#define CRYPTO_AES_BLOCKSIZE    ( 16 )

#define MBEDTLS_RETVAL_CHK(f) do { if( ( ret = f ) != 0 ) goto cleanup; } while( 0 )

#define CRYPTO_CLOCK_ENABLE CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_CRYPTO;

#define CRYPTO_CLOCK_DISABLE CMU->HFBUSCLKEN0 &= ~CMU_HFBUSCLKEN0_CRYPTO;

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n )
{
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

/**
 * \brief           Internal function for setting encryption key in CRYPTO.
 *
 * \param ctx       AES context including key
 */
static inline int crypto_setkey_enc( mbedtls_aes_context *ctx )
{
    uint32_t* _key = (uint32_t*) ctx->key;
    
    switch( ctx->keybits )
    {
    case 128:
        /* Set AES-128 mode */
        BUS_RegMaskedClear(&CRYPTO->CTRL, 1<<_CRYPTO_CTRL_AES_SHIFT);
        /* Load key in KEYBUF register */
        CRYPTO->KEYBUF = _key[0];
        CRYPTO->KEYBUF = _key[1];
        CRYPTO->KEYBUF = _key[2];
        CRYPTO->KEYBUF = _key[3];
        return( 0 );
    case 256:
        /* Set AES-256 mode */
        BUS_RegMaskedSet(&CRYPTO->CTRL, 1<<_CRYPTO_CTRL_AES_SHIFT);
        /* Load key in KEYBUF register (= DDATA4) */
        CRYPTO_DDataWrite( cryptoRegDDATA4, _key );
        return( 0 );
    default:
        /* Unsupported key size */
        return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }
}

/**
 * \brief           Internal function for setting decryption key in CRYPTO.
 *
 * \param ctx       AES context including key
 */
static int crypto_setkey_dec( mbedtls_aes_context *ctx )
{
    uint32_t* _key = (uint32_t*) ctx->key;
    
    switch( ctx->keybits )
    {
    case 128:
        /* Set AES-128 mode */
        BUS_RegMaskedClear(&CRYPTO->CTRL, 1<<_CRYPTO_CTRL_AES_SHIFT);
        /* Load key in KEYBUF register */
        CRYPTO->KEYBUF = _key[0];
        CRYPTO->KEYBUF = _key[1];
        CRYPTO->KEYBUF = _key[2];
        CRYPTO->KEYBUF = _key[3];
        /* Do dummy encryption to generate decrypt key, and move result to
           KEYBUF = DDATA4. */
        CRYPTO_EXECUTE_2(CRYPTO_CMD_INSTR_AESENC,
                         CRYPTO_CMD_INSTR_DDATA1TODDATA4); 
        return( 0 );
    case 256:
        /* Set AES-256 mode */
        BUS_RegMaskedSet(&CRYPTO->CTRL, 1<<_CRYPTO_CTRL_AES_SHIFT);
        /* Load key in KEYBUF register (= DDATA4) */
        CRYPTO_DDataWrite( cryptoRegDDATA4, _key );
        /* Do dummy encryption to generate decrypt key, and move result to
           KEYBUF = DDATA4. */
        CRYPTO_EXECUTE_2(CRYPTO_CMD_INSTR_AESENC,
                         CRYPTO_CMD_INSTR_DDATA1TODDATA4); 
        return( 0 );
    default:
        /* Unsupported key size */
      return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }
}

#if defined(MBEDTLS_CIPHER_MODE_CBC) || \
    defined(MBEDTLS_CIPHER_MODE_CFB) || \
    defined(MBEDTLS_CIPHER_MODE_CTR)
/**
 * \brief           Process multiple AES blocks using current CRYPTO
 *                  instruction sequence.
 *
 * \param len       Length of input buffer (in bytes)
 * \param inReg     Which CRYPTO register that expects input block
 * \param in        Input buffer
 * \param outReg    Which CRYPTO register where result ends up
 * \param out       Output buffer where result should be written
 * \param iv        If 'iv' is non-NULL, copy input to iv for
 *                  subsequent iterative calls.
 */
static void crypto_aes_processloop(uint32_t               len,
                                   CRYPTO_DataReg_TypeDef inReg,
                                   uint32_t*              in,
                                   CRYPTO_DataReg_TypeDef outReg,
                                   uint32_t*              out,
                                   uint32_t*              iv)
{
  len /= CRYPTO_AES_BLOCKSIZE;
  CRYPTO->SEQCTRL = 16 << _CRYPTO_SEQCTRL_LENGTHA_SHIFT;

  while (len--)
  {
    if (iv)
      memcpy(iv, in, 16);
    
    /* Load data and trigger encryption */
    CRYPTO_DataWrite(inReg,(uint32_t*)in);

    CRYPTO->CMD = CRYPTO_CMD_SEQSTART;

    /* Save encrypted/decrypted data */
    CRYPTO_DataRead(outReg,(uint32_t*)out);

    out += 4;
    in  += 4;
  }
}
#endif /* #if defined(MBEDTLS_CIPHER_MODE_CBC) || \
    defined(MBEDTLS_CIPHER_MODE_CFB) || \
    defined(MBEDTLS_CIPHER_MODE_CTR) */

/*
 * Initialize AES context
 */
void mbedtls_aes_init( mbedtls_aes_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_aes_context ) );
}

/*
 * Clear AES context
 */
void mbedtls_aes_free( mbedtls_aes_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_aes_context ) );
}

/*
 * AES key schedule (encryption)
 */
int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx, const unsigned char *key,
                            unsigned int keybits )
{
    if ( ( 128 != keybits ) && ( 256 != keybits ) )
      /* Unsupported key size */
      return( -1 );

    ctx->keybits = keybits;
    memcpy(ctx->key, key, keybits/8);
    
    return 0;
}

/*
 * AES key schedule (decryption)
 */
int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    if ( ( 128 != keybits ) && ( 256 != keybits ) )
      /* Unsupported key size */
      return( -1 );

    ctx->keybits = keybits;
    memcpy(ctx->key, key, keybits/8);
    
    return 0;
}

/*
 * AES-ECB block encryption/decryption
 */
int mbedtls_aes_crypt_ecb( mbedtls_aes_context *ctx,
                           int mode,
                           const unsigned char input[16],
                           unsigned char output[16] )
{
    int ret = 0;

    /* Initialize CRYPTO */
    CRYPTO_CLOCK_ENABLE;
    CRYPTO->CTRL = 0;
    CRYPTO->WAC = 0;
    CRYPTO->SEQCTRL = 0;
    CRYPTO->SEQCTRLB = 0;

    if( mode == MBEDTLS_AES_ENCRYPT )
    {
        /* Load plaintext */
        CRYPTO_DataWrite( cryptoRegDATA0, (uint32_t*)input );

        /* Set encryption key */
        MBEDTLS_RETVAL_CHK( crypto_setkey_enc (ctx) );

        /* Trigger encryption */
        CRYPTO->CMD  = CRYPTO_CMD_INSTR_AESENC;
    }
    else
    {
        /* Set/calculate decryption key */
        MBEDTLS_RETVAL_CHK( crypto_setkey_dec (ctx) );
    
        /* Load ciphertext. Must be done after decryption key calculation
           which is destructive for value in DATA0. */
        CRYPTO_DataWrite( cryptoRegDATA0, (uint32_t*)input );
        
        /* Trigger decryption */
        CRYPTO->CMD  = CRYPTO_CMD_INSTR_AESDEC;
    }
    
    /* Save encrypted/decrypted data */
    CRYPTO_DataRead( cryptoRegDATA0, (uint32_t*)output);
    
 cleanup:
    
    CRYPTO_CLOCK_DISABLE;

    return ret;
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)

/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int ret;

    /* Input length must be a multiple of 16 bytes which is the AES block
       length. */
    if( length & 0xf )
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );

    CRYPTO_CLOCK_ENABLE;
    CRYPTO->CTRL = 0;
    CRYPTO->WAC = 0;
    CRYPTO->SEQCTRL = 0;
    CRYPTO->SEQCTRLB = 0;
    
    if( mode == MBEDTLS_AES_ENCRYPT )
    {
        /* Set encryption key */
        MBEDTLS_RETVAL_CHK( crypto_setkey_enc (ctx) );
        
        CRYPTO_DataWrite( cryptoRegDATA0, (uint32_t*)iv );
        
        CRYPTO->SEQ0 =
            CRYPTO_CMD_INSTR_DATA1TODATA0XOR << _CRYPTO_SEQ0_INSTR0_SHIFT |
            CRYPTO_CMD_INSTR_AESENC          << _CRYPTO_SEQ0_INSTR1_SHIFT;
        
        crypto_aes_processloop( length,
                                cryptoRegDATA1, (uint32_t*) input,
                                cryptoRegDATA0, (uint32_t*) output,
                                0 );
        /* Copy last output block to iv */
        memcpy (iv, &output[length-16], 16);
    }
    else
    {
        /* Set key */
        MBEDTLS_RETVAL_CHK( crypto_setkey_dec (ctx) );
        
        CRYPTO_DataWrite( cryptoRegDATA2, (uint32_t*)iv );
        
        CRYPTO->SEQ0 =
            CRYPTO_CMD_INSTR_DATA1TODATA0    << _CRYPTO_SEQ0_INSTR0_SHIFT |
            CRYPTO_CMD_INSTR_AESDEC          << _CRYPTO_SEQ0_INSTR1_SHIFT |
            CRYPTO_CMD_INSTR_DATA2TODATA0XOR << _CRYPTO_SEQ0_INSTR2_SHIFT |
            CRYPTO_CMD_INSTR_DATA1TODATA2    << _CRYPTO_SEQ0_INSTR3_SHIFT;

        CRYPTO->SEQ1 = 0;
        
        /* The following call is equivalent to the last call in the
           'if( mode == MBEDTLS_AES_ENCRYPT )' branch. However moving this
           call outside the conditional scope results in slightly poorer
           performance for some compiler optimizations. */
        crypto_aes_processloop( length,
                                cryptoRegDATA1, (uint32_t*) input,
                                cryptoRegDATA0, (uint32_t*) output,
                                (uint32_t*)iv );
    }

 cleanup:
    
    CRYPTO_CLOCK_DISABLE;

    return( ret );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                              int mode,
                              size_t length,
                              size_t *iv_off,
                              unsigned char iv[16],
                              const unsigned char *input,
                              unsigned char *output )
{
    if (iv_off && *iv_off)
    {
        int c;
        size_t n = *iv_off;

        if( mode == MBEDTLS_AES_DECRYPT )
        {
            while( length-- )
            {
                if( n == 0 )
                    mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );
                
                c = *input++;
                *output++ = (unsigned char)( c ^ iv[n] );
                iv[n] = (unsigned char) c;
                
                n = ( n + 1 ) & 0x0F;
            }
        }
        else
        {
            while( length-- )
            {
                if( n == 0 )
                    mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );
                
                iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );
                
                n = ( n + 1 ) & 0x0F;
            }
        }
        
        *iv_off = n;
        
        return( 0 );
    }
    else
    {
        int ret = 0;
    
        if( length % 16 )
            return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );

        CRYPTO_CLOCK_ENABLE;
        CRYPTO->CTRL = 0;
        CRYPTO->WAC = 0;
        CRYPTO->SEQCTRL = 0;
        CRYPTO->SEQCTRLB = 0;
        
        /* Set key */
        MBEDTLS_RETVAL_CHK( crypto_setkey_enc (ctx) );

        if( mode == MBEDTLS_AES_DECRYPT )
        {
            /* Load IV */
            CRYPTO_DataWrite( cryptoRegDATA2, (uint32_t*) iv);

            /* Load instructions to CRYPTO sequencer. */
            CRYPTO->SEQ0 =
              CRYPTO_CMD_INSTR_DATA2TODATA0    << _CRYPTO_SEQ0_INSTR0_SHIFT |
              CRYPTO_CMD_INSTR_AESENC          << _CRYPTO_SEQ0_INSTR1_SHIFT |
              CRYPTO_CMD_INSTR_DATA1TODATA0XOR << _CRYPTO_SEQ0_INSTR2_SHIFT |
              CRYPTO_CMD_INSTR_DATA1TODATA2    << _CRYPTO_SEQ0_INSTR3_SHIFT;
            CRYPTO->SEQ1 = 0;
          
            crypto_aes_processloop(length,
                                   cryptoRegDATA1, (uint32_t *)input,
                                   cryptoRegDATA0, (uint32_t *)output,
                                   (uint32_t*)iv );
        }
        else
        {
            /* Load IV */
            CRYPTO_DataWrite( cryptoRegDATA0, (uint32_t*) iv);
            
            /* Load instructions to CRYPTO sequencer. */
            CRYPTO->SEQ0 =
              CRYPTO_CMD_INSTR_AESENC          << _CRYPTO_SEQ0_INSTR0_SHIFT |
              CRYPTO_CMD_INSTR_DATA1TODATA0XOR << _CRYPTO_SEQ0_INSTR1_SHIFT;
            
            crypto_aes_processloop(length,
                                   cryptoRegDATA1, (uint32_t *)input,
                                   cryptoRegDATA0, (uint32_t *)output,
                                   0 );
            
            /* Copy last output block to iv */
            memcpy (iv, &output[length-16], 16);
        }

    cleanup:
        CRYPTO_CLOCK_DISABLE;
        
        return( ret );
    }
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                            int mode,
                            size_t length,
                            unsigned char iv[16],
                            const unsigned char *input,
                            unsigned char *output )
{
    unsigned char c;
    unsigned char ov[17];

    while( length-- )
    {
        memcpy( ov, iv, 16 );
        mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );

        if( mode == MBEDTLS_AES_DECRYPT )
            ov[16] = *input;

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if( mode == MBEDTLS_AES_ENCRYPT )
            ov[16] = c;

        memcpy( iv, ov + 1, 16 );
    }

    return( 0 );
}
#endif /*MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    if (( length & 0xf ) || (nc_off && *nc_off) )
    {
        int c, i;
        size_t n = *nc_off;
    
        while( length-- )
        {
            if( n == 0 )
            {
                mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, nonce_counter, stream_block );

                for( i = 16; i > 0; i-- )
                    if( ++nonce_counter[i - 1] != 0 )
                        break;
            }
            c = *input++;
            *output++ = (unsigned char)( c ^ stream_block[n] );

            n = ( n + 1 ) & 0x0F;
        }

        *nc_off = n;

        return( 0 );
    }
    else
    {
        int ret = 0;
        
        CRYPTO_CLOCK_ENABLE;
        CRYPTO->CTRL = 0;
        CRYPTO->WAC = 0;
        CRYPTO->SEQCTRL = 0;
        CRYPTO->SEQCTRLB = 0;
        
        /* Set key */
        MBEDTLS_RETVAL_CHK( crypto_setkey_enc (ctx) );
        
        /* Set AES-256 mode */
#if 1
        BUS_RegMaskedClear(&CRYPTO->CTRL, _CRYPTO_CTRL_INCWIDTH_MASK);
        BUS_RegMaskedSet(&CRYPTO->CTRL, CRYPTO_CTRL_INCWIDTH_INCWIDTH4);
#else
        CRYPTO->CTRL |= CRYPTO_CTRL_INCWIDTH_INCWIDTH4;
#endif
        
        CRYPTO_DataWrite( cryptoRegDATA1, (uint32_t*)nonce_counter );
        
        CRYPTO->SEQ0 = CRYPTO_CMD_INSTR_DATA1TODATA0  << _CRYPTO_SEQ0_INSTR0_SHIFT |
                       CRYPTO_CMD_INSTR_AESENC        << _CRYPTO_SEQ0_INSTR1_SHIFT |
                       CRYPTO_CMD_INSTR_DATA0TODATA3  << _CRYPTO_SEQ0_INSTR2_SHIFT |
                       CRYPTO_CMD_INSTR_DATA1INC      << _CRYPTO_SEQ0_INSTR3_SHIFT;

        CRYPTO->SEQ1 = CRYPTO_CMD_INSTR_DATA2TODATA0XOR << _CRYPTO_SEQ1_INSTR4_SHIFT;

        crypto_aes_processloop( length,
                                cryptoRegDATA2, (uint32_t*)input,
                                cryptoRegDATA0, (uint32_t*)output,
                                0);
      
        CRYPTO_DataRead( cryptoRegDATA1, (uint32_t*)nonce_counter );

    cleanup:
        
        CRYPTO_CLOCK_DISABLE;
        
        return ret;
    }
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* MBEDTLS_AES_ALT */

#endif /* MBEDTLS_AES_C */
