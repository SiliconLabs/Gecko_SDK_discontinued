USART/LEUART RS232 output example.

This example project uses the EFM32 CMSIS and emlib, to demonstrates the
use of USART and LEUART communication on the SLSTK3401A Starter Kit, as well
as using the virtual COM port on the Starter Kit.

For the default USART communication, serial configuration 115200-8-N-1 is used.
(EXP pin 1 is GND, EXP pin 12 is USART0 Tx and EXP pin 14 is USART0 Rx).
By defining "RETARGET_VCOM" as a build option, a virtual COM port through the
USB cable is enabled.
If LEUART mode is configured by defining "RETARGET_LEUART0" as a build option,
terminal configuration is 9600-8-N-1 if "RETARGET_VCOM" is not defined, and
115200-8-N-1 if "RETARGET_VCOM" is defined.

Board:  Silicon Labs SLSTK3401A Starter Kit
Device: EFM32PG1B200F256GM48
