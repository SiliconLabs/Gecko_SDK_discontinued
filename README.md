Gecko SDK
=========

The Gecko SDK is software development kit developed by Silicon Labs for the [EFM32](http://www.silabs.com/products/mcu/32-bit/Pages/32-bit-microcontrollers.aspx), [EFR32](https://www.silabs.com/products/wireless/Pages/wireless-gecko-iot-connectivity-portfolio.aspx) and [EZR32](http://www.silabs.com/products/wireless/proprietary/Pages/proprietary-wireless-protocols.aspx). It contains the basic software needed for development, covering everything from the low-level hardware abstraction layer (HAL) and peripheral drivers to communication stacks and example code.
The Gecko SDK is also part of the [Simplicity Studio](http://www.silabs.com/simplicity) tool suite from Silicon Labs. Downloading Simplicity Studio also gives you access to a large range of tools, like Configurator and Energy Profiler together with access to all software and documentation.

Documentation
-------------

Full API documentation is available in the [Gecko_SDK_Doc](https://github.com/SiliconLabs/Gecko_SDK_Doc) repository, hosted on [GitHub pages](http://siliconlabs.github.io/Gecko_SDK_Doc/), and within Simplicity Studio by clicking the *Software Documentation* tile.  
For datasheets, reference manuals and other documentation items, download [Simplicity Studio](http://www.silabs.com/simplicity) or head over to the [document library](http://www.silabs.com/support/pages/document-library.aspx?p=MCUs--32-bit).

Download/Versions
-----------------

To download the complete Gecko SDK as one packed zip-file, head over to the [releases](https://github.com/SiliconLabs/Gecko_SDK/releases) section. Here you will find all the different versions downloadable as a zip-file or compressed tarball. Just scroll down to the version/release you want and find the *Source code* link under the *Downloads* section.

Importing into Simplicity Studio
--------------------------------

To import the downloaded version of the Gecko SDK into Simplicity Studio, follow the steps below:

1. Extract the zip-file
2. Open Simplicity Studio
3. Click the settings button
4. Browse to Simplicity Studio -> SDKs
5. Click *Add...*
6. Browse to the location of the extracted SDK
7. Make sure the EFM32 SDK is selected and press *OK*
8. You can now create *Software Examples* from this SDK version

![Visual guide on how to import SDK into Simplicity Studio](https://github.com/SiliconLabs/Gecko_SDK/raw/master/Gecko_SDK_SS_import.png)


About the folders
-----------------

The Gecko MCU SDK is a comprehensive suite of software, with everything needed for software development on a Gecko MCU (EFM32) or Wireless Gecko (EFR32 and EZR32), organized in an easy way to help with application development. It contains full software support, from register definitions to full-fledged examples.
The Gecko MCU SDK is organized into the following folders:
* [.studio] (.studio) — This folder contains metadata information for Simplicity Studio.
* [platform/CMSIS](platform/CMSIS) — This folder contains the basic core definitions definitions and libraries needed to work with the Cortex-M CPUs
* [platform/Device](platform/Device) — This folder contains the device specific register defnitions for our MCUs. It is written according to ARMs Cortex Microcontroller Software Interface Standard (CMSIS), and you have full control over every bit of the hardware using these defines.
* [platform/emlib](platform/emlib) — This layer is the hardware abstraction layer (HAL), which gives control over most of the hardware, but a limited degree of abstraction is provided to reduce complexity and ease implementation. At this level, most of the functions are readily portable across the full range of Gecko devices, from Gecko MCUs (EFM32) to Wireless Geckos (EFR32 and EZR32).
* [platform/emdrv](platform/emdrv) — These are full drivers for different peripherals, such as a SPI driver or a flash driver. They are easy to set up, give immediate access to the given functionality, and are fully portable across all Gecko devices.
* [platform/middleware] (platform/middleware) — These are higher-level libraries that sit on top of the emlib or emdrv layers. Examples are available to demonstrate the use of these libraries.
* [util](util) — Middleware components developed by Silicon Labs and ports of popular third-party middleware. These are full libraries available to use in an application, like graphics, file systems, real-time operating systems (RTOS), certification libraries (IEC-60335 Class B), TCP/IP stacks, and cryptographic libraries (mbedTLS). The third-party software can either be developed by companies like Segger or Micrium or be based on open source code like lwip and FatFS.
* [hardware/kit](hardware/kit) — The Board Support Package (BSP) contains everything you need to use the peripherals available on the Gecko Starter Kits (STK). Drivers are provided for everything from touch buttons and accelerometers to LCD and TFT displays.  
* [app/mcu_example](app/mcu_example) — This folder contains **examples** that run on top of all this software to demonstrate the unique capabilities of the Gecko microcontrollers. They are usually full examples that demonstrate the MCU in a given application and give you a starting point for building your own code. For smaller examples to guide you in setting up peripherals, we also have application notes (AN) that are distributed separately. The examples run on top of all this software to demonstrate the unique capabilities of the Gecko microcontrollers. They are usually full examples that demonstrate the MCU in a given application and give a starting point for building your own code. Application Notes (AN) are generally smaller examples to guide peripheral set-up and are distributed separately on the [website](www.silabs.com/32bit-appnotes) or through Simplicity Studio using the Application Notes area under Documentation.

Changelog
---------
See the changlogs for the individual modules:
* [Device Changelog](platform/Device/Changes-Device.txt)
* [emlib Changelog](platform/emlib/Changes_emlib.txt)
* [emdrv Changelog](platform/emdrv/Changes-emdrv.txt)
* [usb Changelog](platform/middleware/usb_gecko/Changes_usb.txt)
* **kits** - Individual changelogs for each kit in this [folder](hardware/kit)
