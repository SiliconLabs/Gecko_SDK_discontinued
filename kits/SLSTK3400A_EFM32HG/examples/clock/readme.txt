Analog and digital clock example using the Memory LCD on the SLSTK3400A-EFM32HG.

This example project uses the EFM32 CMSIS including emlib and the
board support package support functions/drivers to demonstrate driving
the Sharp Memory LCD display on the SLSTK3400A_EFM32HG kit.

The user can switch between analog and digital clock modes by pushing
the PB0 button.

The user can increment the second count by pushing the PB1 button.
The minute count can be incremented by pushing the PB1 button for more
than 2 seconds.

The analog clock mode demonstrates the use of GLIB to draw hour, minute
and second pointers on the background which is a bitmap in flash memory.

The digital clock demonstrates the 16x20 font (including number characters
only) of the textdisplay driver.

Board:  Silicon Labs SLSTK3400A-EFM32HG Development Kit
Device: EFM32HG322F64
