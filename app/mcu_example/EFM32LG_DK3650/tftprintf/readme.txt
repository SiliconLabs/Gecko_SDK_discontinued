Printf output on TFT-LCD.

This example project uses the EFM32 CMSIS including emlib and the
board support package support functions/drivers to demonstrate driving
the kit's TFT-display from the EFM32 Leopard Gecko. In this case the
display is used as output from a regular printf in the application, as
an alternative to the RS232 port.

This example is driving the display in what is called "address mapped
mode" (or mode 8080). Using this mode, the framebuffer resides in the
memory of the SSD2119 controller chip of the URT TFT-LCD display.

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
