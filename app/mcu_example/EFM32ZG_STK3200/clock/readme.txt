Analog and digital clock example using the Memory LCD on the EFM32ZG-STK3200.

This example project uses the EFM32 CMSIS including emlib and the
board support package support functions/drivers to demonstrate driving
the Sharp Memory LCD display on the EFM32ZG_STK3200 kit.

The user can switch between analog and digital clock modes by pushing
the PB0 button.

The user can increment the second count by pushing the PB1 button.
The minute count can be incremented by pushing the PB1 button for more
than 2 seconds.

The analog clock mode demonstrates the use of GLIB to draw hour, minute
and second pointers on the background which is a bitmap in flash memory.

The digital clock demonstrates the 16x20 font (including number characters
only) of the textdisplay driver.

Board:  Silicon Labs EFM32ZG-STK3200 Development Kit
Device: EFM32ZG222F32
