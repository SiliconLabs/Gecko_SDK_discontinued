Cycle through Energy Modes while updating Segment-LCD.

This example project uses the EFM32 CMSIS and demonstrates the use of
the LCD controller, RTC (real time counter), GPIO and various Energy 
Modes (EM).

EM2 is used for delays in between refreshing the LCD display, and a lot
of "eye candy" are present to show off the MCU module's LCD display.

The LCD controller drives the display down to EM2. In the demo, EM2 is 
used for "most" delays to show off this feature. The user can press 
joystick up or down to activate EM3 and EM4. EM4 requires a system 
reset, while pushing joystick again will wake up the system from Energy
Mode 3. Note that the upper right corner of the kit must show 
"Keys: EFM" for the joystick presses to work. Pressing joystick up and
down will not go into EM3 or EM4 immediately, but after going through 
EM0-EM2 first.

Board:  Silicon Labs EFM32G_DK3550
Device: EFM32G890F128
