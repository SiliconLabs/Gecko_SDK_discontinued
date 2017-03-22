Audio .WAV format player from microSD card.

This example project uses the EFM32 CMSIS including DK BSP (board 
support package) and demonstrates how to play a wav file from the 
microSD-card.

The wav file must be named "sweet1.wav" and must be encoded with 16-bit
PCM audio sampling.

The output volume level is adjusted with kit pushbuttons PB1 and PB2.
Output level is indicated by the 14 leftmost user LEDs.
Push the AEM button on the kit until "EFM" shows in the upper right
corner of the TFT display to activate PB1 and PB2 pushbuttons.

It sets up access to DVK registers, and supports fat-filesystem on the
sd-card.

Board:  Silicon Labs EFM32G-DK3550 Development Kit
Device: EFM32G890F128
