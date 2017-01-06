Conway's game of life, TFT direct drive example.

This example demonstrate driving the EFM32GG_DK37850 kit's TFT-display 
from the EFM32 Leopard Gecko.

This example is driving the display in "direct drive" (or mode generic in 
SSD2119 terms). Using this mode, the framebuffer resides in the external
PSRAM memory of the development kit. This can be accessed directly to 
modify the screen contents.

This demo demonstrates the use of frame buffer control, and implements
Conway's Game of Life, with triple buffering support. Triple buffering
works the following way
  
 We have 3 frame buffers, A, B and C.

 Buffer A, is currently being displayed on the screen
 Buffer B, is being updated with graphics

 When B is finished - B is set to be displayed on the next frame update
                      C is set to be updated with new graphics

 When C is finished - C is set to be displayed on the next frame update 
                      A is set to be updated with new graphics

WARNING:
SD2119 driver and GLIB graphics library are not intended for production 
purposes, and are included here to illustrate TFT display driving only.

These components are subject to changes in API/usage and there will be 
no effort to keep compatibility, or to support this software in any way.

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
