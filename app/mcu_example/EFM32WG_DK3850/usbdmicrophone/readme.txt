USB audio microphone device example.

This example implements a USB connected microphone device. The device enumerates
as a device supporting stereo 16bit PCM encoded audio at a samplerate of
44.1 kHz (the standard audio CD rate). The samples are aquired using the ADC
which is routed to the "AUDIO IN" jack on the mainboard. The example can be
compiled in "tone generator" mode, where the device produce a 1kHz sine.

The USB device is modelled as the MT (Microphone Topology) described in
USB-IF document "Universal Serial Bus Audio Device Class Specification for
Basic Audio Devices", Release 1.0, November 24th, 2009.

The ADC is sampling at a rate of 44.118 kHz, so additional audio samples
are occasionally inserted or removed from the data sent to host to ensure that
the data streams (USB and ADC) stay synchronized. This gives reasonable audio
quality when listening to music, but produce noticeable artifacts when
listening to a pure sinewave or when the example is compiled in "tone generator"
mode.

Board:  Silicon Labs EFM32WG-DK3850 Development Kit
Device: EFM32WG990F256
