Board Support Package API example.

This example project use EFM32 CMSIS and the emlib peripheral library to
demonstrate the use of the EFM32TG_STK3300 Binary Support Package (BSP).

The BSP is used to read out the current uA measurement and VMCU voltage level
from the board controller.

The BSP use a 115800-N-1 UART to communicate with the board controller, if you
do not need the board support functions, there is no need to include the BSP
in your project. Currently, the UART communication with 16 x oversampling is
used, limiting the communication channel to high frequencies for the peripheral
clock.

Board:  Silicon Labs EFM32TG_STK3300 Starter Kit
Device: EFM32TG840F32
