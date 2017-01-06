Audio WAV-player from microSD card.

This example project uses the EFM32 CMSIS including DVK BSP (board support 
package) and demonstrates how to play a wav file from the SD-card.

The wav file must be named "sweet1.wav" and must be encoded with 16-bit
PCM audio sampling.

It sets up access to DVK registers, and supports fat-filesystem
on the sd-card.

Note! On some versions of the Development Kit, you need to remove the prototype 
board for this example to work correctly.

Board:  Silicon Labs EFM32-Gxxx-DK Development Kit
Device: EFM32G290F128 and EFM32G890F128
