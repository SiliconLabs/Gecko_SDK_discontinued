================ EFM32 IEC60335 Class-B Library =========================

This directory, "iec60335_classb", contains the Silicon Labs IEC60335 Class-B
Support for the EFM32G series of microcontrollers.

The "iec60335_classb" SW is designed to support EFM32 rev B and later versions.
The no known caveats exists for the devices:

Some design guidelines for these utilities are

* Follow the guidelines established by ARM's and Silicon Labs's adaptation
  of the CMSIS (see below) standard

* Be usable as a starting point for developing richer, more target specific
  functionality (i.e. copy and modify further)

* Ability to be used as a standalone software component, used by other drivers
  that should cover "the most common cases"

* Readability of the code and usability preferred before optimization for speed
  and size or covering a particular "narrow" purpose

* As little "cross-dependency" between modules as possible, to enable users to
  pick and choose what they want

As a result of this, the library requires basic C99-support. You might have
to enable C99 support in your compiler. Comments are in doxygen compatible
format.

================ About CMSIS ================================================

These utilities are based on EFM32_CMSIS, the Cortex Microcontroller Software
Interface Standard support headers, as supplied by Silicon Labs.

The EFM32_CMSIS library contains all peripheral module registers and bit field
descriptors.

To download EFM32_CMSIS, go to
    http://www.energymicro.com/downloads

For more information about CMSIS see
    http://www.onarm.com
    http://www.arm.com/products/CPUs/CMSIS.html

The requirements for using CMSIS also apply to this package.

================ File structure ==============================================

inc/ - header files
src/ - source files

================ Licenses ====================================================

See the top of each file for SW license. Basically you are free to use the
Silicon Labs code for any project using EFM32 devices. Parts of the CMSIS
library is copyrighted by ARM Inc. See "License.doc" for ARM's CMSIS license.

================ Software updates ============================================

Silicon Labs continually works to provide updated and improved EFMLIB, example
code and other software of use for EFM32 customers. Please check the download
section of

        http://www.energymicro.com/downloads

for the latest releases, news and updates.

               Copyright 2015 Silicon Laboratories, Inc.

