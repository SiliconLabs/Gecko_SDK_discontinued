// Copyright 2014 Silicon Laboratories, Inc.
//
// Implements an InvenSense INMP421 PDM digital microphone to EM34xx Interface
//
// Microphone is connected to an SCx port operated in SPI master RX driven DMA
// mode with MOSI driven via GPIO as microphone on/off power supply (0.65mA
// max)
//
// INMP421 datasheet recommend 2.4MHz CLK, using 1.2MHz CLK for MIPS
//
// PDM data is collected via SPI/DMA
//
// 1.2MSps/1-bit PDM data is passed through a CIC filter with:
//  => Two stages (N=2)
//  => Decimate by 25 (R=25) => reduces data rate to 48kSps
//  => Delay of 2 (M=2)
//  => CIC with these parameters increases bit width to 12.3-bits
//
// CIC filter's 48kSps/12.3-bit PCM data is then:
//  => Logical left shifted by 4 (adjust bit width to ~16-bits)
//  => Passed through an IIR LPF (2nd order) with 7.2kHz BW (0.15fc)
//  => Decimated by 3 => reduces data rate to 16kSps
//
// Four Customer Programmable Series Biquads - Equalizer Options
//  => Processed at 16kSps
//  => Coefficients are in S16.14 format (sign extended to int32_t)
//
// LPF's 16kSps/16-bit PCM is then:
//  => Passed through an IIR HPF (1st order) with f-3dB=160Hz (0.01fc)
//  => Gained by 14dB (mult by 5) to scale -26dBFS at 94dBSPL to -12dBFS
//  => Saturated to 16-bit signed
//
// Two words of 16kSps/16-bit PCM data are compressed to a single byte for
// inclusion in audio buffer
//  => 4:1 compression is achieved via low MIPS IMA ADCPM compression
//  => IMA ADCPCM compressor state variables use 3-bytes (4-bytes unpack)
//  => IMA ADCPCM compressor state at start of each buffer fill is copied to
//     beginning of buffer
//
// Audio buffer size is set to 104-bytes (103-bytes after SEQ header)
//  => After IMA ADCPCM compressor state, 100 bytes are used for compressed data
//  => 100 compression bytes/audio xfer => 200 PCM samples/audio xfer
//  => At 16kSps, a audio transfer is required every 12.50 ms
//
// Due to mathematics of 100 compressed bytes per buffer and 1.2MSps microphone:
//  1. SPI/DMA buffers (default) is 75 bytes every 500us
//  2. 75 bytes => 600 PDM bits at 1.2MSps
//  3. 600 PDM bits => CIC outputs 24 12.3-bit/48kSps PCM words
//  4. LSL4, 2nd Order LPF, and decimate by 3 => 8 16-bit signed PCM at 16kSps
//  5. Four customer programmable biquads at 16kSps
//  5. HPF, Gain, and Saturate => 8 16-bit signed PCM at 16kSps (scaled, no DC)
//  6. 8 16-bit signed PCM at 16kSps => 4 compressed IMA ADPCM bytes
//  7. 4 compressed IMA ADPCM bytes => 25 DMA cycles to fill 100 byte audio
//  8. 25 cycles of 500us each => 12.5 ms between audio xfers
//
// This code uses an ::EmberEventControl called
// ::halMicrophoneImaadpcmDataReadyEventControl to perform periodic tasks in
// the handler ::halMicrophoneImaadpcmDataReadyEventHandler. The application
// framework will generally manage the event automatically. Customers who do not
// use the framework must ensure the event is run, by calling either
// ::emberRunEvents or ::emberRunTask.
#ifndef __MICROPHONE_IMAADPCM_H__
#define __MICROPHONE_IMAADPCM_H__

typedef enum
{
  BIQUADS_NONE_ENABLED = 0x0000,
  BIQUADS_A_ENABLED    = 0x0001,
  BIQUADS_AB_ENABLED   = 0x0002,
  BIQUADS_ABC_ENABLED  = 0x0003,
  BIQUADS_ABCD_ENABLED = 0x0004
} HalMicrophoneBiquadEnable_t;

/** @brief Start microphone
 *
 * This function starts sampling the microphone output.
 *
 * Note: halMicroponeImaadpcmDataReadyCallback will be called with a pointer
 * to the microphone data once it is available.
 *
 * @param none
 */
void halMicrophoneImaadpcmStart(void);

/** @brief Stop microphone
 *
 * This function stops sampling of the microphone output.
 *
 * @param none
 */
void halMicrophoneImaadpcmStop(void);

/** @brief A callback called when new microphone data is ready
 *
 * @appusage This function is called by the plugin when new data has been
 * processed and is ready to be processed by other parts of the system.
 * The callback should be implemented by the application.
 *
 * @param data     Pointer to the data that is ready
 *
 * @param length   Length of the data
 */
void halMicrophoneImaadpcmDataReadyCallback(uint8_t * data, uint8_t length);

#endif // __MICROPHONE_IMAADPCM_H__
