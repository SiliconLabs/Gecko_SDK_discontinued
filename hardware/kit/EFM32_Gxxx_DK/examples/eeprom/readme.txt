I2C Access to EEPROM example.

This example project uses the EFM32 CMSIS and emlib including DVK BSP
(board support package) and demonstrates the use of the I2C bus accessing
the EEPROM on the DVK.

The first 3 bytes of the EEPROM is displayed on the LCD screen (This will
normally be FFFFFF first time.) Since stored in EEPROM, the data survives
resets and power cycles.

The following usage is possible:

- Joystick up/down will increment/decrement the first byte (Byte0) in the
  EEPROM. Byte1 is then set to Byte0+1, and Byte2 is set to Byte1+1.
- Press SW4 button to reset 3 first bytes to 0xFF.

Board:  Silicon Labs EFM32-G8xx-DK Development Kit
Device: EFM32G890F128 w/LCD only
