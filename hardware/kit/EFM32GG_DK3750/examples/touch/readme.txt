Touchscreen example.

This example demonstrate use of the resistive touchscreen on the
EFM32GG_DK3750 kit's TFT-display.

Touchscreen X and Y positions are obtained using the ADC. Two ADC readouts
are performed, one for X position and one for Y position.

The example allows freehand drawing and calibration. After starting the example
make sure that the EFM32 control the display (use AEM button to toggle).

The calibrations algorithm used is from an article in EETimes 5/31/2002 by
Carlos E. Vidales.

Initial touchscreen calibration is "off" on purpose to demonstrate that
calibration takes effect.

Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024
