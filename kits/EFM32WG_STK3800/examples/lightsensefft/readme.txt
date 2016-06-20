Lightsensor example with FFT transform.

This example project uses the EFM32 CMSIS and ARM's DSP lib to demonstrate
floating point FFT and sinc interpolation.

Connect the light sensor output to the ADC input by shorting pins
15 and 14 on the EXP_HEADER of the STK.
Direct various light sources to the light sensor. Expect no specific
frequency from daylight or from a flashlight. Mains powered incandescent bulbs
should give twice the mains frequency. Using another STK running the
"blink" example modified to various blink rates is an excellent signal source.
The frequency bandwidth is approximately 10-500 Hz.
The frequency shows in the 4 digit numerical display upper right on
the LCD. The LCD also displays the number of cpu cycles used to do
the FFT transform.

You can of course run this example without connecting the light sensor to
the ADC, you will still be able to measure the cpu cycle count used to do
the FFT transform.

Experiment using hard and soft floating point math and see the difference
in cpu cycle count.

Board: Silicon Labs EFM32WG_STK3800 Development Kit
Device: EFM32WG990F256
