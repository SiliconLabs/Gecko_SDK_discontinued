// File: duty-cycle-test.h
// Description: Duty-Cycle Test API
//
// Copyright 2015 Silicon Laboratories, Inc.                                *80*

#ifndef DUTY_CYCLE_TEST_H_INCLUDED
#define DUTY_CYCLE_TEST_H_INCLUDED

#ifndef DOXYGEN_SHOULD_SKIP_THIS

//-- Private Test APIs

#ifdef  EMBER_NO_STACK
extern uint32_t DUAL_DCL(dcTxDelaysMs); // Exposed stat for test code
#endif//EMBER_NO_STACK

#define DC_NUM_BUCKETS   ((EM_PHY_DC_PERIOD_SECS / EM_PHY_DC_BUCKET_SECS) + 1)
// aDUTYCYCLEBuckets: The + 1 is regulatory guarantee due to bucket granularity

typedef struct EmPhyDutyCycle {
  int32_t  dcPeriodMaximumBytes;          // Maximum possible bytes in period
  int32_t  dcPeriodCriticalBytes;         // Critical Threshold bytes for period
  int32_t  dcPeriodThresholdBytes;        // Threshold bytes for period
  int32_t  dcPeriodLbtRdcBytes;           // LBT regulatory bytes allowed in period
  int32_t  dcPeriodNonRdcBytes;           // Non-LBT bytes allowed in period
  int32_t  dcPeriodConsumedBytes;         // Our 'macLBTTDurationMs'
  uint32_t dcLastBucketStartMsTick;       // Most recent bucket's start time
  int32_t  dcBucketBytes[DC_NUM_BUCKETS]; // Bytes sent in each time bucket of period
  EmPhyDutyCycleParams dcParams;          // Duty Cycle parameters for this channel
  uint8_t  macPgChanPlusOne;              // Channel+1 for this duty cycle structure
               // PlusOne so initial value of 0 means INVALID_CHANNEL
} EmPhyDutyCycle;

/** @brief Return pointer to the EmPhyDutyCycle{} for the current channel.
 *  @return Pointer to the structure for this channel, possibly NULL.
 */
extern EmPhyDutyCycle* emPhyDcEntry(void);

/** @brief Return index of the bucket for the indicated time.
 *  @param The time in MS ticks.
 *  @return Bucket index.
 */
extern uint8_t emPhyDcBucketIndex(uint32_t timeMsTick);

/** @brief Set a time acceleration factor for testing duty cycle behavior.
 * @param timeAccel A time multiplier; if 0, time effectively stops.
 */
extern void emPhyDcSetTimeAccel(uint16_t timeAccel);

/** @brief Get the current time acceleration factor.
 * @return The time multiplier previously set via emPhyDcSetTimeAccel().
 */
extern uint16_t emPhyDcGetTimeAccel(void);

#endif//DOXYGEN_SHOULD_SKIP_THIS

#endif//DUTY_CYCLE_TEST_H_INCLUDED
