emWin example - reversi game.

This example project uses the emWin library.

TFT driver is controlled in so called "Memory Map"
configuration, where internal RAM of TFT controller 
is used. There is additional framebuffer needed so 
this example could be run in small RAM footprint 
systems. The code fits 128kB flash and uses less 
than 32kB RAM.

In idle mode (where no user interaction is required)
EM2 mode is enabled, allowing reducing power consuption
down to 20uA.

The game is controlled using touch pad or joystick. 
If there is misalignment when using touch pad please
do calibration - from menu select:
Options->Touch Panel calibration.

Board:  Silicon Labs EFM32G-DK3550 Development Kit
Device: EFM32G890F128

