PRS example.

This example project use EFM32 CMSIS and the emlib peripheral library
to demonstrate the use of the Peripheral Reflex System (PRS) on the 
SLSTK3401A kit. The LETIMER0 toggles LED1 via PRS channel 0 at every 250ms.
Button BTN0 toggles LED0 at every 5th press via PCNT0 and PRS channel 2.
This example operates in EM2.

Board:  Silicon Labs SLSTK3401A Starter Kit
Device: EFM32PG1B200F256
