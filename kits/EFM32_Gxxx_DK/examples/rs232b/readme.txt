LEUART output on RS232 port B.

This example project uses the EFM32 CMSIS including DVK BSP (board
support package).

It sets up access to DVK registers, and toggles access to serial 
port b. 

Connecting a terminal emulator on serial port b, with configuration 
9600-8-N-1 will show a printf output and provide a "local echo" on typed 
characters.

Board:  Silicon Labs EFM32-Gxxx-DK Development Kit
Device: EFM32G290F128 and EFM32G890F128

