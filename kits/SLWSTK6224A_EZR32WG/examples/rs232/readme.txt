VCOM/USART/LEUART RS232 output example.

This example project uses CMSIS and emlib, to demonstrates the
use of LEUART, USART or VCOM  communication on the SLWSTK6224A_EZR32WG
development kit.

Serial port selection is done by defining any one of compiletime macros:
#define RETARGET_LEUART0
#define RETARGET_USART1
#define RETARGET_USART2
#define RETARGET_VCOM       (default)

If a USART is used set terminal configuration to 115200-8-N-1.
If the LEUART is used set terminal configuration to 9600-8-N-1.
For the default VCOM communication mode, serial port settings are "dont care".

Board:  Silicon Labs SLWSTK6224A_EZR32WG Development Kit
Device: EZR32WG330F256R63
