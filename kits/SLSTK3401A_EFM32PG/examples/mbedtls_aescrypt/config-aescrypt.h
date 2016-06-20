/*
 * Minimal configuration for AES test on SiliconLabs devices incorporating
 * CRYPTO hardware accelerator.
 *
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#if !defined( NO_CRYPTO_ACCELERATION )
/* SiliconLabs plugins with CRYPTO acceleration support. */
#define MBEDTLS_AES_ALT
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA256_PROCESS_ALT
#define MBEDTLS_ECP_DEVICE_ALT
#define MBEDTLS_ECP_DOUBLE_JAC_ALT
#define MBEDTLS_ECP_DEVICE_ADD_MIXED_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
#define MBEDTLS_MPI_MODULAR_DIVISION_ALT
#endif

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_MODE_CFB
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_SHA256_C
#define MBEDTLS_MD_C

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
