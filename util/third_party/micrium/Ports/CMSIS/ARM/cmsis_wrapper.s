;********************************************************************************************************
;                                           ARM/Keil CMSIS Wrapper
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

    EXTERN  OS_CPU_PendSVHandler                                ; External references
    EXTERN  OS_CPU_SysTickHandler

    EXPORT  PendSV_Handler                                      ; Functions declared in this file
    EXPORT  SysTick_Handler

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;********************************************************************************************************
;                                      WRAPPER FUNCTIONS
;********************************************************************************************************

PendSV_Handler
    B       OS_CPU_PendSVHandler                                ; Jump to the generic PendSV handler

SysTick_Handler
    B       OS_CPU_SysTickHandler                               ; Jump to the generic SysTick handler

    END
