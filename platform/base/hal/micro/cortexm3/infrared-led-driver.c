// Copyright 2014 Silicon Laboratories, Inc.
//-----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"
#include "hal/micro/infrared-led-driver.h"
#include "hal/micro/infrared-led-driver-emit.h"
#include "hal/micro/infrared-led-driver-sird.h"
#include "hal/micro/infrared-led-driver-uird.h"

#define DEFAULT_FRAME_INTERVAL_IN_MS    (0)
#define NODELAY                         (0)

//****************************************************************************

typedef enum
{
  IDLE,
  START,
  REPEAT,
  RELEASE,
  STOP,
} STATE_t;

EmberEventControl halInfraredLedEventControl;

static uint32_t   frameIntervalInMs = DEFAULT_FRAME_INTERVAL_IN_MS;

static uint8_t    *irdPtrCurrent = NULL;
static uint8_t    irdLenCurrent = 0;
static STATE_t    state = IDLE;
static uint8_t    repeateCount = 0;
static bool       reqStart = 0;
static bool       reqStop = 0;
static uint8_t    driverStatus = HAL_INFRARED_LED_STATUS_OK;

static uint8_t      uirdDecryptBuf[HAL_INFRARED_LED_UIRD_DB_STRUCT_SIZE];

//****************************************************************************
// Local functions

// Return the number of milliseconds to the start of the next frame.
static uint32_t getDelayToNextFrameInMs(uint32_t startTimeInMs,
                                        uint32_t intervalInMs)
{
  uint32_t now;
  uint32_t elapsedMs;
  uint32_t delayMs;

  now = halCommonGetInt32uMillisecondTick();
  elapsedMs = elapsedTimeInt32u(startTimeInMs, now);
  // Adjust for ..MillisecondTick function returning 1024 ticks pr second.
  elapsedMs = elapsedMs * 1000 / 1024;
  delayMs = (elapsedMs >= intervalInMs) ? 0 : (intervalInMs - elapsedMs);
//  emberAfCorePrintln( "st=%d, now=%d, el=%d, delay=%d", startTimeInMs, now, elapsedMs, delayMs);
  return delayMs;
}

static void setEvent(uint32_t delayMs)
{
  if( delayMs == 0) {
    emberEventControlSetActive(halInfraredLedEventControl);
  } else {
    emberEventControlSetDelayMS(halInfraredLedEventControl, delayMs);
  }
}

// Decode external data.
// The format must be specified in the irDbFormat,
// ird must point to the database and irdLen must specify the length.
static uint8_t infraredLedDecodeData(HalInfraredLedDbFormat irDbFormat,
                                     const uint8_t *ird,
                                     uint8_t irdLen)
{
  uint8_t decodeStatus;
  int length;

  decodeStatus = HAL_INFRARED_LED_STATUS_OK;
  halInternalInfraredLedEmitClr();
  if (irDbFormat == HAL_INFRARED_LED_DB_FORMAT_UIRD) {
    length = halInfraredLedUirdDecode(ird, irdLen);
    if (length <= 0) {
      decodeStatus = HAL_INFRARED_LED_STATUS_DECODE_ERROR;
    }
  } else if (irDbFormat == HAL_INFRARED_LED_DB_FORMAT_UIRD_ENCRYPTED) {
    if (irdLen > HAL_INFRARED_LED_UIRD_DB_STRUCT_SIZE) {
      decodeStatus = HAL_INFRARED_LED_STATUS_INVALID_LENGTH;
    } else {
      // decrypt
      length = halInfraredLedUirdDecrypt(uirdDecryptBuf, ird, irdLen);
      if (length == 0) {
        decodeStatus = HAL_INFRARED_LED_STATUS_DECRYPT_ERROR;
      } else {
        // Decode from the decrypt buffer
        length = halInfraredLedUirdDecode(uirdDecryptBuf, irdLen);
        if (length <= 0) {
          decodeStatus = HAL_INFRARED_LED_STATUS_DECODE_ERROR;
        }
      }
    }
  } else if (irDbFormat == HAL_INFRARED_LED_DB_FORMAT_SIRD) {
    length = halInfraredLedsirdDecode(ird, irdLen);
    if (length <= 0) {
      decodeStatus = HAL_INFRARED_LED_STATUS_DECODE_ERROR;
    }
  } else {
    decodeStatus = HAL_INFRARED_LED_STATUS_UNKNOWN_FORMAT;
  }
  if (decodeStatus != HAL_INFRARED_LED_STATUS_OK) {
    halInternalInfraredLedEmitClr();
  }
  return decodeStatus;
}

// Decode ir data in the local database.
// The format must be specified in the irDbFormat,
// the index is specified in the dbIndex.
static int infraredLedDecodeIndex(HalInfraredLedDbFormat irDbFormat,
                                  uint8_t dbIndex)
{
  uint8_t decodeStatus;
  uint8_t *irdPtr;
  uint8_t irdLen;

  decodeStatus = HAL_INFRARED_LED_STATUS_OK;
  if (irDbFormat == HAL_INFRARED_LED_DB_FORMAT_SIRD) {
    if (dbIndex >= HAL_INFRARED_LED_SIRD_DB_ENTRIES) {
      decodeStatus = HAL_INFRARED_LED_STATUS_INDEX_OUT_OF_RANGE;
    } else {
      irdPtr = ( uint8_t *)halInfraredLedSirdDb[ dbIndex].pDat;
      irdLen = halInfraredLedSirdDb[ dbIndex].uLen;
    }
  } else if (irDbFormat == HAL_INFRARED_LED_DB_FORMAT_UIRD) {
    if (dbIndex >= HAL_INFRARED_LED_UIRD_DB_ENTRIES) {
      decodeStatus = HAL_INFRARED_LED_STATUS_INDEX_OUT_OF_RANGE;
    } else {
      irdPtr = (uint8_t *)&halInfraredLedUirdDatabase[ dbIndex];
      irdLen = HAL_INFRARED_LED_UIRD_DB_STRUCT_SIZE;
    }
  } else if (irDbFormat == HAL_INFRARED_LED_DB_FORMAT_UIRD_ENCRYPTED) {
    if (dbIndex >= HAL_INFRARED_LED_UIRD_ENCR_DB_ENTRIES) {
      decodeStatus = HAL_INFRARED_LED_STATUS_INDEX_OUT_OF_RANGE;
    } else {
      irdPtr = (uint8_t *)&halInfraredLedUirdEncryptDatabase[dbIndex];
      irdLen = HAL_INFRARED_LED_UIRD_ENCR_DB_STRUCT_SIZE;
    }
  } else {
    decodeStatus = HAL_INFRARED_LED_STATUS_UNKNOWN_FORMAT;
  }
  if (decodeStatus == HAL_INFRARED_LED_STATUS_OK) {
    decodeStatus = infraredLedDecodeData(irDbFormat, irdPtr, irdLen);
  }
  return decodeStatus;
}

//****************************************************************************
// Driver interface

// Initialize the driver.
void halInfraredLedInitialize(void)
{
  reqStart = false;
  reqStop = false;
  state = IDLE;
  irdPtrCurrent = 0;
  irdLenCurrent = 0;
  halInternalInfraredLedEmitClr();
}

// Set the interval to control how often frames are emitted.
// The value specifies the minimum periode between start of frames.
void halInfraredLedSetFrameIntervalInMs(uint32_t intervalInMs)
{
    frameIntervalInMs = intervalInMs;
}

// This is the state machine that emits one frame at a time according to the
// format in the ir-database. It is controlled by the halInfraredLedStart and
// halInfraredLedStop functions and the event set by itself - only.
void halInfraredLedEventHandler(void)
{
  uint32_t startTimeInMs;
  uint32_t delayMs;

  emberEventControlSetInactive(halInfraredLedEventControl);

  switch(state) {
  case IDLE:
    if (reqStart) {
      state = START;
      setEvent(NODELAY);
    } else if (reqStop) {
      state = STOP;
      setEvent(NODELAY);
    }
    break;

  case START:
    startTimeInMs = halCommonGetInt32uMillisecondTick();
    halInternalInfraredLedEmitPress();
    repeateCount = halInternalInfraredLedEmitHeader.uRepeatCount;
    if (repeateCount > 0) {
      repeateCount--;
    }
    state = REPEAT;
    delayMs = getDelayToNextFrameInMs(startTimeInMs, frameIntervalInMs);
    setEvent(delayMs);
    break;

  case REPEAT:
    if (repeateCount > 0) {
      repeateCount--;
      startTimeInMs = halCommonGetInt32uMillisecondTick();
      halInternalInfraredLedEmitRepeat();
      delayMs = getDelayToNextFrameInMs(startTimeInMs, frameIntervalInMs);
      setEvent(delayMs);
    } else if (reqStop) {
      startTimeInMs = halCommonGetInt32uMillisecondTick();
      halInternalInfraredLedEmitRelease();
      state = STOP;
      setEvent(NODELAY);
    } else if (halInternalInfraredLedEmitHeader.bRepeatMode) {
      startTimeInMs = halCommonGetInt32uMillisecondTick();
      halInternalInfraredLedEmitRepeat();
      delayMs = getDelayToNextFrameInMs(startTimeInMs, frameIntervalInMs);
      setEvent(delayMs);
    } else {
      state = RELEASE;
    }
    break;

  case RELEASE:
    startTimeInMs = halCommonGetInt32uMillisecondTick();
    halInternalInfraredLedEmitRelease();
    state = STOP;
    setEvent(NODELAY);
    break;

  case STOP:
    halInternalInfraredLedEmitToggleStepToNext();
    reqStart = false;
    reqStop = false;
    state = IDLE;
    irdPtrCurrent = NULL;
    irdLenCurrent = 0;
    break;
  }
}

// Start the transmissions of a sequence of IR frames. The format must
// be specified in the IR database entry.
uint8_t halInfraredLedStart(HalInfraredLedDbFormat irDbFormat,
                          const uint8_t *ird,
                          uint8_t irdLen)
{
static uint8_t *irdPtrPrevious;
static uint8_t irdLenPrevious;

  // Is there a requests in progress?
  if ((irdPtrCurrent != 0) || (irdLenCurrent != 0)) {
    return HAL_INFRARED_LED_STATUS_BUSY;
  }
  irdPtrCurrent = (uint8_t *)ird;
  irdLenCurrent = irdLen;
  // Is the current request different from the previous?
  if ((ird != irdPtrPrevious) || (irdLen != irdLenPrevious)) {
    // This is a new format
    irdPtrPrevious = (uint8_t *)ird;
    irdLenPrevious = irdLen;
    // Always reset the toggle counter for the SIRD format
    if (irDbFormat == HAL_INFRARED_LED_DB_FORMAT_SIRD) {
      halInternalInfraredLedEmitToggleReset();
    }
  }
  if (ird == 0) {
    // If the pointer is 0, irdLen shall be interpreted as the index.
    driverStatus = infraredLedDecodeIndex(irDbFormat, irdLen);
  } else {
    driverStatus = infraredLedDecodeData(irDbFormat, ird, irdLen);
  }
  if (driverStatus == HAL_INFRARED_LED_STATUS_OK) {
    reqStart = true;
    if (state == IDLE) {
      emberEventControlSetActive(halInfraredLedEventControl);
    }
  }
  else {
    irdPtrCurrent = NULL;
    irdLenCurrent = 0;
    irdPtrPrevious = NULL;
    irdLenPrevious = 0;
  }
  return driverStatus;
}

// Stop the ongoing sequence of IR transmissions and transmit the final
// IR frame. The format must be specified in the IR database entry.
uint8_t halInfraredLedStop(HalInfraredLedDbFormat irDbFormat,
                         const uint8_t *ird,
                         uint8_t irdLen)
{
  if (reqStart == true) {
    // If the driver has been started, it shall be stopped.
    reqStop = true;
    emberEventControlSetActive(halInfraredLedEventControl);
  }
  else {
    // If the driver has not been started, reset variables.
    reqStop = false;
    state = IDLE;
    irdPtrCurrent = NULL;
    irdLenCurrent = 0;
  }

  return HAL_INFRARED_LED_STATUS_OK;
}
