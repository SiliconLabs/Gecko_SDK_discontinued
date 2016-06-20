Weatherstation using the Sensor add-on board on the SLSTK3401A.

This example project uses the EFM32 CMSIS including emlib and the
board support package support functions/drivers to demonstrate driving
the Sharp Memory LCD display on the SLSTK3401A kit. In addition, it
demonstrates the use of Silicon Labs I2C based sensors on the expansion
board header.

You must have a Silicon Labs Sensor add-on board to make use of this demo.
Once the demo is started, hold your hand over the gesture sensor. Once
the sensor detects your hand the display will become active.

Swipe left and right over the sensor to scroll through temperature, humidity
and UV index. On the temperature screen, you can swipe up or down to change
between Fahrenheit or Celcius.

Board:  Silicon Labs SLSTK3401A Starter Kit and Sensor Add-on board
Device: EFM32PG1B200F256GM48
