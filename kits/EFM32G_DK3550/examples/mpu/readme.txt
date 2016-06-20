Cortex-M3 Memory Protection Unit example.

This example configures the MPU to demonstrate usage of this peripheral
to catch memory access violations. The serial port is used to create and
report on access violations, on LEUART1.

Connect a terminal on RS232 port with baudrate 9600-8-N-1. This demo
requires the use of the RS232 port in LEUART mode, UART mode will not
work.

The example will also use Energy Mode 2 (EM2).

Board:  Silicon Labs EFM32G_DK3550 Development Kit
Device: EFM32G890F128

