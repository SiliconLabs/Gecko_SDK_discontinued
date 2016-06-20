README for mbed TLS CRYPTO hardware acceleraton plugins from Silicon Labs
=========================================================================

Introduction
------------
This folder include alternative implementations (plugins) of some mbed TLS functions. The supported plugins are:
- sl_ecp.c     
- sl_aes.c
- sl_sha256.c
- sl_sha1.c

sl_ecp.c includes alternative implementations of some functions in the 'mbedtls/library/ecp.c' file in order to accelerate ECC point multiplication (the mbedtls_ecp_mul function) on 3 Short Weierstrass curves, namely secp192r1, secp224r1 and secp256r1.

sl_aes includes alternative implementations of the functions defined in 'mbedtls/include/mbedtls/aes.h' in order to accelerate AES interface of mbedtls.

sl_sha1.c includes alternative implementations of the functions defined in 'mbedtls/include/mbedtls/sha1.h' in order to accelerate SHA-1 interface of mbedtls.

sl_sha256.c includes alternative implementations of the functions defined in 'mbedtls/include/mbedtls/sha256.h' in order to accelerate SHA-256 interface of mbedtls.

NOTE:
The current versions of the CRYPTO plugins do _not_ support multi-threading. I.e. the user application is responsible for not calling the mbedtls interfaces using CRYPTO plugins with more than one thread concurrently.


Configuration
-------------

Please refer to the main README file (in the mbedtls base folder) and `include/mbedtls/config.h` for general mbed TLS configuration. The CRYPTO hardware acceleraton plugins can be configured by building with the following macro definitions:

#define MBEDTLS_AES_ALT   /* Includes CRYPTO accelerated implementation of the aes.h interface. */
#define MBEDTLS_SHA1_ALT   /* Includes CRYPTO accelerated implementation of the sha1.h interface. */
#define MBEDTLS_SHA256_ALT   /* Includes CRYPTO accelerated implementation of the sha256.h interface. */
#define MBEDTLS_SHA256_PROCESS_ALT   /* Includes CRYPTO accelerated implementation of the mbedtls_sha256_process function. */
#define MBEDTLS_ECP_DEVICE_ALT   /* Includes CRYPTO accelerated implementation of some functions in ecp.c. */
#define MBEDTLS_ECP_DOUBLE_JAC_ALT /* Includes CRYPTO accelerated implementation of the ecp_double_jac function. */
#define MBEDTLS_ECP_DEVICE_ADD_MIXED_ALT /* Includes CRYPTO accelerated implementation of the ecp_add_mixed function. */
#define MBEDTLS_ECP_NORMALIZE_JAC_ALT /* Includes CRYPTO accelerated implementation of the ecp_normalize_jac function. */
#define MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT /* Includes CRYPTO accelerated implementation of the ecp_normalize_jac_many function. */
#define MBEDTLS_MPI_MODULAR_DIVISION_ALT /* Includes the CRYPTO accelerated function mbedtls_mpi_div_mod which can serve as a replacement of the regular mbedtls_mpi_inv_mod function of mbedtls. */


Compiling
---------

Please refer to the example programs in the EFM32 SDK for project files examples for various IDE/toolchains.
I.e:
kits/SLSTK3401A_EFM32PG/examples/mbedtls_aescrypt
kits/SLSTK3401A_EFM32PG/examples/mbedtls_ecdsa
