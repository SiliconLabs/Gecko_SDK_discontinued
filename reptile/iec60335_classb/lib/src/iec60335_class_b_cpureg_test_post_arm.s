;/***************************************************************************//**
; * @file   iec60335_class_b_cpureg_test_post_arm.s
; * @brief  assemble based POST CPU register test for ARM compiler
; *             IEC60335 Class B CPU register tests
; * @version 1.0.0
; *******************************************************************************
; * @section License
; * <b>Copyright 2015 Silicon Labs, http://www.silabs.com</b>
; *******************************************************************************
; *
; * This file is licensed under the Silabs License Agreement. See the file
; * "Silabs_License_Agreement.txt" for details. Before using this software for
; * any purpose, you must agree to the terms of that agreement.
; *
; ******************************************************************************/

    AREA    |.text|, CODE, READONLY
    THUMB
;/* functions exported */
    EXPORT    ASMCPUregTestPOST

;/* external struct */
    IMPORT    IEC60335_CPUregTestPOST

;/* Test structure offset definitions */
testState            EQU     0x0
testResult           EQU     0x4

;/* Test pattern definition */
pattern1             EQU     0xAAAAAAAA
pattern2             EQU     0xAAAAAAA8
pattern3             EQU     0x55555555
pattern4             EQU     0x55555554
pattern5             EQU     0xA8000000
pattern6             EQU     0x50000000
pattern7             EQU     0x000000A0
pattern8             EQU     0x00000060

;/* test case flags */
TestClear            EQU     0x00000000
TestState_r0         EQU     0x00000010
TestState_r1r7       EQU     0x00000020
TestState_r8r12      EQU     0x00000040
TestStateMSP         EQU     0x00000100
TestStatePSP         EQU     0x00000200
TestStateLR          EQU     0x00000400
TestStateAPSR        EQU     0x00000800
TestStatePRIMASK     EQU     0x00001000
TestStateFAULTMASK   EQU     0x00002000
TestStateBASEPRI     EQU     0x00004000

;/* test results */
TestResult_fail      EQU     0
TestResult_pass      EQU     1

;/* CPU register test */
ASMCPUregTestPOST
;/* push registers to stack (atomic instruction) */
    push     {r0-r12,r14}
    mov      r12, lr
    cmp      r12, lr
    bne.w    Test_CPU_SFR_fail

;/* clear IEC60335_CPUregTestPOST */
    ldr      r0,  =TestClear
    ldr      r8,  =TestState_r0
    ldr      r9,  =IEC60335_CPUregTestPOST
    str      r8,  [r9,#testState]
    str      r0,  [r9,#testResult]

;/* registers r0-r7 tests */
Test_r0
;/* register r0 test:
; * r0 is tested first by immediate instructions
; * r0 is tested first to keep it free for the following tests */
;/* cmp instruction to compare with immediate values
; * byte test requires shift to the LSB */

;/* r0[31:24] */
    movw     r0,  #0x0000
    movt     r0,  #0xAA00
    lsr      r0,  r0, #24
    cmp      r0,  #0xAA
    bne.w    Test_r0_Exit
;/* r0[23:16] */
    movw     r0,  #0x0000
    movt     r0,  #0x00AA
    lsr      r0,  r0, #16
    cmp      r0,  #0xAA
    bne.w    Test_r0_Exit
;/* r0[15:8] */
    movw     r0,  #0xAA00
    lsr      r0,  r0, #8
    cmp      r0,  #0xAA
    bne.w    Test_r0_Exit
;/* r0[7:0] */
    movw     r0,  #0x00AA
    cmp      r0,  #0xAA
    bne.w    Test_r0_Exit

;/* use inverse pattern */
;/* r0[31:24] */
    movw     r0,  #0x0000
    movt     r0,  #0x5500
    lsr      r0,  r0, #24
    cmp      r0,  #0x55
    bne.w    Test_r0_Exit
;/* test r0[23:16] */
    movw     r0,  #0x0000
    movt     r0,  #0x0055
    lsr      r0,  r0, #16
    cmp      r0,  #0x55
    bne.w    Test_r0_Exit
;/* test r0[15:8] */
    movw     r0,  #0x5500
    lsr      r0,  r0, #8
    cmp      r0,  #0x55
    bne.w    Test_r0_Exit
;/* test r0[7:0] */
    movw     r0,  #0x0055
    cmp      r0,  #0x55
    bne.w    Test_r0_Exit

;/* mark r0 as passed by calling next test */
Test_r0_pass
;/* continue with result */
    b        Test_r1_r7

Test_r1_r7
;/* register r1-r7 test:
; * step 1:
; * each registers will be written with pattern1 = 0xAAAA.AAAA
; * each register will be compared to r0 for the test.
; * step 2:
; * the inverse pattern pattern3 = 0x5555.5555 is written to the
; * register and compared to r0 */

;/* clear IEC60335_CPUregTestPOST */
    ldr      r8,  =TestState_r1r7
    ldr      r7,  [r9]
    orr      r8,  r8, r7
    str      r8,  [r9,#testState]

;/* load pattern1 to r0 */
    ldr      r0,  =pattern1
;/* load pattern1 to r1-r7 */
    mov      r1,  r0
    mov      r2,  r0
    mov      r3,  r0
    mov      r4,  r0
    mov      r5,  r0
    mov      r6,  r0
    mov      r7,  r0
;/* test pattern1 */
    cmp      r1,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r2,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r3,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r4,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r5,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r6,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r7,  r0
    bne.w    Test_r1_r7_Exit

;/* load pattern3 to r0 */
    ldr      r0,  =pattern3
;/* load pattern3 to r1-r7 */
    mov      r1,  r0
    mov      r2,  r0
    mov      r3,  r0
    mov      r4,  r0
    mov      r5,  r0
    mov      r6,  r0
    mov      r7,  r0
;/* test pattern3 */
    cmp      r1,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r2,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r3,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r4,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r5,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r6,  r0
    bne.w    Test_r1_r7_Exit
    cmp      r7,  r0
    bne.w    Test_r1_r7_Exit

Test_r1_r7_pass
;/* continue with result */
    b        Test_r8_r12

;/* registers r8-r12 tests */
Test_r8_r12
;/* step 1:
; * each registers will be written with pattern1 = 0xAAAA.AAAA
; * each register will be compared to r0 for the test.
; * step 2:
; * the inverse pattern pattern3 = 0x5555.5555 is written to the
; * register and compared to r0 */

;/* save LR to r3 */
    mov      r3,  r12
;/* load IEC60335_CPUregTestPOST with test state */
    ldr      r5,  =TestState_r8r12
    ldr      r6,  =IEC60335_CPUregTestPOST
    ldr      r7,  [r6]
    orr      r5,  r5, r7
    str      r5,  [r6,#testState]

;/* load pattern1 to r0 */
    ldr      r0,  =pattern1
;/* load pattern1 to r8-r12 */
    mov      r8,  r0
    mov      r9,  r0
    mov      r10, r0
    mov      r11, r0
    mov      r12, r0
;/* test patter1 */
    cmp      r8,  r0
    bne.w    Test_r8_r12_Exit
    cmp      r9,  r0
    bne.w    Test_r8_r12_Exit
    cmp      r10, r0
    bne.w    Test_r8_r12_Exit
    cmp      r11, r0
    bne.w    Test_r8_r12_Exit
    cmp      r12, r0
    bne.w    Test_r8_r12_Exit

;/* load pattern3 to r0 */
    ldr      r0,  =pattern3
;/* load pattern3 to r8-r12 */
    mov      r8,  r0
    mov      r9,  r0
    mov      r10, r0
    mov      r11, r0
    mov      r12, r0
;/* test pattenr3 */
    cmp      r8,  r0
    bne.w    Test_r8_r12_Exit
    cmp      r9,  r0
    bne.w    Test_r8_r12_Exit
    cmp      r10, r0
    bne.w    Test_r8_r12_Exit
    cmp      r11, r0
    bne.w    Test_r8_r12_Exit
    cmp      r12, r0
    bne.w    Test_r8_r12_Exit

Test_r8_r12_pass
;/* restore LR */
    mov      r12, r3
;/* continue with result */
    b        Test_MSP

Test_r8_r12_Exit
;/* restore LR */
    mov      r12, r3
    b        Test_CPU_SFR_fail

;/* MainSP test: */
Test_MSP
;/* step 1:
; * select the MSP as stack pointer and
; * use mov command to read/write with different pattern
; * compare to r0 for the test.
; * step 2:
; * use mrs/msr command to read/write with different pattern
; * register and compared to r0
; * MSP[1:0] is always read as zero */

;/* save the CONTROL register value */
    mrs      r3,  CONTROL
;/* load IEC60335_CPUregTestPOST with test state */
    ldr      r8,  =TestStateMSP
    ldr      r9,  =IEC60335_CPUregTestPOST
    ldr      r10, [r9]
    orr      r8,  r8, r10
    str      r8,  [r9,#testState]

;/* set MSP as default stack */
    mov      r6,  #0x0
    msr      CONTROL, r6
;/* save current MSP content */
    mov      r4,  r13

;/* verify MSP as stack pointer */
    mrs      r5,  MSP
    cmp      r4,  r5
    bne      Test_MSP_Exit

;/* load pattern2 to MSP */
    ldr      r0,  =pattern2
    mov      r13, r0
;/* compare with r0 */
    cmp      r13, r0
    bne      Test_MSP_Exit

;/* load pattern4 to MSP */
    ldr      r0,  =pattern4
    mov      r13, r0
;/* compare with r0 */
    cmp      r13, r0
    bne      Test_MSP_Exit

;/* load pattern2 to MSP and read back r1 */
    ldr      r0,  =pattern2
    msr      MSP, r0
    mrs      r1,  MSP
;/* compare r1 with r0 (indirect) */
    cmp      r1,  r0
    bne      Test_MSP_Exit

;/* load pattern4 to MSP and read back r1 */
    ldr      r0,  =pattern4
    msr      MSP, r0
    mrs      r1,  MSP
;/* compare r1 with r0 (indirect) */
    cmp      r1,  r0
    bne      Test_MSP_Exit

Test_MSP_pass
;/* restore MSP and CONTROL */
    mov      r13, r4
    msr      CONTROL, r3
;/* continue with result */
    b        Test_PSP

Test_MSP_Exit
;/* restore MSP and CONTROL */
    mov      r13, r4
    msr      CONTROL, r3
    b        Test_CPU_SFR_fail

;/* PSP register test: */
Test_PSP
;/* step 1:
; * select the PSP as stack pointer and
; * use mov command to read/write with different pattern
; * compare to r0 for the test.
; * step 2:
; * use mrs/msr command to read/write with different pattern
; * register and compared to r0
; * PSP[1:0] is always read as zero */

;/* save the CONTROL register value */
    mrs      r3,  CONTROL

;/* load IEC60335_CPUregTestPOST with test state */
    ldr      r8,  =TestStatePSP
    ldr      r10, [r9]
    orr      r8,  r8, r10
    str      r8,  [r9,#testState]

;/* set PSP as default stack */
    mov      r6,  #0x02
    msr      CONTROL, r6
;/* save current PSP content */
    mrs      r4,  PSP

;/* verify PSP as stack pointer */
    mrs      r5,  PSP
    cmp      r4,  r5
    bne      Test_PSP_Exit
;/* Step1 */
;/* load pattern2 to PSP */
    ldr      r0,  =pattern2
    mov      r13, r0
;/* compare with r0 */
    cmp      r13, r0
    bne      Test_PSP_Exit

;/* load pattern4 to PSP */
    ldr      r0,  =pattern4
    mov      r13, r0
;/* compare with r0 */
    cmp      r13, r0
    bne      Test_PSP_Exit

;/* Step2 */
;/* load pattern2 to PSP and read back r1 */
    ldr      r0,  =pattern2
    msr      PSP, r0
    mrs      r1,  PSP
;/* compare r1 with r0 (indirect) */
    cmp      r1,  r0
    bne      Test_PSP_Exit

;/* load pattern4 to PSP and read back r1 */
    ldr      r0,  =pattern4
    msr      PSP, r0
    mrs      r1,  PSP
;/* compare r1 with r0 (indirect) */
    cmp      r1,  r0
    bne      Test_PSP_Exit

Test_PSP_pass
;/* restore CONTROL and PSP */
    msr      PSP, r4
    msr      CONTROL, r3
;/* continue with result */
    b        Test_LR

Test_PSP_Exit
;/* restore CONTROL and PSP */
    msr      PSP, r4
    msr      CONTROL, r3
    b        Test_CPU_SFR_fail

;/* LR register test: */
Test_LR
;/* step 1:
; * LR registers will be written with pattern1 = 0xAAAA.AAAA
; * and will be compared to r0 for the test.
; * step 2:
; * the inverse pattern pattern3 = 0x5555.5555 is written to the
; * register and compared to r0 */

;/* save the LR register content */
    mov      r3,  r14

;/* load IEC60335_CPUregTestPOST with test state */
    ldr      r8,  =TestStateLR
    ldr      r10, [r9]
    orr      r8,  r8, r10
    str      r8,  [r9,#testState]

;/* load pattern1 to LR */
    ldr      r0,  =pattern1
    mov      r14, r0
;/* compare with r0 */
    cmp      r14, r0
    bne      Test_LR_Exit

;/* load pattern3 to LR */
    ldr      r0,  =pattern3
    mov      r14, r0
;/* compare with r0 */
    cmp      r14, r0
    bne      Test_LR_Exit

Test_LR_pass
;/* restore LR register */
    mov      r14, r3
;/* continue with result */
    b        Test_APSR

Test_LR_Exit
;/* restore LR register and branch to eror condition */
    mov      r14, r3
    b        Test_CPU_SFR_fail

;/* APSR register test: */
Test_APSR
;/* step 1:
; * APSR registers will be written with pattern5 = 0xA800.0000
; * and will be compared to r0 for the test.
; * step 2:
; * the inverse pattern pattern6 = 0x5000.0000 is written to the
; * register and read back to r1 and compared to r0
; * IPSR and EPSR are read-only */

;/* save the APSR register content */
    mrs      r3,  APSR

;/* load IEC60335_CPUregTestPOST with test state */
    ldr      r8,  =TestStateAPSR
    ldr      r10, [r9]
    orr      r8,  r8, r10
    str      r8,  [r9,#testState]

;/* load pattern5 to APSR */
    ldr      r0,  =pattern5
    msr      APSR, r0
;/* read and compare with r0 */
    mrs      r1,  APSR
    cmp      r1,  r0
    bne      Test_APSR_Exit

;/* load pattern6 to APSR */
    ldr      r0,  =pattern6
    msr      APSR, r0
;/* read and compare with r0 */
    mrs      r1,  APSR
    cmp      r1,  r0
    bne      Test_APSR_Exit

Test_APSR_pass
;/* restore APSR register */
    msr      APSR, r3
;/* continue with result */
    b        Test_PRIMASK

Test_APSR_Exit
;/* restore APSR register and branch to eror condition */
    msr      APSR, r3
    b        Test_CPU_SFR_fail

;/* PRIMASK register test: */
Test_PRIMASK
;/* step 1:
; * PRIMASK registers [:0] will be written with 0x0000.0000
; * and will be compared to r0 for the test.
; * step 2:
; * the inverse pattern 0x0000.0001 is written to the
; * register and read back to r1 and compared to r0 */

;/* save the PRIMASK register content */
    mrs      r3,  PRIMASK

;/* load IEC60335_CPUregTestPOST with test state */
    ldr      r8,  =TestStatePRIMASK
    ldr      r10, [r9]
    orr      r8,  r8, r10
    str      r8,  [r9,#testState]

;/* load pattern to PRIMASK */
    mov      r0,  #0
    msr      PRIMASK, r0
;/* read and compare with r0 */
    mrs      r1,  PRIMASK
    cmp      r1,  r0
    bne      Test_PRIMASK_Exit

;/* load pattern to PRIMASK */
    mov      r0,  #1
    msr      PRIMASK, r0
;/* read and compare with r0 */
    mrs      r1,  PRIMASK
    cmp      r1,  r0
    bne      Test_PRIMASK_Exit

Test_PRIMASK_pass
;/* restore PRIMASK register */
    msr      PRIMASK, r3
;/* continue with result */
    b        Test_FAULTMASK

Test_PRIMASK_Exit
;/* restore PRIMASK register */
    msr      PRIMASK, r3
    b        Test_CPU_SFR_fail

;/* FAULTMASK register test: */
Test_FAULTMASK
;/* step 1:
; * FAULTMASK registers [:0] will be written with 0x0000.0000
; * and will be compared to r0 for the test.
; * step 2:
; * the inverse pattern 0x0000.0001 is written to the
; * register and read back to r1 and compared to r0 */

;/* save the FAULTMASK register content */
    mrs      r3,  FAULTMASK

;/* load IEC60335_CPUregTestPOST with test state */
    ldr      r8,  =TestStateFAULTMASK
    ldr      r10, [r9]
    orr      r8,  r8, r10
    str      r8,  [r9,#testState]

;/* load pattern to FAULTMASK */
    mov      r0,  #0
    msr      FAULTMASK, r0
;/* read and compare with r0 */
    mrs      r1,  FAULTMASK
    cmp      r1,  r0
    bne      Test_FAULTMASK_Exit

;/* load pattern to FAULTMASK */
    mov      r0,  #1
    msr      FAULTMASK, r0
;/* read and compare with r0 */
    mrs      r1,  FAULTMASK
    cmp      r1,  r0
    bne      Test_FAULTMASK_Exit

Test_FAULTMASK_pass
;/* restore FAULTMASK register */
    msr      FAULTMASK, r3
;/* continue with result */
    b        Test_BASEPRI

Test_FAULTMASK_Exit
;/* restore FAULTMASK register */
    msr      FAULTMASK, r3
    b        Test_CPU_SFR_fail

;/* BASEPRI register test: */
Test_BASEPRI
;/* step 1:
; * BASEPRI registers [7:0] will be written with pattern7 = 0x0000.00A0
; * and will be compared to r0 for the test.
; * step 2:
; * the inverse pattern pattern8 = 0x0000.0055 is written to the
; * register and read back to r1 and compared to r0
; * BASEPRI[4:0] are read as 0 */

;/* save the BASEPRI register content */
    mrs      r3,  BASEPRI

;/* load IEC60335_CPUregTestPOST with test state */
    ldr      r8,  =TestStateBASEPRI
    ldr      r10, [r9]
    orr      r8,  r8, r10
    str      r8,  [r9,#testState]

;/* load pattern7 to BASEPRI */
    ldr      r0,  =pattern7
    msr      BASEPRI, r0
;/* read and compare with r0 */
    mrs      r1,  BASEPRI
    cmp      r1,  r0
    bne      Test_BASEPRI_Exit

;/* load pattern8 to BASEPRI */
    ldr      r0,  =pattern8
    msr      BASEPRI, r0
;/* read and compare with r0 */
    mrs      r1,  BASEPRI
    cmp      r1,  r0
    bne      Test_BASEPRI_Exit

Test_BASEPRI_pass
;/* restore BASEPRI register */
    msr      BASEPRI, r3
;/* load IEC60335_CPUregTestPOST with pass result */
    ldr      r0,  =TestResult_pass
    str      r0,  [r9,#testResult]
;/* continue with result */
    b        Test_CPU_SFR_Exit

Test_BASEPRI_Exit
;    /* Restore the BASEPRI */
    msr      BASEPRI, r3
    b        Test_CPU_SFR_fail

Test_r0_Exit
Test_r1_r7_Exit
Test_CPU_SFR_fail
;/* returns a positiv test result */
    ldr      r0,  =TestResult_fail
    str      r0,  [r9,#testResult]

Test_CPU_SFR_Exit
;/* pop the stack back to registers (atomic instruction) */
    pop      {r0-r12,r14}
;/* branch with exchange back */
    bx       lr
;/* end of CPU SFR registers tests */
    NOP
    END
