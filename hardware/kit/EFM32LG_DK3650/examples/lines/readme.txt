Lines TFT Direct Drive example.

This example demonstrate driving the EFM32GG_DK37850 kit's TFT-display
from the EFM32 Leopard Gecko in "direct drive" (or mode generic in
SSD2119 terms). Using this mode, the framebuffer resides in the external
PSRAM memory of the development kit. This can be accessed directly to
modify the screen contents.

In this mode, the display is configured for having 16-bits per pixel,
RGB with 5-6-5 bits color components of each. The framebuffer is located
at EBI bank 2, offset address 0x88000000. This demo simply draws some
alternating, coloured lines over the entire display.

WARNING:
SD2119 driver and GLIB graphics library are not intended for production 
purposes, and are included here to illustrate TFT display driving only.

These components are subject to changes in API/usage and there will be 
no effort to keep compatibility, or to support this software in any way.

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
