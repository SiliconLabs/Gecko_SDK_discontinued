USB CDC virtual COM port example.

This example project use the EFM32 USB Device protocol stack
and implements an USB CDC based virtual COM port.

USART1 on the STK is used as the physical COM port.
Any data sent to the virtual CDC COM port is transmitted on USART1.
Any data received on USART1 is transmitted to the virtial port.
Connect a terminal to the TTL-level serialport (115200-N-8-1) on EXP port pins
3 (Tx)  and 5 (Rx).

When connecting the mcu plugin boards USB port to a Windows host PC,
the new hardware "Wizard" may or may not prompt you to provide a driver
installation file (.inf file) depending on your Windows version.

Manually direct Windows to look for drivers in the directory where you have
unpacked your copy of the Silabs-CDC_Install.zip driver file.
This can be done with the new device "Wizard" which might pop up after device
insertion, or you can open "Device Manager", left click on the new device and
select "Update Driver Software...".

Some versions of Windows wont allow you to install unsigned drivers. If you
suspect this, reboot the PC into "Disable Driver Signature Enforcement" mode.
Win7:
Repeatedly push F8 during boot until the boot menu appears.
Select the "Disable Driver Signature Enforcement" option.
Win10:
Start Menu->Power button->Hold Shift key down and select Restart.
When computer has restarted into the options menu, select:
Troubleshoot->Advanced Option->Startup Settings->Restart->Disable driver signature enforcement
You should now be able to install an unsigned driver.

When a driver is succesfully installed, the device will be listed as a "Ports"
device in Device Manager, double-click it, select the "Port Settings" tab and
maybe the "Advanced..." button to set serial port properties.

Board:  Silicon Labs SLSTK3400A Starter Kit
Device: EFM32HG322F64
