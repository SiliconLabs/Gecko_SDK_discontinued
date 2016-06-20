Joystick control example.

This example project uses the EFM32 CMSIS including DVK BSP (board
support package) and demonstrates the use of the BSP for accessing
joystick movement.

It sets up access to DVK registers, enables an interrupt to register
joystick movement which are indicated on the user leds.

Note! To enable the EFM32 to read the joystick and push buttons, the
AEM button on the kit must be pressed to indicate "Keys: EFM32" in the
upper right corner of the TFT-LCD.

Board:  Silicon Labs EFM32-Gxxx-DK Development Kit
Device: EFM32G290F128 and EFM32G890F128
