USB audio headphone device example.

This example implements a USB connected headphone device. The device enumerates
as a device supporting stereo 16bit PCM encoded audio at a samplerate of
44.1 kHz (the standard audio CD rate). The samples are sent directly to the I2S
DAC on the mainboard and audio is available on the "AUDIO OUT" jack.

The USB device is modelled as the HT1 (Headphone Topology 1) described in
USB-IF document "Universal Serial Bus Audio Device Class Specification for
Basic Audio Devices", Release 1.0, November 24th, 2009.

The I2S DAC is fed at a samplerate of 44.118 kHz, so additional audio samples
are occasionally inserted to ensure that the data streams (USB and DAC) stay
synchronized.

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
