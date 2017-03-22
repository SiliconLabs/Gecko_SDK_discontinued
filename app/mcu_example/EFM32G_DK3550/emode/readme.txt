Static Energy Mode example, select a single EM level.

This example project uses the EFM32 to demonstrate the use of the
LCD controller, RTC (real time counter), GPIO and various Energy Modes.

A counter ticks down from 4 seconds, and the user can press the joystick
up and down to select the various energy modes, EM0 (busy loop from 
flash), EM1, EM2, EM3, EM4, EM2+RTC, EM2+RTC+LCD. Remember to press the AEM key
to give the EFM32 access to the joystick.

Board:  Silicon Labs EFM32G_DK3550 Development Kit
Device: EFM32G890F128

