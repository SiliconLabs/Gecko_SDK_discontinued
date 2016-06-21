/***************************************************************************//**
 * @file    iec60335_class_b_interrupt_test.c
 * @brief   IEC60335 Class B c based interrupt test
 *             POST and BIST interrupt test for all compiler
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


#include "iec60335_class_b_interrupt_test.h"
/*! @addtogroup IEC60335_IRQ_TEST
 * @{
 */
/*! @cond RealView */
#if defined (__CC_ARM)                              /*--RealView Compiler --*/
    #define __ASM    __asm                          /*!< asm keyword for ARM Compiler */
/* ARM armcc specific function */
void __call_Vect(uint32_t vect)
{
  uint32_t *org_vector = (uint32_t *)vect;
  void (*Process)(void)= (void(*)(void))(*org_vector);
  Process();
}

#define __ENABLE_IRQ()     __enable_irq();          /*!< global Interrupt enable */
#define __DISABLE_IRQ()    __disable_irq()          /*!< global Interrupt disable */
/*! @endcond */

#elif (defined (__ICCARM__))                        /*-- ICC Compiler --*/
/*! @cond ICC */
    #define __ASM    __asm                          /*!< asm keyword for IAR Compiler */
/* IAR specific function */
void __call_Vect(uint32_t vect)
{
  uint32_t *org_vector = (uint32_t *)vect;
  void (*Process)(void) = (void(*)(void))(*org_vector);
  Process();
}

#define __ENABLE_IRQ     __enable_interrupt         /*!< global Interrupt enable */
#define __DISABLE_IRQ    __disable_interrupt        /*!< global Interrupt disable */
/*! @endcond */

#elif (defined (__GNUC__))                          /*-- GNU Compiler --*/
/*! @cond GCC */
    #define __ASM    __asm                          /*!< asm keyword for GNU Compiler */
/* GCC specific function */
void __call_Vect(uint32_t vect)
{
  uint32_t *org_vector = (uint32_t *)vect;
  void (*Process)(void) = (void(*)(void))(*org_vector);
  Process();
}

static __INLINE void __ENABLE_IRQ(void)
{
  __ASM volatile ("cpsie i");
}
static __INLINE void __DISABLE_IRQ(void)
{
  __ASM volatile ("cpsid i");
}
/*! @endcond */

#elif defined   (__TASKING__)
/*! @cond TASKING */
#error "not implemented yet !"                  /*!< TASKING Compiler is not implemented   */
/*! @endcond */
#else
#error "iec60355_class_b_interrupt_test.c: undefined compiler"
#endif

IEC60335_IRQ_Test_t IEC60335_IRQ_Test;
IEC60335_IRQ_Test_t *pIEC60335_IRQ_Test = 0;

/* replacement vector table */
#if defined (__ICCARM__)
#pragma data_alignment=0x100
uint32_t IRQVectorTable[64];
#else
uint32_t IRQVectorTable[64] __attribute__((aligned(0x100)));
#endif
/* test data array first element is SysTick_IRQn */
#ifdef TESSY
uint32_t IRQCounterTable[32];
#else
static uint32_t IRQCounterTable[32];
#endif

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
#pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC60335_IRQReplacementHandler(void)
{
  int32_t       IRQ_Idx;
  IRQTestData_t *IRQ_Test;
  /* get active interrupt number */
  IRQ_Idx = ((int32_t)(SCB->ICSR) & SCB_ICSR_VECTACTIVE_Msk) - 16;
  /* set counter, first element is SysTick_IRQn */
  if (IRQ_Idx > PendSV_IRQn)
  {
    IRQ_Test = (IRQTestData_t *) IRQCounterTable[IRQ_Idx+1];
    /* check test active */
    if (pIEC60335_IRQ_Test->Active & (0x1 << (IRQ_Idx+1)))
    {
      IRQ_Test->EffectiveCount++;
    }
  }
  /* call original vector */
  if ((IRQ_Idx <= PendSV_IRQn) || (pIEC60335_IRQ_Test->Mode & (0x1 << (IRQ_Idx+1))))
  {
    /* call original vector from flash */
    __call_Vect((IRQ_Idx + 16) * 4);
  }
}

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC60335_ClassB_InitInterruptTest(IRQn_Type IRQn, uint8_t Mode, IRQTestData_t *CountSetup)
{
  /* test only peripheral IRQs, first element is SysTick_IRQn */
  if (IRQn > PendSV_IRQn)
  {
    /* check initial usage */
    if (pIEC60335_IRQ_Test == 0)
    {
      __disable_irq();
      pIEC60335_IRQ_Test = &IEC60335_IRQ_Test;
      /* copy vector table */
      util_memcpy(&IRQVectorTable[0], 0, 64 * 4);
      pIEC60335_IRQ_Test->Active = 0x00;            /* clear all tests active */
      pIEC60335_IRQ_Test->Mode   = 0x00;            /* clear all flags */
      /* change offset to RAM */
      SCB->VTOR = ((0x1 << SCB_VTOR_TBLBASE_Pos) +
                   (((uint32_t) &IRQVectorTable[0]) & 0xFFFF));
      __enable_irq();
    }

    __disable_irq();
    /* toggle status active flag */
    if (pIEC60335_IRQ_Test->Active & (1 << (IRQn + 1)))
    {
      pIEC60335_IRQ_Test->Active &= ~(1 << (IRQn + 1));
      /* original vector */
      util_memcpy(&IRQVectorTable[IRQn + 16], (void *)(IRQn + 16), 4);
    }
    else       /* activate */
    {
      pIEC60335_IRQ_Test->Active |= (1 << (IRQn + 1));
      /* replace vector */
      IRQVectorTable[IRQn + 16] = (uint32_t) &IEC60335_IRQReplacementHandler;

      /* reset the counter */
      IRQCounterTable[IRQn + 1]  = (uint32_t) CountSetup;
      CountSetup->EffectiveCount = 0;

      /* toggle mode of calling original vectors */
      if (Mode)
      {
        pIEC60335_IRQ_Test->Mode |= (1 << (IRQn + 1));
      }
      else
      {
        pIEC60335_IRQ_Test->Mode &= ~(1 << (IRQn + 1));
      }
    }
    __enable_irq();
  }
  else
  {
    __disable_irq();
    /* disable all tested IRQs */
    IEC60335_IRQ_Test.Mode   = 0;
    IEC60335_IRQ_Test.Active = 0;
    pIEC60335_IRQ_Test         = (IEC60335_IRQ_Test_t *) 0;
    SCB->VTOR                  = 0;
    __enable_irq();
  }
}

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
testResult_t IEC60335_ClassB_InterruptCheck(IRQn_Type IRQn)
{
  testResult_t  Result = IEC60335_testFailed;
  IRQTestData_t *Counter;
  /* valid Index */
  if (IRQn > PendSV_IRQn)
  {
    /* check test active */
    if (pIEC60335_IRQ_Test->Active & (1 << (IRQn + 1)))
    {
      Counter = (IRQTestData_t *) IRQCounterTable[IRQn + 1];

      if ((Counter->EffectiveCount < Counter->MaxThres) && (Counter->EffectiveCount > Counter->MinThres))
      {
        Result                  = IEC60335_testPassed;
        Counter->EffectiveCount = 0;
      }
    }
  }
  return Result;
}

/*!
 * @}
 */

/************************************** EOF *********************************/
