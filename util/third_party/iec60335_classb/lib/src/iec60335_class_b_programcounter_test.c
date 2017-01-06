/***************************************************************************//**
 * @file        iec60335_class_b_programcounter_test.c
 * @brief       c based PC test
 *              POST and BIST PC test for all compiler
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

#include "iec60335_class_b.h"

/*! @addtogroup IEC60335_PC_TEST
 * @{
 */

/*!
 * @brief   private function referenced by POST and BIST tests
 * @param   data    RAM location to do something with (not relevant)
 * @return  IEC60335_B_PCTest_ReturnValues    table based enum value
 * @note    The function is placed in a specified code segment to force
 * the linker to locate it separately. The return value is specified by
 * the return value table with enumerations.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_B_PCTestCodeA"
#else
__attribute__((section(".IEC60335_B_PCTestCodeA")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
IEC60335_B_PCTest_ReturnValues IEC60335_B_PCTest_A(Data_t *data)
{
  data->number++;
  return VAL_A;
}

/*!
 * @brief   private function referenced by POST and BIST tests
 * @param   data    RAM location to do something with (not relevant)
 * @return  IEC60335_B_PCTest_ReturnValues    table based enum value
 * @note    The function is placed in a specified code segment to force
 * the linker to locate it separately. The return value is specified by
 * the return value table with enumerations.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_B_PCTestCodeB"
#else
__attribute__((section(".IEC60335_B_PCTestCodeB")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
IEC60335_B_PCTest_ReturnValues IEC60335_B_PCTest_B(Data_t *data)
{
  data->number++;
  return VAL_B;
}

/*!
 * @brief   private function referenced by POST and BIST tests
 * @param   data    RAM location to do something with (not relevant)
 * @return  IEC60335_B_PCTest_ReturnValues    table based enum value
 * @note    The function is placed in a specified code segment to force
 * the linker to locate it separately. The return value is specified by
 * the return value table with enumerations.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_B_PCTestCodeC"
#else
__attribute__((section(".IEC60335_B_PCTestCodeC")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
IEC60335_B_PCTest_ReturnValues IEC60335_B_PCTest_C(Data_t *data)
{
  data->number++;
  return VAL_C;
}

/*!
 * @brief   private function referenced by POST and BIST tests
 * @param   data    RAM location to do something with (not relevant)
 * @return  IEC60335_B_PCTest_ReturnValues    table based enum value
 * @note    The function is placed in a specified code segment to force
 * the linker to locate it separately. The return value is specified by
 * the return value table with enumerations.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_B_PCTestCodeD"
#else
__attribute__((section(".IEC60335_B_PCTestCodeD")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
IEC60335_B_PCTest_ReturnValues IEC60335_B_PCTest_D(Data_t *data)
{
  data->number++;
  return VAL_D;
}

/*!
 * @brief   private function referenced by POST and BIST tests
 * @param   data    RAM location to do something with (not relevant)
 * @return  IEC60335_B_PCTest_ReturnValues    table based enum value
 * @note    The function is placed in a specified code segment to force
 * the linker to locate it separately. The return value is specified by
 * the return value table with enumerations.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_B_PCTestCodeE"
#else
__attribute__((section(".IEC60335_B_PCTestCodeE"), long_call))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
IEC60335_B_PCTest_ReturnValues IEC60335_B_PCTest_E(Data_t *data)
{
  data->number++;
  return VAL_E;
}

/*!
 * @brief   private function referenced by POST and BIST tests
 * @param   data    RAM location to do something with (not relevant)
 * @return  IEC60335_B_PCTest_ReturnValues    table based enum value
 * @note    The function is placed in a specified code segment to force
 * the linker to locate it separately. The return value is specified by
 * the return value table with enumerations.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_B_PCTestCodeF"
#else
__attribute__((section(".IEC60335_B_PCTestCodeF"), long_call))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
IEC60335_B_PCTest_ReturnValues IEC60335_B_PCTest_F(Data_t *data)
{
  data->number++;
  return VAL_F;
}

/*!
 * @brief   function table to the PC test subroutines,
 * this table is Flash located to prevent RAM usage in POST tests
 */
 #ifdef TESSY
/*! @cond TESSY */
Exec_t PCTest_funcTable[6] = {
/*! @endcond */
#else
const Exec_t PCTest_funcTable[6] = {
#endif
  (Exec_t) IEC60335_B_PCTest_A,
  (Exec_t) IEC60335_B_PCTest_B,
  (Exec_t) IEC60335_B_PCTest_C,
  (Exec_t) IEC60335_B_PCTest_D,
  (Exec_t) IEC60335_B_PCTest_E,
  (Exec_t) IEC60335_B_PCTest_F
};

/*!
 * @brief   function table to the PC test subroutines return values,
 * this table is Flash located to prevent RAM usage in POST tests
 */
#ifdef TESSY
/*! @cond TESSY */
IEC60335_B_PCTest_ReturnValues PCTest_resultTable[6] = {
/*! @endcond */
#else
const IEC60335_B_PCTest_ReturnValues PCTest_resultTable[6] = {
#endif
  VAL_A,
  VAL_B,
  VAL_C,
  VAL_D,
  VAL_E,
  VAL_F
};

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */

#if defined (__CC_ARM)
/*! @cond RealView */
__ASM testResult_t IEC60335_ClassB_PCTest_POST(const Exec_t *fTable, const IEC60335_B_PCTest_ReturnValues *rTable)
{
;check input parameter
    CMP R0, #0
    BEQ param
    CMP R1, #0
    BEQ param
;save PCTest_funcTable
    MOV R8, R0
;save PCTest_resultTable
    MOV R9, R1
;loop
    LDR R4, =0x00
;   LDR R5, =0x18
;result
    LDR R6, =0x00
;save the Link register
    MOV R7, R14
; calculate function table to execute
loop
;load next action pointer
    MOV R0, R8
    ADD R0, R0, R4
    LDR R0, [R0]
    BLX R0
; check PCTest_resultTable value
    MOV R1, R9
    ADD R1, R1, R4
    LDR R1, [R1]
    ADD R4, R4, #4
    CMP R0, R1
    BNE err
    CMP R4, #24
    BNE loop
;good output
    LDR R6, =1
    B   exit
;error output
err
    LDR R6, =0
exit
;restore the Link register
    MOV R14, R7
    MOV R0, R6
param
    bx  lr
}
/*! @endcond */
#elif defined (__ICCARM__)
/*! @cond ICC */
/* IAR iccarm specific functions */
#pragma diag_suppress=Pe940

testResult_t IEC60335_ClassB_PCTest_POST(const Exec_t *fTable, const IEC60335_B_PCTest_ReturnValues *rTable)
{
//;save PCTest_funcTable
  __ASM("CMP R0, #0     \n" /* check input parameter */
        "BEQ.W param    \n"
        "CMP R1, #0     \n"
        "BEQ.W param    \n"
        "MOV R8, R0     \n" /* save PCTest_resultTable */
        "MOV R9, R1     \n"

        "MOV R4,#0      \n" /* loop */
                            /* LDR R5, =0x18 */
        "MOV R6,#0      \n" /* prepare result */
        "MOV R7, R14    \n" /* save the Link register */

        "loop: nop      \n" /* calculate function table to execute */
        "MOV R0, R8     \n" /* load next action pointer */
        "ADD R0, R0, R4 \n"
        "LDR R0, [R0]   \n"
        "BLX R0         \n"

        "MOV R1, R9     \n" /* check PCTest_resultTable value */
        "ADD R1, R1, R4 \n"
        "LDR R1, [R1]   \n"
        "ADD R4, R4, #4 \n"
        "CMP R0, R1     \n"
        "BNE.W err      \n"
        "CMP R4, #24    \n"
        "BNE.W loop     \n"

        "MOV R6, #1     \n" /* good output */
        "B.W   exit     \n"

        "err: nop       \n" /* error output */
        "MOV R6, #0     \n"
        "exit: nop      \n"

        "MOV R14, R7    \n" /* restore the Link register */
        "MOV R0, R6     \n"
        "param: nop     \n"
        "bx  lr");
}
#pragma diag_default=Pe940
/*! @endcond */
#elif defined   (__GNUC__)
/*! @cond GCC */
testResult_t IEC60335_ClassB_PCTest_POST(const Exec_t *fTable, const IEC60335_B_PCTest_ReturnValues *rTable);

testResult_t IEC60335_ClassB_PCTest_POST(const Exec_t *fTable, const IEC60335_B_PCTest_ReturnValues *rTable)
{
#ifdef TESSY
uint32_t result = 0;
#else
register uint32_t result;
#endif
__ASM volatile  ("MOV R8, %0     \n" : : "r" (fTable));
__ASM volatile  ("MOV R9, %0     \n" : : "r" (rTable));
__ASM volatile  ("LDR R6, =0x00  \n" /* reset result */
                 "CMP R8, #0     \n" /* check input parameter */
                 "BEQ param      \n"
                 "CMP R9, #0     \n"
                 "BEQ param      \n"
                 "LDR R4, =0x00  \n" /* reset counter */
                 "LDR R6, =0x00  \n"
                 "MOV R10, R7    \n"
                 "MOV R7, R14    \n"
                 "loop:          \n" /* start loop */
                 "MOV R0, R8     \n" /* generate next call address */
                 "ADD R0, R0, R4 \n"
                 "LDR R0, [R0]   \n"
                 "BLX  R0        \n" /* call subfunction */
                 "MOV R1, R9     \n" /* get expected result from table */
                 "ADD R1, R1, R4 \n"
                 "LDR R1, [R1]   \n"
                 "ADD R4, R4, #4 \n"
                 "CMP R0, R1     \n" /* compare results */
                 "BNE err        \n"
                 "CMP R4, #24    \n"
                 "BNE loop       \n" /* cont loop */
                 "LDR R6, =1     \n"
                 "B   exit       \n"
                 "err:           \n"
                 "LDR R6, =0     \n"
                 "exit:          \n"
                 "MOV R14, R7    \n"
                 "MOV R7, R10    \n"
                 "param:         \n"
                 "MOV %0, R6       " : "=r" (result));
    return(result);
}
/*! @endcond */
#elif defined   (__TASKING__)
/*! @cond TASKING */
    #error "not implemented yet !"              /*!< TASKING Compiler is not implemented   */
/*! @endcond */
#endif

/*!
 * @note   The C-function is not used in the multi-compiler environment.
 * It is for documentation purpose only
 *
 * testResult_t IEC60335_ClassB_PCTest_POST(const Exec_t *fTable, const IEC60335_B_PCTest_ReturnValues *rTable)
 * {
 *    UINT8 loop = 0;
 *    testResult_t Result = IEC60335_testFailed;
 *
 *    if ((fTable == 0) ||(rTable == 0)) {  // check parameter
 *      return (Result);
 *    }
 *    while (loop < 6) {    // march through tests
 *
 *       if (PCTest_resultTable[loop]==PCTest_funcTable[loop]) {
 *          loop ++;    // good condition
 *       }
 *       else {
 *          loop = 10;  // error condition
 *       }
 *    }
 *    if (loop == 6) {
 *       Result = IEC60335_testPassed;
 *    return (Result);
 * }
 */

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
testResult_t IEC60335_ClassB_PCTest_BIST(void)
{
  static uint8_t lastFunction = 0;
  testResult_t   result       = IEC60335_testFailed;
  Exec_t         pfAction;
  Data_t         data = { VAL_G, 100 };

  if ((PCTest_resultTable[0] == 0) || (PCTest_funcTable[0] == 0))
  {
    return result;
  }
  if (lastFunction > 5)
  {
    lastFunction = 0;
  }

  pfAction = PCTest_funcTable[lastFunction];

  if (PCTest_resultTable[lastFunction] == (uint32_t)(*pfAction)(&data))
  {
    result                          = IEC60335_testPassed;
    lastFunction > 4 ? lastFunction = 0 : lastFunction++;
  }

  return result;
}

/*!
 * @}
 */

/************************************** EOF *********************************/
