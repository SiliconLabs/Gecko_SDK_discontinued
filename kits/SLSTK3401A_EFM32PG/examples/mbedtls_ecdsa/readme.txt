mbedTLS ECDSA example using the CRYPTO module in the EFM32PG in order to
accelerate the ECDSA API functions of mbedTLS.

This example project uses the EFM32 CMSIS including emlib to run
mbedTLS libraries on the EFM32PG.

The example redirects standard I/O to the VCOM virtual serial port of the
SLSTK3401A_EFM32PG kit. The user can use a terminal emulator program to
connect to the serial port with the default settings of 115200-8-N-1.

The example has been instrumented with code to count the number of clock
cycles spent inside the ecdsa api calls, mbedtls_ecdsa_genkey,
mbedtls_ecdsa_write_signature and mbedtls_ecdsa_read_signature.
The results are printed to stdout, i.e. the VCOM serial port console.
In order to check the performance gain of CRYPTO acceleration, the user
can switch off CRYPTO hardware acceleration by compiling with
-DNO_CRYPTO_ACCELERATION in the compiler command line.

The user can change the specific ECC curve used in the example
by modifying the ECPARAMS macro definition. Available curves with
CRYPTO acceleration support are:
MBEDTLS_ECP_DP_SECP256R1
MBEDTLS_ECP_DP_SECP224R1
MBEDTLS_ECP_DP_SECP192R1

Board:  Silicon Labs SLSTK3401A Starter Kit
Device: EFM32PG1B200F256
