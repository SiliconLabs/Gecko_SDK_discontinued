// Copyright 2014 Silicon Laboratories, Inc.
//-----------------------------------------------------------------------------
#ifndef __MSADPCM_H__
#define __MSADPCM_H__

//-----------------------------------------------------------------------------
// #defines
//-----------------------------------------------------------------------------
typedef struct HalMsadpcmState {
  uint8_t predictor;            // Index into msadpcmAdaptCoeffx tables
  uint16_t delta;               // Quantization scale
  int16_t sample1;             // Second sample of block (full PCM format)
  int16_t sample2;             // First sample of block (full PCM format)

                              // Temp variables
  int32_t coeff1;              // Promoted msadpcmAdaptCoeff1[State->predictor]
  int32_t coeff2;              // Promoted msadpcmAdaptCoeff2[State->predictor]
} HalMsadpcmState;

// PACKED SIZE
#define MSADCPM_STATE_SIZE (sizeof(uint8_t)+sizeof(uint16_t)+2*sizeof(int16_t))
#define MSADCPM_SAMPLES_IN_HEADER 2

//-----------------------------------------------------------------------------
// Global API Function Prototypes
//-----------------------------------------------------------------------------
// Decode/Encode a single sample and update state
int16_t halInternalMsadpcmDecode(uint8_t compressedCode,
                                HalMsadpcmState *state);
uint8_t halInternalMsadpcmEncode(int16_t realSample,
                               HalMsadpcmState *state);
void halInternalMsadpcmResetState(HalMsadpcmState *state,
                                  uint8_t predictor,
                                  uint8_t resetDelta,
                                  const int16_t *samples,
                                  uint8_t numsamples);
void halInternalMsadpcmSaveState(uint8_t *stateDst,
                                 const HalMsadpcmState *stateSrc);
void halInternalMsadpcmRestoreState(HalMsadpcmState *stateDst,
                                    const uint8_t *stateSrc);
#endif // __MSADPCM_H__
