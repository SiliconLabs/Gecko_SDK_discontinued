I2C temperature sensor example.

This example project uses the EFM32 CMSIS and emlib including DK BSP
(board support package) to demonstrate the use of the I2C bus and 
the temperature sensor on the DK.

Note: On some DK boards the temperature sensor is placed too close 
to components that generate significant heat. The sensor may therefore
read a temperature 2-4 degrees Celsius higher than the ambient temperature.

The following configuration is possible:

- Joystick push toggles between Fahrenheit and Celsius display mode.

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
