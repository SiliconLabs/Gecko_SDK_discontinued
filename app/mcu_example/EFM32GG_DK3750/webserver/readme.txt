Webserver example
==================

This example shows how to use the EFM32GG-DK3750 to run a http server with a
static web page. This example is using the lwip network stack to provide the
necessary protocol implementations and it's using the ksz8851snl ethernet
controller on the development kit to send and receive packets.

Web page
=========

To view the static web page open your browser to http://dk-ip/ where the 
dk-ip is the ip address assigned to your development kit.

You can find the source code for the static web page under the html directory.
These source files are converted into the fsdata_custom.c file that the lwip
http module knows how to read. This conversion is done by the makefsdata tool.

To customize your own web pages, you can use the makefsdata tool to generate
your own fsdata_custom.c file. The makefsdata tool is provided as part of lwip,
and you can find the C source file for it in the lwip-
contrib/apps/httpserver_raw/makefsdata folder (Don't use the makefsdata perl
script, it is outdated). This is an example of how you can generate the
fsdata_custom.c file using makefsdata.exe.

makefsdata.exe html -l1 -f:fsdata_custom.c

Output
=======

When a network interface is up and running the IP address of the server is
displayed on the LCD display together with rx/tx packet statistics. If you want
to view the lwip trace output you can open a serial connection to the
development kit using baud rate 115200.

IP Configuration
=================

By default this example is configured to use DHCP to fetch an ip address. If you
want to use static ip address instead then follow these steps.

1. open lwipopts.h
2. edit the LWIP_DHCP option to be

#define LWIP_DHCP                       0

3. edit the STATIC_* options to match your desired ip address, subnet mask,
default gateway. This is an example of how to configure these static ip
addresses.

ip: 192.168.0.12
sn: 255.255.255.0
gw: 192.168.0.1

#define STATIC_IPADDR0                  192
#define STATIC_IPADDR1                  168
#define STATIC_IPADDR2                  0
#define STATIC_IPADDR3                  12

#define STATIC_NET_MASK0                255
#define STATIC_NET_MASK1                255
#define STATIC_NET_MASK2                255
#define STATIC_NET_MASK3                0

#define STATIC_GW_IPADDR0               192
#define STATIC_GW_IPADDR1               168
#define STATIC_GW_IPADDR2               0
#define STATIC_GW_IPADDR3               1


ksz8851snl interface
=====================

The ksz8851snl ethernet controller is using an spi interface, and this is
connected to one of the EFM32 USARTs. See the kit user-manual for the actual
pins. This example also shows how to use the provided ksz8851snl drivers to
operate the ethernet controller. These are the main files that are used when
interfacing with the ksz8851snl.

drivers/ksz8851snl.c

This is the hardware abstraction layer for the ksz8851snl. This file contains
functionality like enable and initialize the chip.

drivers/ksz8851snl_spi.c

This contains the spi communication functions for the ksz8851snl. We provide the
low level operations like read/write register as well as read/write fifo in this
file.

lwip-contrib/ports/efm32/ksz8851snl_driver.c

This is the lwip driver that interfaces between the lwip stack and our
ksz8851snl.c hardware abstraction layer.

Interrupt Handling
===================

The ksz8851snl has an interrupt pin that is routed through the board controller
and into a GPIO pin on the EFM32. When the ethernet interrupt is enabled. This
interrupt pin is active low. The ksz8851snl will trigger an interrupt if the
link status has changed or if an ethernet frame has been received or
transmitted. In this example the GPIO is configured to trigger an interrupt on a
falling edge of the ethernet interrupt pin.

The interrupts of the ksz8851snl are not serviced directly from the GPIO
interrupt handler. What we do instead is to signal to the application that an
interrupt is pending and let the main application eventually call the interrupt
service routine within the ksz8851snl driver. The reason for doing this is
because we want to minimize the amount of time inside the interrupt handler.
When a received frame interrupt is triggered the ksz8851snl driver will have to
allocate buffer and transfer a large amount of data from the controller to ram,
and then pass this buffer into the lwip stack.


Board:  Silicon Labs EFM32GG-DK3750 Development Kit
Device: EFM32GG990F1024

