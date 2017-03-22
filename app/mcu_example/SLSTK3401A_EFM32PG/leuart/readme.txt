LEUART/LDMA in Energy mode2 output example.

This example project reads and sends data byte back to terminal using LEUART
while staying in EM2.

It uses the EFM32 CMSIS and emlib, to demonstrate the
use of LEUART communication using LDMA under the EM2 on the 
SLSTK3401A Starter Kit.

The LEUART is configured to be used with LDMA under EM2.
The terminal configuration is 9600-8-N-1.
The LDMA is configured to handle read and write data bytes from LEUART under EM2.

Testing:
1. Use the expansion header to connect the LEUART to your PC.
2. Pin Configuration:
	2.1. Pin-12 (PA0) LEUART0_TX
	2.2. Pin-14 (PA1) LEUART0_RX
	2.3. Pin-1 Gnd

3. Configure terminal to 9600-8-N-1.
4. Enter characters from your keyboard and see if you receive the echo on the terminal.
5. Check the average current consumption is equal to EM2 on Energy Profiler.

Board:  Silicon Labs SLSTK3401A Starter Kit
Device: EFM32PG1B200F256GM48