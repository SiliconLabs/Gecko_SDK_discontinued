@
@********************************************************************************************************
@                                                uC/OS-III
@                                          The Real-Time Kernel
@
@
@                           (c) Copyright 2009-2016; Micrium Inc.; Weston, FL
@                    All rights reserved.  Protected by international copyright laws.
@
@                                           ARM Cortex-M3 Port
@
@ File      : OS_CPU_A.ASM
@ Version   : V3.06.00
@ By        : JJL
@             FT
@
@ For       : ARMv7M Cortex-M3
@ Mode      : Thumb2
@ Toolchain : GNU C Compiler
@********************************************************************************************************
@

@/*
@*********************************************************************************************************
@*********************************************************************************************************
@*                               WARNING - DEPRECATION NOTICE - WARNING
@* June 2016
@* This file is part of a deprecated port and will be removed in a future release.
@* The functionalities of this port were replaced by the generic ARM-Cortex-M port.
@*********************************************************************************************************
@*********************************************************************************************************
@*/

@********************************************************************************************************
@                                          PUBLIC FUNCTIONS
@********************************************************************************************************

.extern  OSRunning                                           @ External references.
.extern  OSPrioCur
.extern  OSPrioHighRdy
.extern  OSTCBCurPtr
.extern  OSTCBHighRdyPtr
.extern  OSIntExit
.extern  OSTaskSwHook
.extern  OS_CPU_ExceptStkBase


.global  OSStartHighRdy                                      @ Functions declared in this file.
.global  OS_CPU_PendSVHandler

@********************************************************************************************************
@                                               EQUATES
@********************************************************************************************************

.equ NVIC_INT_CTRL,   0xE000ED04                              @ Interrupt control state register.
.equ NVIC_SYSPRI14,   0xE000ED22                              @ System priority register (priority 14).
.equ NVIC_PENDSV_PRI, 0xFF                                    @ PendSV priority value (lowest).
.equ NVIC_PENDSVSET,  0x10000000                              @ Value to trigger PendSV exception.


@********************************************************************************************************
@                                     CODE GENERATION DIRECTIVES
@********************************************************************************************************

.text
.align 2
.thumb
.syntax unified


@********************************************************************************************************
@                                         START MULTITASKING
@                                      void OSStartHighRdy(void)
@
@ Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
@              the first task to start.
@
@           2) OSStartHighRdy() MUST:
@              a) Setup PendSV exception priority to lowest;
@              b) Set initial PSP to 0, to tell context switcher this is first run;
@              c) Set the main stack to OS_CPU_ExceptStkBase
@              d) Trigger PendSV exception;
@              e) Enable interrupts (tasks will run with interrupts enabled).
@********************************************************************************************************
.thumb_func
OSStartHighRdy:
    CPSID   I                                                   @ Prevent interruption during context switch
    MOVW    R0, #:lower16:NVIC_SYSPRI14                         @ Set the PendSV exception priority
    MOVT    R0, #:upper16:NVIC_SYSPRI14

    MOVW    R1, #:lower16:NVIC_PENDSV_PRI
    MOVT    R1, #:upper16:NVIC_PENDSV_PRI
    STRB    R1, [R0]

    MOVW    R0, #:lower16:OS_CPU_ExceptStkBase                  @ Initialize the MSP to the OS_CPU_ExceptStkBase
    MOVT    R0, #:upper16:OS_CPU_ExceptStkBase
    LDR     R1, [R0]
    MSR     MSP, R1

    MOVW    R0, #:lower16:OSPrioCur                             @ OSPrioCur   = OSPrioHighRdy;
    MOVT    R0, #:upper16:OSPrioCur
    MOVW    R1, #:lower16:OSPrioHighRdy
    MOVT    R1, #:upper16:OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    MOVW    R5, #:lower16:OSTCBCurPtr
    MOVT    R5, #:upper16:OSTCBCurPtr
    MOVW    R1, #:lower16:OSTCBHighRdyPtr                       @ OSTCBCurPtr = OSTCBHighRdyPtr;
    MOVT    R1, #:upper16:OSTCBHighRdyPtr
    LDR     R2, [R1]
    STR     R2, [R5]

    LDR     R0, [R2]                                            @ R0 is new process SP; SP = OSTCBHighRdyPtr->StkPtr;
    MSR     PSP, R0                                             @ Load PSP with new process SP

    MRS     R0, CONTROL
    ORR     R0, R0, #2
    MSR     CONTROL, R0
    ISB                                                         @ Sync instruction stream

    LDMFD    SP!, {R4-R11}                                      @ Restore r4-11 from new process stack
    LDMFD    SP!, {R0-R3}                                       @ Restore r0, r3
    LDMFD    SP!, {R12, LR}                                     @ Load R12 and LR
    LDMFD    SP!, {R1, R2}                                      @ Load PC and discard xPSR
    CPSIE    I
    BX       R1


@********************************************************************************************************
@                                       HANDLE PendSV EXCEPTION
@                                   void OS_CPU_PendSVHandler(void)
@
@ Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
@              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
@              processor context on any exception, and restores same on return from exception.  So only
@              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
@              this way means that context saving and restoring is identical whether it is initiated from
@              a thread or occurs due to an interrupt or exception.
@
@           2) Pseudo-code is:
@              a) Get the process SP, if 0 then skip (goto d) the saving part (first context switch);
@              b) Save remaining regs r4-r11 on process stack;
@              c) Save the process SP in its TCB, OSTCBCurPtr->OSTCBStkPtr = SP;
@              d) Call OSTaskSwHook();
@              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
@              f) Get current ready thread TCB, OSTCBCurPtr = OSTCBHighRdyPtr;
@              g) Get new process SP from TCB, SP = OSTCBHighRdyPtr->OSTCBStkPtr;
@              h) Restore R4-R11 from new process stack;
@              i) Perform exception return which will restore remaining context.
@
@           3) On entry into PendSV handler:
@              a) The following have been saved on the process stack (by processor):
@                 xPSR, PC, LR, R12, R0-R3
@              b) Processor mode is switched to Handler mode (from Thread mode)
@              c) Stack is Main stack (switched from Process stack)
@              d) OSTCBCurPtr      points to the OS_TCB of the task to suspend
@                 OSTCBHighRdyPtr  points to the OS_TCB of the task to resume
@
@           4) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
@              know that it will only be run when no other exception or interrupt is active, and
@              therefore safe to assume that context being switched out was using the process stack (PSP).
@********************************************************************************************************

.thumb_func
OS_CPU_PendSVHandler:
    CPSID   I                                                   @ Prevent interruption during context switch
    MRS     R0, PSP                                             @ PSP is process stack pointer
    STMFD   R0!, {R4-R11}                                       @ Save remaining regs r4-11 on process stack

    MOVW    R5, #:lower16:OSTCBCurPtr                           @ OSTCBCurPtr->OSTCBStkPtr = SP;
    MOVT    R5, #:upper16:OSTCBCurPtr
    LDR     R6, [R5]
    STR     R0, [R6]                                            @ R0 is SP of process being switched out

                                                                @ At this point, entire context of process has been saved
    MOV     R4, LR                                              @ Save LR exc_return value
    BL      OSTaskSwHook                                        @ OSTaskSwHook();

    MOVW    R0, #:lower16:OSPrioCur                             @ OSPrioCur   = OSPrioHighRdy;
    MOVT    R0, #:upper16:OSPrioCur
    MOVW    R1, #:lower16:OSPrioHighRdy
    MOVT    R1, #:upper16:OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    MOVW    R1, #:lower16:OSTCBHighRdyPtr                      @ OSTCBCurPtr = OSTCBHighRdyPtr;
    MOVT    R1, #:upper16:OSTCBHighRdyPtr
    LDR     R2, [R1]
    STR     R2, [R5]

    ORR     LR, R4, #0x04                                       @ Ensure exception return uses process stack
    LDR     R0, [R2]                                            @ R0 is new process SP; SP = OSTCBHighRdyPtr->StkPtr;
    LDMFD   R0!, {R4-R11}                                       @ Restore r4-11 from new process stack
    MSR     PSP, R0                                             @ Load PSP with new process SP
    CPSIE   I
    BX      LR                                                  @ Exception return will restore remaining context

.end
