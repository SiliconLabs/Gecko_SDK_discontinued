VCOM/USART/LEUART RS232 output example.

This example project uses CMSIS and emlib, to demonstrates the
use of LEUART, USART or VCOM  communication on the SLWSTK6240A_EZR32HG
wireless starter kit.

Serial port selection is done by defining any one of compiletime macros:
#define RETARGET_LEUART0
#define RETARGET_USART0
#define RETARGET_VCOM       (default)

If a USART is used set terminal configuration to 115200-8-N-1.
If the LEUART is used set terminal configuration to 9600-8-N-1.
For the default VCOM communication mode, serial port settings are "dont care".

Board:  Silicon Labs SLWSTK6240A_EZR32HG Starter Kit
Device: EZR32HG320F64R61
