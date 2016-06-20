Core clock example.

This example project uses the EFM32 CMSIS and emlib including DVK BSP
(board support package) and demonstrates the use of various clock
sources for driving the CORE. It also includes configuration using EM0-2
modes when in idle.

The following configuration through use of joystick is possible:

- Joystick up and down, toggles core clock source.
- Joystick left and right, changes core clock divison factor.
  (Divisor shown with ring.)
- Joystick push toggles energy mode.
  (Energy mode shown with EM symbol.)

Notice that the application does not allow core clock frequency below
appr 8kHz, in order to not become too unresponsive. Thus, changing from a
higher frequency to a lower frequency oscillator/band, may cause divisor
to auto-adjust.

If the Gecko symbol is lit, it indicates that debug mode is active, and
that energy modes will not behave as normal. Reset and run without the
debugger when trying different energy modes.

Board:  Silicon Labs EFM32-G8xx-DK Development Kit
Device: EFM32G890F128 w/LCD only
