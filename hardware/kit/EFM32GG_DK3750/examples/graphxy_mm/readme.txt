emWin example - XY Graph demonstration - energy efficient.

This example project uses the emWin library maintaining
low power consumption.

TFT driver is controlled in so called "Memory Map"
configuration, where internal RAM of TFT controller 
is used. There is no additional framebuffer needed
so this example could be run in small RAM footprint 
systems. The code fits 128kB flash and uses less 
than 32kB RAM.

In idle mode (where no user interaction is required)
EM2 mode is enabled, allowing reducing power consuption
down to ~4uA.

Control using touch panel or joystick.
On application startup there is possibility to
perform touch panel calibration.

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
