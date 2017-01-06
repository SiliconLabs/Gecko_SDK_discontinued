USB HID keyboard example.

This example project use the EFM32 USB Device protocol stack
and implements an USB HID class keyboard device (one button !).

SW1 is the keyboard button. Pressing this button will succesively write
"Silicon Labs - ".

User LED's 0-3 is used as a "heart-beat" counting pattern. The count rate
reflects the scan rate of the onebutton keypad.

User LED 8 reflects hosts NumLock status.
User LED 9 reflects hosts CapsLock status.
User LED 10 reflects hosts ScrollLock status.
User LED 15 reflects the status of the keyboard pushbutton.

It is also possible to make a buspowered device:
 - Enable #define BUSPOWERED in usbconfig.h
 - Program the MCU plugin module, remove it fron the DK.
 - Connect a pushbutton from PORTE bit 0 to GND (optional keyboard button).
 - Connect "USB VREGO" and "VMCU" together.
 - Optionally connect "USB VREGO" and "3.3V" together to enable an activity LED
   (the blue LED labeled "STATUS).
   Note: This will cause the device suspend current to exceed 2.5 mA.

 - NOTE !!! Do NOT insert the plugin module back on the DK with "USB VREGO",
   "3.3V" and "VMCU" connected together !!!.

This example will output useful info on the DK serial port (115.200 8N1).

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
