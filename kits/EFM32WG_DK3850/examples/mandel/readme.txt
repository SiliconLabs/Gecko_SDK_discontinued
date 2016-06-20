Mandelbrot image zoom example, using TFT direct drive.

This example demonstrate driving the EFM32GG_DK37850 kit's TFT-display 
from the EFM32 Wonder Gecko.

The example is driving the display in "direct drive" (or mode generic in 
SSD2119 terms). Using this mode, the framebuffer resides in the external
PSRAM memory of the development kit. This can be accessed directly to 
modify the screen contents.

This example makes a simple mandelbrot fractal zoomer, using up most 
of the external 4MB PSRAM to store the various images, where the screens
are being flipped around in the interrupt routine.

WARNING:
SD2119 driver and GLIB graphics library are not intended for production 
purposes, and are included here to illustrate TFT display driving only.
These components are subject to changes in API/usage and there will be 
no effort to keep compatibility, or to support this software in any way.

Board:  Silicon Labs EFM32WG-DK3850 Development Kit
Device: EFM32WG990F256
