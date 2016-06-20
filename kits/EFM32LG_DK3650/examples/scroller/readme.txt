Horizontal single pixel text scroller example.

This example demonstrate driving the EFM32LG_DK3650 kit's TFT-display
from the EFM32 Leopard Gecko.

This example is driving the display in "direct drive" (or mode generic in
SSD2119 terms). Using this mode, the framebuffer resides in the external
PSRAM memory of the development kit. This can be accessed directly to
modify the screen contents.

This demo demonstrates the use of frame buffer control, and implements
a horizontal scroller and shows the hardware assisted masking and
blending capabilities.

The horizontal scroller is implemented by using a large framebuffer,
which is shifted right one pixel for each horizontal scan line, using
the horizontal sync interrupt, and frame base sync trigger  capability
of the Leopard Gecko devices.

The masking and blending is hardware assisted. The geckos being drawn
on screen are using the same software procedure, only adding mask and
enable configurations.

WARNING:
SD2119 driver and GLIB graphics library are not intended for production
purposes, and are included here to illustrate TFT display driving only.
This component are subject to changes in API/usage and there will be
no effort to keep compatibility, or to support this software in any way.

NOTE:
This example is too large to be built with IDEs with 32KB size limits.

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
