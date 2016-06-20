Energy Modes with LCD display example.

This example project use EFM32 CMSIS and the emlib peripheral library to
demonstrate the use of the LCD, RTC (real time counter), GPIO and
various Energy Modes on the starter kit.

The example will cycle through Energy Mode EM0-EM3, staying one second in
each mode. PB0 can be pressed anytime to enter EM4.
Wakeup from EM4 is either by reset pushbutton or by GPIO pin (port C bit 9)
connected to pushbutton PB1.

Board:  Silicon Labs SLSTK3400A-EFM32HG Development Kit
Device: EFM32HG322F64
