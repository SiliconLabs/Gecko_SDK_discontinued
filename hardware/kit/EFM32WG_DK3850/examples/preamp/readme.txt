Audio preamplifier with tone controls example.

This example project uses the EFM32 CMSIS including the BSP (board
support package) CMSIS-DSP to demonstrate the signal processing capabilities
of the Wonder Gecko MCU.

- Connect an audio source to the audio in connector
  on the DK, for instance a mobile phone or an MP3 player.
- Connect an active loudspeaker with built-in amplifier,
  to the audio out connector of the DK.

WARNING:
--------
Do not attach or use headphones with this example. Use small
loadspeakers with built in amplification, ensuring volume is at an
acceptable level. Exposure to loud noises from any source for extended
periods of time may temporarily or permanently affect your hearing. The
louder the volume sound level, the less time is required before your
hearing could be affected. Hearing damage from loud noise is sometimes
undetectable at first and can have a cumulative effect.

The input signal is converted to digital and processed by the EFM32 core
before being converted back to analog.

Make sure to set kit display mode to "EFM" (signalled by flashing LED's),
by pushing kit "AEM" button.

The output volume level is adjusted with joystick up/down.
The output balance is adjusted with joystick left/right.
Bass tone control is adjusted with pushbuttons PB1 and PB2.
Treble tone control is adjusted with pushbuttons PB3 and PB4.
A 390 Hz test tone is emitted when pushing joystick center.

Board:  Silicon Labs EFM32WG-DK3850 Development Kit
Device: EFM32WG990F256
