Pre-amplifier example.

This example project uses the EFM32 CMSIS including DK BSP (board 
support package) and demonstrates the use of the ADC/DAC/DMA/PRS/TIMER
in order to implement a very simple pre-amplifier.

- Connect an audio source to the audio in connector
  on the DVK, for instance an MP3 player.
- Connect an active loudspeaker with built-in amplifier
  to the audio out connector on the DVK.

WARNING:
--------
Do not attach or use headphones with this example. Use small
loadspeakers with built in amplification, ensuring volume is at an
acceptable level. Exposure to loud noises from any source for extended
periods of time may temporarily or permanently affect your hearing. The
louder the volume sound level, the less time is required before your
hearing could be affected. Hearing damage from loud noise is sometimes
undetectable at first and can have a cumulative effect.


The input signal is converted to digital and level
adjusted by the EFM32 core before being converted back
to analog.

The output volume level is adjusted with kit pushbuttons PB1 and PB2.
Output level is indicated by the 14 leftmost user LEDs.

The example will clip the output signal if exceeding predefined
level. This may occur due to too high input signal and/or to high
volume setting. Clipping is indicated by the rightmost user LED.

Push the AEM button on the kit until "EFM" shows in the upper right
corner of the TFT display to activate PB1 and PB2 pushbuttons.

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
