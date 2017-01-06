USB Mass Storage Device example.

This example project use the EFM32 USB Device protocol stack
to implement a device Mass Storage Class device (MSD).

The example implements a small 32KByte "disk" using internal FLASH memory.

Led 0 will flash when there is traffic on the USB bus, led 1 will be on when
the device is connected to an USB host.

NOTE: The example does not check if the code segment extends into the
32KByte "disk" part of flash memory. Take care when adding code to the example !

Board:  Silicon Labs SLSTK3400A Starter Kit
Device: EFM32HG322F64
