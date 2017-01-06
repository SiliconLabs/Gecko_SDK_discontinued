Slideshow example using microSD.

This example uses the FatFS library for accssing a filesystem on a 
microSD card. It will loop through the files present on the card. 
BMP files with a resolution less than the resolution of the screen
(320x240) will be displayed on the TFT.

The demo has two modes of operation. If a file called "files.txt" is 
present on the root of the microSD card, this file will be read. The
BMP files will then be displayed in the order they are listed in the
file.

If this file is not present, all files on the root of tile SD card is 
looped through and displayed. If the BMP decoder cannot display a 
certain file, an error message is displayed.


WARNING:
SD2119 driver and GLIB graphics library are not intended for production 
purposes, and are included here to illustrate TFT display driving only.
This components are subject to changes in API/usage and there will be 
no effort to keep compatibility, or to support this software in any way.

NOTE:
This example is too large to be built with IDEs with 32KB size limits.

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
