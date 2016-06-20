Analog and digital clock example using the Memory LCD on the SLWSTK6223A_EZR32WG.

This example project uses CMSIS including emlib and the
board support package support functions/drivers to demonstrate driving
the Sharp Memory LCD display on the SLWSTK6223A_EZR32WG kit.

The user can switch between analog and digital clock modes by pushing
the PB0 button.

The user can increment the minute count by pushing the PB1 button.
The minute count can be incremented by pushing the PB1 button for more
than 0.5 seconds.

The analog clock mode demonstrates the use of GLIB to draw hour, minute
and second pointers on the background which is a bitmap in flash memory.

The digital clock demonstrates the 16x20 font (including number characters
only) of the textdisplay driver.

Board:  Silicon Labs SLWSTK6223A_EZR32WG Development Kit
Device: EZR32WG330F256R63
