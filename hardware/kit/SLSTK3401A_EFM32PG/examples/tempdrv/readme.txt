Display temperature range in Energy Mode 3 using SLSTK3401A_EFM32PG board.

In this example project temperature is being sampled and the correct
temperature range will be displayed on the LCD.
Following messages will be displayed on the LCD based on the temperature
reading:
  1. Below 0 degC
  2. Above 30 degC
  3. Above 85 degC
  4. Between 0 & 30 degC

The example project uses temperature driver(tmpdrv) to monitor 
temperature and a display module to display the correct 
temperature range on the LCD. tempdrv allows us to specify 
the temperature level and register a callback if 
the temperature increases or decreases from the specified threshold.
Moreover, after displaying the right message on the LCD the board goes
to Energy mode 3 to preserve the power consumption.  

The project uses the EFM32 CMSIS including emlib, the board support and 
the package support functions/drivers to demonstrate the use of 
tempdrv and the LCD display.

Board:  Silicon Labs SLSTK3401A_EFM32PG starter kit
Device: EFM32PG1B200F256GM48
