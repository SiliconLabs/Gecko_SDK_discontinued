README for mbed TLS CRYPTO hardware acceleraton plugins from Silicon Labs
=========================================================================

Introduction
------------
This folder include alternative implementations (plugins) from Silicon Labs for some of the mbed TLS library functions, including AES, CCM, CMAC, ECC (ECP, ECDH, ECDSA, ECJPAKE), SHA1 and SHA256. The plugins use the AES and CRYPTO hardware modules to accelerate the standard mbed TLS library functions that are implemented in C. The CRYPTO hardware module is incorporated in the new generations of MCUs from SIlicon Labs and the AES hardware module is incorporated in the classic EFM32 MCUs from Silicon Labs.

In addition there is software support for the True Random Number Generator (TRNG) incorporated from Series 1 Configuration 2 devices (EFR32MG12, etc.). The TRNG module implements an entropy source plugin module for mbed TLS that can be used in applications needing random numbers or indirectly using mbed TLS modules that depend on the random number generation interfaces of mbed TLS.

Please refer to the mbedTLS section of the Gecko SDK documentation for more information on using mbed TLS on Silicon Labs devices.

Configuration
-------------
The configuration macros of the CRYPTO hardware acceleraton plugins from Silicon Labs are documented in configs/config-sl-crypto-all-acceleration.h. The configs/config-sl-crypto-all-acceleration.h enables acceleration of all plugins from Silicon Labs and provides a full mbed TLS configuration which can be used to get started with application development. Please refer to the Gecko SDK documentation for more information on using mbed TLS on Silicon Labs devices. Please refer to the main README file (in the mbed TLS base folder) and `include/mbedtls/config.h` for general mbed TLS configuration.

Compiling
---------
Please refer to the example programs in the EFM32 SDK for project files examples for various IDE/toolchains.
I.e:
kits/SLSTK3401A_EFM32PG/examples/mbedtls_aescrypt
kits/SLSTK3401A_EFM32PG/examples/mbedtls_ecdsa
