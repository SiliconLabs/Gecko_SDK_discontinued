USB CDC virtual COM port example.

This example project use the EFM32 USB Device protocol stack
and implements an USB CDC based virtual COM port.

UART1 on the DK is used as the physical COM port.
Any data sent to the virtual CDC COM port is transmitted on UART1.
Any data received on UART1 is transmitted to the virtual port.

When connecting the mcu plugin boards USB port to a Windows host PC,
the new hardware "Wizard" may or may not prompt you to provide a driver
installation file (.inf file) depending on your Windows version.

Manually direct Windows to look for drivers in the directory where you have
unpacked your copy of the Silabs-CDC_Install.zip driver file.
This can be done with the new device "Wizard" which might pop up after device
insertion, or you can open "Device Manager", left click on the new device and
select "Update Driver Software...".

Some versions of Windows wont allow you to install unsigned drivers. If you
suspect this, reboot the PC and repeatedly push F8 during boot until the boot
menu appears. Select the "Disable Driver Signature Enforcement" option.
You should now be able to install an unsigned driver.

When a driver is succesfully installed, the device will be listed as a "Ports"
device in Device Manager, double-click it, select the "Port Settings" tab and
maybe the "Advanced..." button to set serial port properties.

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
