emWin example - show gui functionality

This example project uses the emWin library.

TFT driver is controlled in so called "Memory Map"
configuration, where internal RAM of TFT controller
is used. There is additional framebuffer needed so
this example could be run in small RAM footprint
systems. The code fits 128kB flash and uses less
than 32kB RAM.

In idle mode (where no user interaction is required)
EM2 mode is enabled, allowing reduced power consumption.

Simple application showing different visual effects
which could be achieved by means of emWin

Board:  Silicon Labs EFM32G-DK3550 Development Kit
Device: EFM32G890F128
