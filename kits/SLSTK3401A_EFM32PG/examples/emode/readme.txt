Select a single energy mode, and stay there.

This example project uses the EFM32 CMSIS and demonstrates the use of
the LCD display, RTCC, GPIO and various Energy Modes (EM).

Use PB1 to cycle through the energy mode tests available.
Press PB0 to start selected test.

Note: The EMU power configuration register can only be written once after a
      power-on reset and may be locked to a different configuration. For the
      emode demo to function properly, a power-on reset may be required to
      configure the DCDC properly.

This demo application has been made to give a quick demo of the
energyAware Profiler from Silicon Labs, including EFM32 energy modes.

Board:  Silicon Labs SLSTK3401A Starter Kit
Device: EFM32PG1B200F256
