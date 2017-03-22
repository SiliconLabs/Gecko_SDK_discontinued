Audio FFT example.

This example project uses the EFM32 CMSIS and ARM's DSP lib to demonstrate
floating point FFT of audio data.

Connect an audio source to AUDIO IN on the DK and set the AEM switch to the EFM
position.
A FFT plot of the audio input will be shown on the screen together with a count
of the cpu-cycles used to calculate the FFT.
The audio bandwidth is 4 kHz.

Experiment using hard and soft floating point and various compiler optimization
flags to view the effect on the cpu cycle count.

Board: Silicon Labs EFM32WG_DK3850 Development Kit
Device: EFM32WG990F256
