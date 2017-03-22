@********************************************************************************************************
@                                           GCC CMSIS Wrapper
@********************************************************************************************************

@********************************************************************************************************
@                                           PUBLIC FUNCTIONS
@********************************************************************************************************

    .extern  OS_CPU_PendSVHandler                                @ External references
    .extern  OS_CPU_SysTickHandler

    .global  PendSV_Handler                                      @ Functions declared in this file
    .global  SysTick_Handler

@********************************************************************************************************
@                                      CODE GENERATION DIRECTIVES
@********************************************************************************************************

.text
.align 2
.thumb
.syntax unified

@********************************************************************************************************
@                                          WRAPPER FUNCTIONS
@********************************************************************************************************

.thumb_func
PendSV_Handler:
    B       OS_CPU_PendSVHandler                                @ Jump to the generic PendSV handler

.thumb_func
SysTick_Handler:
    B       OS_CPU_SysTickHandler                               @ Jump to the generic SysTick handler

.end
