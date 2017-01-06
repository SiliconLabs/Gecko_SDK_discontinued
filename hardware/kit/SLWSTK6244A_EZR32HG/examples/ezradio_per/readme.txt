EZRadio packet error rate (PER) example using the EZRadio device in the EZR32HG
 device.

This example project uses the EZR32HG CMSIS including emlib, emdrv and the
board support package support functions/drivers to demonstrate driving
the EZRadio or EZRadioPRO device in the EZR32 device.

It is advised to use two boards with same configuration and firmware, so
both packet transmission and reception functionality can be tested.

This example bases on the standard TRx with auto acknowledge example. It is 
advised to study the original application first.
All the original functionalities can be turned on, however, the main purpose
of this example is to show how to implement transmission packet error rate (PER) 
and RSSI measurements in an application. The user can send either one (PB0) 
or multiple packets (PB1) with the transmitter node and study the PER and RSSI
values on the receiver node.

The user can send specified or unlimited number of packets by pushing PB1.
The user can cease the transmission by pushing PB1 again.
The number of transmitted packets is configurable.

The application implements Packet Trace functionality, that means that the
user can observe transmitted and received packets with the Network Analyzer Tool
of Simplicity studio using the built-in default radio configuration header files.

The sample app is able to consume radio configuration header files generated
with either Simplicity Studio or Wireless Development Suite. The generated
configuration is consumed automatically in Simplicity Studio. In order to use
the generated header file instead of the default one with other toolchains enable
RADIO_USE_GENERATED_CONFIGURATION in ezradio_plugin_manager.h.

For more information please refer to the Quick Start Guide of the application.

Board:  Silicon Labs SLWSTK6244A_EZR32HG Development Kit
Device: EZR32HG320F64R63
