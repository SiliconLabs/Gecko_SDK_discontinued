Energy Modes with segment LCD example.

This example project uses the EFM32 CMSIS and demonstrates the use of 
the LCD controller, RTC (real time counter), GPIO and various Energy 
Modes (EM).

EM2 is used for delays in between refreshing the LCD display, and a lot
of "eye candy" are present to show off the MCU module's LCD display.

The LCD controller drives the display down to EM2. In the demo, EM2 is 
used for "most" delays to show off this feature. The user can press PB0
or PB1 to activate EM3 and EM4. EM4 requires a system reset, while PB0 
again will wake up the system from Energy Mode 3.

Board:  Silicon Labs EFM32-Gxxx-STK Development Kit
Device: EFM32G890F128 with LCD 
