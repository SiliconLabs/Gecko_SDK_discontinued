Weatherstation using the Sensor add-on board on the EFM32TG-STK3300.

This example project uses the EFM32 CMSIS including emlib and the
board support package support functions/drivers to demonstrate driving
the LCD display on the EFM32TG_STK3300 kit. In addition, it
demonstrates the use of Silabs I2C based sensors on the expansion board
header.

You must have a Silabs Sensor add-on board to make use of this demo.
Once the demo is started, hold your hand over the gesture sensor. Once
the sensor detects your hand the display will become active.

Swipe left and right over the sensor to scroll through temperature, humidity
and UV index. On the temperature screen, you can swipe up or down to change
between Fahrenheit or Celcius.

Note: Due to the size of this example, you might need to enable compiler 
optimizations to be able to fit the entire binary in the 32k of flash!

Board:  Silicon Labs EFM32TG-STK3300 Development Kit and Sensor Add-on board
Device: EFM32TG840F32
