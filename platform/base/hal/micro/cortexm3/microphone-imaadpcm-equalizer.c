// Copyright 2015 Silicon Laboratories, Inc.
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "hal/micro/microphone-imaadpcm.h"

//-----------------------------------------------------------------------------
// Customer Programmable Biquads Enable
// Note: halMicrophonebBiquadEnable allows enabling the following biquad
// combinations:
//  - 0x0004 - D, C, B, and A
//  - 0x0003 - C, B, and A only (bypass D)
//  - 0x0002 - B and A only (bypass D and C)
//  - 0x0001 - A only (bypass D, C, and B)
//  - 0x0000 (or any other value) - None (bypass all)
// Note: If a biquad is not needed for equalizer, it is recommended to bypass
#define MICROPHONE_IMAADPCM_EQUALIZER_BIQUAD_ENABLE BIQUADS_ABCD_ENABLED

const HalMicrophoneBiquadEnable_t halMicrophoneBiquadEnable =
  MICROPHONE_IMAADPCM_EQUALIZER_BIQUAD_ENABLE;

//-----------------------------------------------------------------------------
// Customer Programmable Biquads Coefs
// Four 16kSps Audio Codec Biquads for Custom Equalizer per Remote Enclosure
// Coefs need to be S16.14 format, sign extended to int32_t

// The biquad difference equation is:
// Yn = A0 * Xn + A1 * Xnm1 + A2 * Xnm2 + B1 * Ynm1 + B2 *Ymn2

// In the arrays below, the biquad coefs mappings are:
//    biquadX16kSpsCoefs[0] = A0 for biquad X
//    biquadX16kSpsCoefs[1] = A1 for biquad X
//    biquadX16kSpsCoefs[2] = A2 for biquad X
//    biquadX16kSpsCoefs[3] = B1 for biquad X
//    biquadX16kSpsCoefs[4] = B2 for biquad X

const int32_t halMicrophonebBiquadA16kSpsCoefs[5] = {
#if (MICROPHONE_IMAADPCM_EQUALIZER_BIQUAD_ENABLE >= BIQUADS_A_ENABLED)
  19675, // low shelving filter with Gain=6dB, s=1, and critical freq=2kHz
  -15379,
  5543,
  17833,
  -6380
#else
  16384,  // A0 - bypass
  0,      // A1
  0,      // A2
  0,      // B1
  0       // B2
#endif
};

const int32_t halMicrophonebBiquadB16kSpsCoefs[5] = {
#if (MICROPHONE_IMAADPCM_EQUALIZER_BIQUAD_ENABLE >= BIQUADS_AB_ENABLED)
  19675, // high shelving filter with Gain=6dB, s=1, and critical freq=6kHz
  15379,
  5543,
  -17833,
  -6380
#else
  16384,  // A0 - bypass
  0,      // A1
  0,      // A2
  0,      // B1
  0       // B2
#endif
};

const int32_t halMicrophonebBiquadC16kSpsCoefs[5] = {
#if (MICROPHONE_IMAADPCM_EQUALIZER_BIQUAD_ENABLE >= BIQUADS_ABC_ENABLED)
  21824, // peaking shelving filter with Gain=6dB, s=1, and critical freq=4kHz
  0,
  13,
  0,
  -5453
#else
  16384,  // A0 - bypass
  0,      // A1
  0,      // A2
  0,      // B1
  0       // B2
#endif
};

const int32_t halMicrophonebBiquadD16kSpsCoefs[5] = {
#if (MICROPHONE_IMAADPCM_EQUALIZER_BIQUAD_ENABLE == BIQUADS_ABCD_ENABLED)
  4280, // BPF filter with fcenter=2kHz and BW=4kHz
  0,
  -4280,
  17118,
  -7825
#else
  16384,  // A0 - bypass
  0,      // A1
  0,      // A2
  0,      // B1
  0       // B2
#endif
};
