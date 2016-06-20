USB Host HID keyboard example.

This example project uses the USB host stack to implement
support for USB HID Keyboards.

Connect a terminal to the DK UART port (115.200, N, 8, 1) and
then connect a USB keyboard to the USB port of the MCU plug-in module.

The example will flash keyboard leds (numlock, scrollock and capslock)
with a binary counting pattern.

Keyboard entry will be echoed on the UART serial port.

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
