// Copyright 2014 Silicon Laboratories, Inc.

#ifndef __INFRARED_LED_DRIVER_EMIT_H__
#define __INFRARED_LED_DRIVER_EMIT_H__

#include PLATFORM_HEADER


#define HAL_INFRARED_LED_EMIT_TIMING_BUFFER_SIZE       ( 16)
#define HAL_INFRARED_LED_EMIT_BITS_BUFFER_SIZE         ( 200)
#define HAL_INFRARED_LED_EMIT_TOGGLE_SIZE              ( 5)
#define HAL_INFRARED_LED_EMIT_FRAME_TYPE_COUNT         ( 3)


enum {
  HAL_INFRARED_LED_EMIT_PRESS_FRAME = 0,
  HAL_INFRARED_LED_EMIT_REPEAT_FRAME,
  HAL_INFRARED_LED_EMIT_RELEASE_FRAME
};

// Header structure
typedef struct {
  uint8_t   bRepeatMode;
  uint8_t   bUseCarrier;
  uint8_t   bMarkSpace;
  uint8_t   uRepeatCount;
  uint16_t  wCarrierPeriod;
} HalInternalInfraredLedEmitHeader_t;

// Timing buffer structure
typedef struct {
  uint16_t mark;
  uint16_t space;
} HalInternalInfraredLedEmitTiming_t;

// Frame data structure
typedef struct {
  uint8_t *pBitIdxBuf;
  uint8_t  uBitCount;
} HalInternalInfraredLedEmitFrame_t;


extern HalInternalInfraredLedEmitHeader_t   halInternalInfraredLedEmitHeader;
extern HalInternalInfraredLedEmitTiming_t   halInternalInfraredLedEmitTimingBuf[ HAL_INFRARED_LED_EMIT_TIMING_BUFFER_SIZE];
extern uint8_t                                halInternalInfraredLedEmitTimingCnt;
extern uint8_t                                halInternalInfraredLedEmitBitIdxBuf[ HAL_INFRARED_LED_EMIT_BITS_BUFFER_SIZE];
extern HalInternalInfraredLedEmitFrame_t    halInternalInfraredLedEmitFrameBuf[ HAL_INFRARED_LED_EMIT_FRAME_TYPE_COUNT];
extern uint8_t                                halInternalInfraredLedEmitToggleT;
extern uint8_t                                halInternalInfraredLedEmitToggleS;
extern uint8_t                                halInternalInfraredLedEmitToggleFirst;
extern HalInternalInfraredLedEmitFrame_t    halInternalInfraredLedEmitToggleBuf[ HAL_INFRARED_LED_EMIT_TOGGLE_SIZE];


void halInternalInfraredLedEmitClr( void);
void halInternalInfraredLedEmitPress(void);
void halInternalInfraredLedEmitRepeat(void);
void halInternalInfraredLedEmitRelease(void);
void halInternalInfraredLedEmitToggleReset();
void halInternalInfraredLedEmitToggleStepToNext();

#endif // __INFRARED_LED_DRIVER_EMIT_H__
