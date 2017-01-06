/***************************************************************************//**
 * @file    iec60335_class_b_typedef.h
 * @brief   global types and definitions
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef __IEC60335_CLASS_B_TYPEDEF_H__
#define __IEC60335_CLASS_B_TYPEDEF_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum testResult_enum
{
  IEC60335_testFailed     = 0,              /*!< test result failed replacement */
  IEC60335_testPassed     = 1,              /*!< test result passed replacement */
  IEC60335_testInProgress = 2               /*!< test is still in progress replacement */
} testResult_t;

typedef enum testState_enum
{
  IEC60335_stateDone       = 0,             /*!< test is done replacement */
  IEC60335_statePending    = 1,             /*!< test is pending replacement */
  IEC60335_stateInProgress = 2              /*!< test is still in progress replacement */
} type_testState_t;

typedef enum testStart_enum
{
  IEC60335_stop  = 0,                       /*!< test command stop */
  IEC60335_start = 1                        /*!< test command start */
} type_testStart_t;

/* CPU register test */
typedef struct
{
  unsigned int testState;                   /*!< CPU register POST tests passed */
  testResult_t testResult;                  /*!< CPU register POST test status (PASS/FAIL) */
} IEC60335_CPUreg_struct;

/* clock test */
typedef struct ClkTest_struct
{
  uint32_t Ratio;                           /*!< ration of timer ticks to RTC ticks, defines the number of timer calls in the test*/
  uint32_t Tolerance;                       /*!< tolerance of timer ticks, defines the deviation acceptable in the test*/
  uint32_t timerCounter;                    /*!< The counter Variable in the test */
  uint32_t baseTicks;                       /*!< basic counter occurrences */
  uint32_t result;                          /*!< result after test finished = (testResult_t + 1) */
} ClockTest_t;

/* Interrupt test */
#define _VAL_A    0x01234567                /*!< values fixed to 32 bit regardless of compiler */
#define _VAL_B    0x12345678
#define _VAL_C    0x23456789
#define _VAL_D    0x3456789A
#define _VAL_E    0x456789AB
#define _VAL_F    0x56789ABC
#define _VAL_G    0x6789ABCD
/* possible return values */
enum _ReturnValues
{
  VAL_A = _VAL_A,
  VAL_B = _VAL_B,
  VAL_C = _VAL_C,
  VAL_D = _VAL_D,
  VAL_E = _VAL_E,
  VAL_F = _VAL_F,
  VAL_G = _VAL_G,
};
typedef enum _ReturnValues   IEC60335_B_PCTest_ReturnValues;

/*! @brief Structure for interrupt test configuration.
 *
 */
typedef struct
{
  uint32_t Active;                              /*!< flag register indicating active on test */
  uint32_t Mode;                                /*!< flag register indicating original vector wanted */
} IEC60335_IRQ_Test_t;

/*! @brief type structure for interrupt testing.
 *
 *  For each interrupt, which has to be tested, there has to be one instance of this structure.
 *  To initialize the struct, use the initialization function:
 *  \a IEC60335_ClassB_InitInterruptTest(IRQn_Type IRQn, uint8_t Mode, IRQTestData_t *CountSetup).
 */
typedef struct
{
  uint32_t EffectiveCount;                      /*!< counter Variable*/
  uint32_t MinThres;                            /*!< minimum threshold target of interrupt occurrences*/
  uint32_t MaxThres;                            /*!< maximum threshold target of interrupt occurrences*/
} IRQTestData_t;

/* Program Counter test */
struct _Data
{
  IEC60335_B_PCTest_ReturnValues ret;
  uint32_t                       number;        /*!< action type */
};
typedef struct _Data   Data_t;

typedef IEC60335_B_PCTest_ReturnValues (*Exec_t)(Data_t *data);


/* Flash test */
typedef struct IEC60335_FlashSign
{
  uint32_t CRC32Val;                            /*!< CRC32 value */
  uint32_t MemLenght;                           /*!< memory length for calculation */
  uint32_t NextAddress;                         /*!< next address in Flash */
  uint32_t BlockSize;                           /*!< block size of Flash */
  uint32_t Status;                              /*!< status information of this struct */
} FlashCRC_t;

#define MIN(a, b)             ((a < b) ? a : b)     /*!< macro returning smallest input */
#define MAX(a, b)             ((a > b) ? a : b)     /*!< macro returning biggest input */
#define CUT(a, b, c)          (MAX(a, MIN(b, c)))   /*!< macro returning a (a>b>c),b (a<b<c), c(a<c<b) */
#define IsInRange(a, b, c)    ((CUT(a, b, c) == b) ? true : false) /*!< macro returning true if a<b<c */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __IEC60335_CLASS_B_TYPEDEF_H__ */

/************************************** EOF *********************************/
