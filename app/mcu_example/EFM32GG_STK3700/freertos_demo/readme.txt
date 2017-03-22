FreeRTOS - Demo

This is the demo application which is also provided in the official FreeRTOS 
distribution. The demo can operate in 3 modes showing different functionality 
of the FreeRTOS system. The mode can be configured by changing the value of 
configCREATE_LOW_POWER_DEMO in the FreeRTOSConfig.h file. 

0 => Full Demo

This is the default mode of this example. The "Full Demo" is a comprehensive 
test and demo application that shows how to use various FreeRTOS API's. When 
running the demo there is a check task which toggles a LED on the board every
3 seconds when the demo is running as expected.

1 => Low Power using RTC

This mode is using the low power tickless idle functionality of FreeRTOS to 
show how to use low power while using FreeRTOS. In this mode the MCU will 
sleep in EM2 whenever FreeRTOS enter idle mode and it will be using the RTC to 
trigger wakeup.

2 => Low Power using BURTC

This mode is using the low power tickless idle functionality of FreeRTOS to 
show how to use low power while using FreeRTOS. In this mode the MCU will 
sleep in EM3 whenever FreeRTOS enter idle mode and it will be using the BURTC 
to trigger wakeup.

Further information on the example can be found at this location:
http://www.freertos.org/EFM32-Giant-Gecko-Pearl-Gecko-tickless-RTOS-demo.html

Board:  Silicon Labs EFM32GG_STK3700 Starter Kit
Device: EFM32GG990F1024