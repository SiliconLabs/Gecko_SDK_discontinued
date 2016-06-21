TouchGFX demo - smartphone look and feel on the EFM32GG.

To learn more about TouchGFX or download the full framework with EFM32GG-DK3750
Development Kit support, visit www.touchgfx.com.

This demo is supplied in binary form only by Draupner Graphics A/S.
Touchgfx.bin is the executable which must be flashed into MCU main flash memory,
touchgfx.ebin contains constant graphics data which must be flashed into the
norflash on the DK mainboard. Touchgfx_loader.bin is supplied by Silicon Labs
and will use LZ4 decompression to flash the norflash, then flash main flash and
finally reset the MCU to run the touchgfx demo. The DK leds will show a
progress bar while programming the norflash. When leds 15,14,1 and 0 start
flashing the demo will be started when the AEM button is pressed.

This demo demonstrates key features of the TouchGFX framework such as full
screen transitions, drag and drop, animations, and swipe gestures. The
MCU load is presented in the upper right corner throughout the demo.

The high performance and low MCU load are achieved by the advanced rendering
algorithms of TouchGFX and by utilizing advanced DMA features included in the
EFM32GG.

The button labeled PB1 can be used to toggle the hardware masking and
alpha-blending features that are unique to the EFM32 processor. Notice the
increase in MCU load especially during screen transitions with these features
disabled. TouchGFX is currently the only graphics framework on the market that
utilizes these features.

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
