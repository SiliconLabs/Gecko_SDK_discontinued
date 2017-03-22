Board Support Package API demo for voltage and current readout.

This example project uses the EFM32 CMSIS and demonstrates the use of
the STK BSP.

The BSP is used to read out the current consumption and VMCU voltage level
from the board controller. The readings are printed to the display.

The CPU runs from the HFRCO clock and change HFRCO frequency every few 
seconds to show how CPU frequency affects power consumption.

The BSP use the LEUART at 115800-N-1 to communicate with the board controller.

Board:  Silicon Labs SLSTK3400A_EFM32HG Starter Kit
Device: EFM32HG322F64
