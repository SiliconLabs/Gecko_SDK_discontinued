Energy Mode example #2.

This example project uses the EFM32 CMSIS and demonstrates  the use of
the LCD controller, RTC (real time counter), GPIO and various Energy
Modes (EM).

A counter ticks down from 4 seconds, and the user can press the joystick
up and down to select the various energy modes, EM0 (busy loop from
flash), EM1, EM2, EM3, EM4, EM2+RTC and EM2+RTC+LCD. Remember to press
the AEM key to give the EFM32 access to the joystick.

Board:  Silicon Labs EFM32-Gxxx-DK Development Kit
Device: EFM32G890F128 with LCD _ONLY_
