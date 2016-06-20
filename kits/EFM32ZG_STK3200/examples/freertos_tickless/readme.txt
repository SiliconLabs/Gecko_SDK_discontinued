FreeRTOS - Tickless example

This example project uses the FreeRTOS, and gives a basic demonstration of using two tasks,
one sender generating number and one receiver that displays the number on the display. The FreeRTOS is 
configured in tickless mode, going into EM2 when no tasks are active.

EM3 mode is not used in this example because timers are not available for the display driver in EM3.
For more details, see the configuraiton in FreeRTOSConfig.h file.

Board:  Silicon Labs EFM32ZG_STK3200 Starter Kit
Device: EFM32ZG222F32