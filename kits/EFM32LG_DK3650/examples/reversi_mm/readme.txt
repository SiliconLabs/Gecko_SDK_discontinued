emWin example - reversi game - energy efficient.

This example project uses the emWin library maintaining
low power consumption.

TFT driver is controlled in so called "Memory Map"
configuration, where internal RAM of TFT controller 
is used. There is no additional framebuffer needed
so this example could be run in small RAM footprint 
systems. The code fits 128kB flash and uses less 
than 32kB RAM.

In idle mode (where no user interaction is required)
EM2 mode is enabled, allowing reducing power consumption
down to 4uA.

The game is controlled using touch pad or joystick. 
If there is misalignment when using touch pad please
do calibration - from menu select:
Options->Touch Panel calibration.

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
