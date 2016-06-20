USB LEUSB HID keyboard example. (Extends original HID keyboard example)

This example project uses the EFM32 USB Device protocol stack
and implements an USB HID class keyboard device (one button !).

The HID device enumerates and immediately disables the Low Energy USB features
(which are enabled by default in the USB protocol stack), allowing observation
of "normal" power consumption on VBUS.

PB0 is the keyboard button. Pressing this button will succesively write
"Silicon Labs / ".

PB1 is the toggle button for Low Energy Mode (LEM).  Pressing this button will
alternate between LEM enabled/disabled.  The graphic displayed on the STK LCD
will update to reflect this state (reads "HID keyboard Device" in normal mode,
or "Low-Energy USB" when LEM is enabled).

The example is power optimized when LEM is enabled.  With the STK power switch in
"USB" position (board power is drawn from USB host via VBUS), when USB cable is plugged
in and USB is active the current drawn from VBUS is ~8.3 mA in normal mode, or ~5.8 mA
when LEM is enabled, of which only ~460 uA is consumed by the USB PHY.  When the cable
is disconnected or the device is suspended, the current is less than 5 uA.


Board:  Silicon Labs SLSTK3400A Starter Kit
Device: EFM32HG322F64
