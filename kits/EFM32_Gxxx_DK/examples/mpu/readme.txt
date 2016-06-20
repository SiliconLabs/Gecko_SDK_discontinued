Cortex-M3 Memory Protection Unit example.

This example project uses the EFM32 CMSIS including DVK BSP (board
support package).

It sets up the MPU to demonstrate usage of MPU to catch memory access 
violations. Serial port b (LEUART1) is used to create and report on 
access violations.

Connect a terminal emulator on serial port b with configuration
9600-8-N-1 to run the example.

The example also use Energy Mode 2 (EM2).

Board:  Silicon Labs EFM32-Gxxx-DK Development Kit
Device: EFM32G290F128 and EFM32G890F128

