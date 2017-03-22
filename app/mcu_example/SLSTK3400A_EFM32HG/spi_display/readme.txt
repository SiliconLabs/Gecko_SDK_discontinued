Toggle all pixels on the LCD display every 500ms with low energy peripherals.

This example project uses EFM32 CMSIS and the emlib peripheral library to 
demonstrate driving the Sharp Memory LCD display on the STK3200 kit while 
staying in low energy modes.

A DMA channel is used to transfer frame buffers to the LCD display with the 
SPI protocol, staying in EM1 during the data transfer and in EM3 while
waiting for the next frame update. An RTC interrupt toggles the EXTCOM pin 
60 times per second as required by the display.

Board:  Silicon Labs STK3200 Starter Kit
Device: EFM32HG322F64