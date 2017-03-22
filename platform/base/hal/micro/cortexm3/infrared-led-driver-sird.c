// Copyright 2014 Silicon Laboratories, Inc.

#include "hal/micro/infrared-led-driver-emit.h"
#include "hal/micro/infrared-led-driver-sird.h"

//****************************************************************************
// sird database definitions

static const uint8_t samsungPowerToggle[] =
{
  // Header
  0x03,0x01,0x04,0x00,0x22,0x00,0x00,0x00,0xd2,0x00,
  // Counters
  0x8c,0x00,0x7d,0x00,
  0x8c,0x00,0xa4,0x01,
  0x71,0x04,0x65,0x04,
  0x8c,0x00,0xe6,0x2d,
  // Repeate frame
  0x21,0x11,0x00,0x00,0x01,0x11,0x00,0x00,0x00,0x01,
  0x00,0x00,0x01,0x10,0x11,0x11,0x13
};

const HalInfraredLedSirdDb_t halInfraredLedSirdDb[HAL_INFRARED_LED_SIRD_DB_ENTRIES] =
{
  { samsungPowerToggle, sizeof(samsungPowerToggle)},
};

//****************************************************************************
// Static functions

static void frameNibblesToBytes(uint8_t *pSrc, uint8_t *pDst, uint8_t uBitCount)
{
  uint8_t i;

  for (i=0 ; i<uBitCount ; i++) {
    if (i & 0x1) {
      *pDst = *pSrc & 0x0f;        // process lower nibble
      pSrc++;
    } else {
      *pDst = *pSrc >> 4;          // process upper nibble
    }
    pDst++;
  }
}

//****************************************************************************
// Public functions

int halInfraredLedsirdDecode(const uint8_t *irdPtr, uint8_t irdLen)
{
  uint8_t i;
  uint8_t byteCntHeader = 10;
  uint8_t byteCntTiming = 0;
  uint8_t byteCntFirst = 0;
  uint8_t byteCntRepeate = 0;
  uint8_t byteCntRelease = 0;
  uint8_t byteCntToggleT = 0;
  uint8_t byteCntToggle = 0;
  uint8_t expectedLen = 0;
  int   iTotalNumberOfBits = 0;
  uint8_t *pInp;
  uint8_t *pBitIdxBuf;

  // Validate the sird frame before decoding
  // Is the header complete?
  if (irdLen < byteCntHeader) {
    return 0;
  }
  // Get the count for each part of the frame
  halInternalInfraredLedEmitTimingCnt = irdPtr[2];
  halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_PRESS_FRAME].uBitCount = irdPtr[3];
  halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_REPEAT_FRAME].uBitCount = irdPtr[4];
  halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_RELEASE_FRAME].uBitCount = irdPtr[5];
  halInternalInfraredLedEmitToggleT = irdPtr[6];
  halInternalInfraredLedEmitToggleS = irdPtr[7];
  // Calculate how many bytes each part should be
  byteCntTiming = halInternalInfraredLedEmitTimingCnt*4;
  byteCntFirst = (halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_PRESS_FRAME].uBitCount + 1) / 2;
  byteCntRepeate = (halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_REPEAT_FRAME].uBitCount + 1) / 2;
  byteCntRelease = (halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_RELEASE_FRAME].uBitCount + 1) / 2;
  byteCntToggleT = (halInternalInfraredLedEmitToggleT + 1) / 2;
  byteCntToggle = ((halInternalInfraredLedEmitToggleS > 0) ? 1 : 0) + byteCntToggleT * halInternalInfraredLedEmitToggleS;
  expectedLen =
    byteCntHeader
    + byteCntTiming
    + byteCntFirst
    + byteCntRepeate
    + byteCntRelease
    + byteCntToggle;
  // Is there a match between the expected length and the actual length?
  if (irdLen != expectedLen) {
    return 0;
  }
  // Is there room to store timing
  if (halInternalInfraredLedEmitTimingCnt > HAL_INFRARED_LED_EMIT_TIMING_BUFFER_SIZE) {
    return 0;
  }
  // Is there room to store bits?
  iTotalNumberOfBits =
    (int)halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_PRESS_FRAME].uBitCount
    + (int)halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_REPEAT_FRAME].uBitCount
    + (int)halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_RELEASE_FRAME].uBitCount
    + ((int)halInternalInfraredLedEmitToggleT * (int)halInternalInfraredLedEmitToggleS);
  if (iTotalNumberOfBits > HAL_INFRARED_LED_EMIT_BITS_BUFFER_SIZE) {
    return 0;
  }
  //**********************************
  // Get stuff from the input
  pInp = (uint8_t *)irdPtr;
  pBitIdxBuf = halInternalInfraredLedEmitBitIdxBuf;
  halInternalInfraredLedEmitHeader.bRepeatMode = ((irdPtr[0] & 0x01) == 0x01) ? 1 : 0;
  halInternalInfraredLedEmitHeader.bUseCarrier = ((irdPtr[0] & 0x02) == 0x02) ? 1 : 0;
  halInternalInfraredLedEmitHeader.bMarkSpace = ((irdPtr[0] & 0x04) == 0x04) ? 1 : 0;
  halInternalInfraredLedEmitHeader.uRepeatCount = irdPtr[1] & 0x0f;
  halInternalInfraredLedEmitHeader.wCarrierPeriod = HIGH_LOW_TO_INT(irdPtr[9], irdPtr[8]);
  pInp += 10;
  // Get symbol timing
  for (i=0; i < halInternalInfraredLedEmitTimingCnt; i++) {
    halInternalInfraredLedEmitTimingBuf[i].mark = HIGH_LOW_TO_INT(irdPtr[11 + i * 4], irdPtr[10 + i * 4]);
    halInternalInfraredLedEmitTimingBuf[i].space = HIGH_LOW_TO_INT(irdPtr[13 + i * 4], irdPtr[12 + i * 4]);
  }
  pInp += byteCntTiming;
  // Get press frame definition
  if (halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_PRESS_FRAME].uBitCount > 0) {
    halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_PRESS_FRAME].pBitIdxBuf = pBitIdxBuf;
    frameNibblesToBytes(pInp, pBitIdxBuf, halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_PRESS_FRAME].uBitCount);
  }
  pInp += byteCntFirst;
  pBitIdxBuf += halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_PRESS_FRAME].uBitCount;
  // Get repeate frame definition
  if (halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_REPEAT_FRAME].uBitCount > 0) {
    halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_REPEAT_FRAME].pBitIdxBuf = pBitIdxBuf;
    frameNibblesToBytes(pInp, pBitIdxBuf, halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_REPEAT_FRAME].uBitCount);
  }
  pInp += byteCntRepeate;
  pBitIdxBuf += halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_REPEAT_FRAME].uBitCount;
  // Get release frame definition
  if (halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_RELEASE_FRAME].uBitCount > 0) {
    halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_RELEASE_FRAME].pBitIdxBuf = pBitIdxBuf;
    frameNibblesToBytes(pInp, pBitIdxBuf, halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_RELEASE_FRAME].uBitCount);
  }
  pInp += byteCntRelease;
  pBitIdxBuf += halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_RELEASE_FRAME].uBitCount;
  // Get toggle frame definition
  if ((halInternalInfraredLedEmitToggleS > 0) && (halInternalInfraredLedEmitToggleT > 0)) {
    halInternalInfraredLedEmitToggleFirst = *pInp;
    pInp++;
    for (i = 0; i < halInternalInfraredLedEmitToggleS; i++) {
      halInternalInfraredLedEmitToggleBuf[i].uBitCount = halInternalInfraredLedEmitToggleT;
      halInternalInfraredLedEmitToggleBuf[i].pBitIdxBuf = pBitIdxBuf;
      frameNibblesToBytes(pInp, pBitIdxBuf, halInternalInfraredLedEmitToggleT);
      pInp += byteCntToggleT;
      pBitIdxBuf += halInternalInfraredLedEmitToggleT;
    }
  }
  return iTotalNumberOfBits;
}


