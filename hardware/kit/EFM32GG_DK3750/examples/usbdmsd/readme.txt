USB Mass Storage Device example.

This example project use the EFM32 USB Device protocol stack
to implement a device Mass Storage Class device (MSD).

The example provides five different MSD types.

 - A 96KByte "disk" in internal SRAM
 - A 4MByte "disk" in external PSRAM
 - A disk implemented with a micro SD-Card
 - An internal FLASH "disk" (disksize is internal FLASH size minus 64K)
 - A 16MB external NORFLASH "disk"

Select mediatype in msddmedia.h (#define MSD_MEDIA)

It is also possible to make a buspowered device:
 - Enable #define BUSPOWERED in usbconfig.h (only valid for SRAM and FLASH media)
 - Program the MCU plugin module, remove it from the DK.
 - Connect "USB VREGO" and "VMCU" together.
 - Optionally connect "USB VREGO" and "3.3V" together to enable an activity LED
   (the blue LED labeled "STATUS").
   Note: This will cause the device suspend current to exceed 2.5 mA.

 - NOTE !!! Do NOT insert the plugin module back on the DK with "USB VREGO",
   "3.3V" and "VMCU" connected together !!!.

The example will output some info on the DK serial port (115.200 8N1).

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
