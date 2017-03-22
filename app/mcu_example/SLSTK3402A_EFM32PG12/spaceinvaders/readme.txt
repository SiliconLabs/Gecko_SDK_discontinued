Space Invaders game using the Memory LCD on the SLSTK3402A.

This example project uses the EFM32 CMSIS including emlib and the
board support package support functions/drivers to demonstrate driving
the Sharp Memory LCD display on the SLSTK3402A Starter Kit.

Game instructions are displayed on the startup screen.
(On kits with slider instead of touch pads, use outermost segments to move tank).

For debug builds the game may run slow because they are not optimized
for speed. The user should build and run the release builds for
optimized performance.

Board:  Silicon Labs SLSTK3402A Starter Kit
Device: EFM32PG12B500F1024GL125
