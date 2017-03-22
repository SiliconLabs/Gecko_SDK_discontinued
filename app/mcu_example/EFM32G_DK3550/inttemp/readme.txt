Internal temperature sensor example.

This example project uses the EFM32 CMSIS and emlib including DVK
BSP (board support package) and demonstrates the use of the internal
temperature sensor and ADC on the EFM32.

Note: EFM32 devices with production ID lower than 18 may measure inaccurate
temperatures due to a sensor calibration bug.

The following configuration is possible:

- Joystick push toggles between Fahrenheit and Celsius display mode.

Board:  Silicon Labs EFM32G_DK3550 Development Kit
Device: EFM32G890F128
