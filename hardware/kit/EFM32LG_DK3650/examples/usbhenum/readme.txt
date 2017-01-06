USB device enumerator example.

This example project uses the USB host stack to implement
a simple device enumerator.

Connect a terminal to the DK UART port (115.200, N, 8, 1) and
then connect any USB device to the USB port of the MCU plug-in module.

Info about the VID/PID and some string descriptors will be output on
the serial port. The connected USB device will not be configured.

Note that USB disk drives may fail, as some of them draw too much current
when attached.

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
