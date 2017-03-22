// Copyright 2014 Silicon Laboratories, Inc.
//-----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "hal/micro/msadpcm.h"

//-----------------------------------------------------------------------------
// Local Constants
//-----------------------------------------------------------------------------
static const int16_t msadpcmAdaptTable[] = {
    230, 230, 230, 230, 307, 409, 512, 614,
    768, 614, 512, 409, 307, 230, 230, 230
};

static const int16_t msadpcmAdaptCoeff1[] = {
    256, 512, 0, 192, 240, 460, 392
};

static const int16_t msadpcmAdaptCoeff2[] = {
    0, -256, 0, 64, 0, -208, -232
};

//-----------------------------------------------------------------------------
// Global API Functions
int16_t halInternalMsadpcmDecode(uint8_t compressedCode, HalMsadpcmState *state)
{
  int32_t predictedSample, uncompressedCode;

  compressedCode &= 0x0F;   // Ensure upper compressedCode is 0

  uncompressedCode = (int32_t)compressedCode; // Convert code to int32_t
  if (compressedCode & 0x08) {
    uncompressedCode -= 0x10;
  }
  // Calculate sample
  predictedSample = ((int32_t)state->sample1 * state->coeff1
                     + (int32_t)state->sample2 * state->coeff2) >> 8;
  predictedSample += uncompressedCode * state->delta;

  // Saturate to int16_t
  if (predictedSample > 32767) {
    predictedSample = 32767;
  } else if (predictedSample < -32768) {
      predictedSample = -32768;
  }

  // Update state variables
  state->sample2 = state->sample1;
  state->sample1 = (int16_t)predictedSample;

  state->delta = (msadpcmAdaptTable[compressedCode] * state->delta) >> 8;
  if (state->delta < 16) {
    state->delta = 16;
  }

  return state->sample1;
}

//-----------------------------------------------------------------------------
uint8_t halInternalMsadpcmEncode(int16_t realSample, HalMsadpcmState *state)
{
  int32_t predictedSample, uncompressedCode, adjustment;
  uint8_t compressedCode;

  predictedSample = ((int32_t)state->sample1 * state->coeff1
                     + (int32_t)state->sample2 * state->coeff2) >> 8;

  uncompressedCode = (int32_t)realSample - predictedSample;
  adjustment = state->delta >> 1;
  if (uncompressedCode < 0) {
    adjustment = -adjustment;
  }
  uncompressedCode = (uncompressedCode + adjustment) / state->delta;

  if (uncompressedCode > 7) {
    uncompressedCode = 7;
  } else if (uncompressedCode < -8) {
    uncompressedCode = -8;
  }
  compressedCode = (uint8_t)(uncompressedCode & 0x0F);

  predictedSample += uncompressedCode * state->delta;

  if (predictedSample > 32767) {
    predictedSample = 32767;
  } else if (predictedSample < -32768) {
    predictedSample = -32768;
  }

  state->sample2 = state->sample1;
  state->sample1 = (int16_t)predictedSample;

  state->delta = (msadpcmAdaptTable[compressedCode] * state->delta) >> 8;
  if (state->delta < 16) {
    state->delta = 16;
  }

  return compressedCode;
}

//-----------------------------------------------------------------------------
void halInternalMsadpcmResetState(HalMsadpcmState *state,
                       uint8_t predictor,
                       uint8_t resetDelta,
                       const int16_t *samples,
                       uint8_t numsamples)
{
  uint8_t i, j;
  uint8_t minPredictor;
  uint16_t savedDelta, minDelta;

  if ((predictor <= 6) || (numsamples < 3)) {
    if (predictor <= 6) {
      state->predictor = predictor;
    } else {
      state->predictor = 0;
    }

    if (resetDelta) {
      state->delta = 16;
    }

    if (numsamples < 2) {
      state->sample1 = 0;
      state->sample2 = 0;
    } else {
      state->sample1 = samples[1];
      state->sample2 = samples[0];
    }

    state->coeff1 = msadpcmAdaptCoeff1[state->predictor];
    state->coeff2 = msadpcmAdaptCoeff2[state->predictor];
  } else {
    savedDelta = state->delta;
    minPredictor = 0;
    minDelta = 0xffff;
    for (i=0; i <=6; i++) {
      state->predictor = i;
      state->delta = resetDelta ? 16 : savedDelta;
      state->sample1 = samples[1];
      state->sample2 = samples[0];

      state->coeff1 = msadpcmAdaptCoeff1[state->predictor];
      state->coeff2 = msadpcmAdaptCoeff2[state->predictor];

      for (j=2; j < numsamples; j++) {
        halInternalMsadpcmEncode(samples[j], state);
      }

      if (state->delta < minDelta) {
        minPredictor = i;
        minDelta = state->delta;
      }
    }

    state->predictor = minPredictor;
    state->delta = resetDelta ? 16 : savedDelta;
    state->sample1 = samples[1];
    state->sample2 = samples[0];

    state->coeff1 = msadpcmAdaptCoeff1[state->predictor];
    state->coeff2 = msadpcmAdaptCoeff2[state->predictor];
  }
}

//-----------------------------------------------------------------------------
void halInternalMsadpcmSaveState(uint8_t *stateDst, const HalMsadpcmState *stateSrc)
{
  // Save state in a packed form
  stateDst[0] = stateSrc->predictor;
  stateDst[1] = (uint8_t)( stateSrc->delta         & 0xff);
  stateDst[2] = (uint8_t)((stateSrc->delta   >> 8) & 0xff);
  stateDst[3] = (uint8_t)( stateSrc->sample1       & 0xff);
  stateDst[4] = (uint8_t)((stateSrc->sample1 >> 8) & 0xff);
  stateDst[5] = (uint8_t)( stateSrc->sample2       & 0xff);
  stateDst[6] = (uint8_t)((stateSrc->sample2 >> 8) & 0xff);
}

//-----------------------------------------------------------------------------
void halInternalMsadpcmRestoreState(HalMsadpcmState *stateDst, const uint8_t *stateSrc)
{
  // Restore state from a packed form
  stateDst->predictor = stateSrc[0];
  if (stateSrc[0] > 6) {
    stateDst->predictor = 6; // predictor range 0-6
  }

  stateDst->delta  = (int16_t)((((uint16_t)stateSrc[2]) << 8)
                                + ((uint16_t)stateSrc[1]));

  stateDst->sample1 = (int16_t)((((uint16_t)stateSrc[4]) << 8)
                                + ((uint16_t)stateSrc[3]));
  stateDst->sample2 = (int16_t)((((uint16_t)stateSrc[6]) << 8)
                                + ((uint16_t)stateSrc[5]));

  stateDst->coeff1 = msadpcmAdaptCoeff1[stateDst->predictor];
  stateDst->coeff2 = msadpcmAdaptCoeff2[stateDst->predictor];
}
