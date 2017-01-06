TFT-LCD graphics demo.

This example project uses the EFM32 CMSIS including the DVK BSP (board
support package) and demonstrates driving the DVK kit TFT-display from 
the EFM32.

The G290 variant of this project uses EBI to drive the display controller
through a direct memory mapped interface.

The G890 variant of this project uses USART SPI to drive the display 
controller. This greatly reduces update speed compared to the EBI 
variant.

WARNING:

SD2119 driver and GLIB graphics library are not intended for production 
purposes, and are included here to illustrate TFT display driving only.
These components are subject to changes in API/usage and there will be 
no effort to keep compatibility, or to support this software in any way.

Board:  Silicon Labs EFM32-G2xx-DK Development Kit
Device: EFM32G290F128
Board:  Silicon Labs EFM32-G8xx-DK Development Kit
Device: EFM32G890F128
