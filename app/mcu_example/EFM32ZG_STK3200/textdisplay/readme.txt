Printf output on Sharp Memory LCD LS013B7DH03.

This example project uses the EFM32 CMSIS including emlib and the
board support package support functions/drivers to demonstrate driving
the Sharp Memory LCD display on the EFM32 Zero Gecko. In this case the
display is used as output from a regular printf in the application, as
an alternative to the RS232 port.

The user can customize the behaviour of the DISPLAY driver by changing
configuration macros in displayconfigapp.h.

Board:  Silicon Labs EFM32ZG-STK3200 Development Kit
Device: EFM32ZG222F32
