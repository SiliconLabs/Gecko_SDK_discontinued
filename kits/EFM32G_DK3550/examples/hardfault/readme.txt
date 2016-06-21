Hardfault handler example.

This example project demonstrates the use of the hard fault handler, and
how to install and use another HardFault exception handler to provide
useful information when these types of errors occurs.
The information is transmitted on the kit UART port, serial configuration
115200-8-N-1 is used.

Three function calls for triggering software errors to be trapped is
provided, comment them in and out to select between them.

Board:  Silicon Labs EFM32G-DK3550 Development Kit
Device: EFM32G890F128

