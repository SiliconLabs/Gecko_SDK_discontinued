FreeRTOS - Tickless example

This example project uses FreeRTOS to drive and display images on the Sharp 
Memory LCD. The example will create two tasks. One task is drawing a different 
image to the LCD every 3 seconds, while the other task is toggling the 
EXTCOMIN pin on the Sharp Memory LCD.

FreeRTOS is configured using the FreeRTOSConfig.h file in the example project 
to use the tickless idle functionality. So when FreeRTOS is idle it will enter 
EM2 and use the RTC to wakeup on the next event.

Board:  Silicon Labs EFM32ZG_STK3200 Starter Kit
Device: EFM32ZG222F32