Gecko SDK
=========

The Gecko SDK is software development kit developed by Silicon Labs for the [EFM32](http://www.silabs.com/products/mcu/32-bit/Pages/32-bit-microcontrollers.aspx) and [EZR32](http://www.silabs.com/products/wireless/proprietary/Pages/proprietary-wireless-protocols.aspx). It contains the basic software needed for development, covering everything from the low-level hardware abstraction layer (HAL) and peripheral drivers to communication stacks and example code.
The Gecko SDK is also part of the [Simplicity Studio](http://www.silabs.com/simplicity) toolsuite from Silicon Labs. Downloading Simplicity Studio also gives you access to a large range of tools, like Configurator and Energy Profiler together with access to all software and docuementation 

Documentation
-------------

Full API documentation is available hosted on [GitHub pages](http://siliconlabs.github.io/Gecko_SDK/EM_CMSIS_DOC_4.1.0/), in the [EM_CMSIS_DOC_4.1.0](EM_CMSIS_DOC_4.1.0) folder and within Simplicity Studio by clicking the *Software Documentation* tile.
For datasheets, reference manuals and other documentation items, download [Simplicity Studio](http://www.silabs.com/simplicity) or head over to the [document library](http://www.silabs.com/support/pages/document-library.aspx?p=MCUs--32-bit).

About the folders
-----------------

The Gecko MCU SDK is a comprehensive suite of software, with everything needed for software development on a Gecko MCU (EFM32) or Wireless Gecko (EZR32), organized in an easy way to help with application development. It contains full software support, from register definitions to full-fledged examples.
The Gecko MCU SDK is organized into the following folders:
* [CMSIS](CMSIS) — This folder contains the basic core definitions definitions and libraries needed to work with the Cortex-M CPUs
* [Device](Device) — This folder contains the device specific register defnitions for our MCUs. It is written according to ARMs Cortex Microcontroller Software Interface Standard (CMSIS), and you have full control over every bit of the hardware using these defines.
* [emlib](emlib) — This layer is the hardware abstraction layer (HAL), which gives control over most of the hardware, but a limited degree of abstraction is provided to reduce complexity and ease implementation. At this level, most of the functions are readily portable across the full range of Gecko devices, from Gecko MCUs (EFM32) to Wireless Geckos (EZR32).
* [emdrv](emdrv) — These are full drivers for different peripherals, such as a SPI driver or a flash driver. They are easy to set up, give immediate access to the given functionality, and are fully portable across all Gecko devices.
* [reptile](reptile) — Middleware components developed by Silicon Labs and ports of popular third-party middleware. These are full libraries available to use in an application, like graphics, file systems, crypto libraries, real-time operating systems (RTOS), andTCP/IP stacks. These can either be developed by third-party companies like Segger or Micrium or be based on open source code like lwip and FatFS.
* [usb](usb) - The USB library for the devices. Contains both a host and device stack and is very well integrated with the MCUs energy modes to allow low-power operation. Examples are included for Mass Storage Device (MSD) class, Communication Device Class (CDC), Human Interface Device (HID) class, audio class and Vendor Unique Device (VUD) class.
* [kits](kits) — The Board Support Package (BSP) contains everything you need to use the peripherals available on the Gecko Starter Kits (STK). Drivers are provided for everything from touch buttons and accelerometers to LCD and TFT displays.  
This folder also contain **examples** that run on top of all this software to demonstrate the unique capabilities of the Gecko microcontrollers. They are usually full examples that demonstrate the MCU in a given application and give you a starting point for building your own code. For smaller examples to guide you in setting up peripherals, we also have application notes (AN) that are distributed separately. The examples run on top of all this software to demonstrate the unique capabilities of the Gecko microcontrollers. They are usually full examples that demonstrate the MCU in a given application and give a starting point for building your own code. Application Notes (AN) are generally smaller examples to guide peripheral set-up and are distributed separately on the [website](www.silabs.com/32bit-appnotes) or through Simplicity Studio using the [Application Notes] tile.

Changelog
---------
See the changlogs for the individual modules:
* [Device Changelog](Device/Changes-CMSIS.txt)
* [emlib Changelog](emlib/Changes_emlib.txt)
* [emdrv Changelog](emdrv/Changes-emdrv.txt)
* [usb Changelog](usb/Changes_usb.txt)
* **kits** - Individual changelogs for each kit in this [folder](kits)