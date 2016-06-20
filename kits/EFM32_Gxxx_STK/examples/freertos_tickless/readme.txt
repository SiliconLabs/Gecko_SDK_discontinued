FreeRTOS - Tickless example

This example project uses the FreeRTOS, and gives a basic demonstration of using two tasks,
one sender generating number and one receiver that displays the number on LCD. The FreeRTOS is 
configured in tick-less mode, going into EM2 when no tasks are active (  is possible to use 
EM1 or EM2 in this example, EM3 mode can't work, because LCD driver and timer don't work in that mode- configured 
in FreeRTOSConfig.h file ). 
This example is intended as a skeleton for new projects using FreeRTOS for energy aware applications. 

Board:  Silicon Labs EFM32-Gxxx-STK Development Kit
Device: EFM32G890F128