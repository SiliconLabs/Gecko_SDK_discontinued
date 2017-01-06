/***************************************************************************//**
 * @file    iec60335_class_b_timer_rtc_test.c
 * @brief   IEC60335 Class B c based clock structure test
 *                POST and BIST clock structure test for all compiler
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

/*! @addtogroup IEC60335_TIMER_RTC_TEST
 * @{
 * @image html Timer-Test.jpg
 */

 #ifdef TESSY
/*! @cond TESSY */
uint32_t TS_TessyDummyRTC(void);
uint32_t TS_TessyDummyCMU(void);
uint32_t TS_TessyDummyIntermediate(unsigned long interm);
#define SyncRTC()    (TS_TessyDummyRTC())
#define SyncCMU()    (TS_TessyDummyCMU())
#define GetIRQRTC()  (TS_TessyDummyRTC())
/*! @endcond */
#else
#define SyncRTC()    (RTC->SYNCBUSY & 1)
#define SyncCMU()    (CMU->OSCENCMD & 0x80)
#define GetIRQRTC()  (RTC->IF & RTC_IF_COMP0)
#endif

ClockTest_t ClockTest;  /* RTC/Timer-test requires fixed data */

/*!
 * @brief    internal function to reset all test structures
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC60335_resetClockTest(uint32_t ratio, uint32_t tolerance)
{
  /* reset counter */
  if (ratio == 0)
  {
    ClockTest.timerCounter = 0;
    ClockTest.baseTicks    = 0;
    ClockTest.result       = IEC60335_testInProgress + 1;
  }
  else
  {
    ClockTest.Ratio        = ratio;
    ClockTest.Tolerance    = tolerance;
    ClockTest.timerCounter = 0;
    ClockTest.baseTicks    = 0;
  }
}

/*!
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC60335_ClassB_initClockTest(uint32_t ratio, uint32_t tolerance)
{
  /* setup parameter */
  IEC60335_resetClockTest(ratio, tolerance);

  /* start RTC */
  if (!RTC->CTRL & RTC_CTRL_EN)
  {
    /* enable LE clock */
    CMU->HFCORECLKEN0 |= 4;
    CMU->OSCENCMD      = 0x40;              /* enable osc LFXO */
    while (SyncCMU()) ;                     /* wait for stabilizing */
    CMU->LFCLKSEL |= CMU_LFCLKSEL_LFA_LFXO;
    /* enable RTC clock */
    CMU->LFACLKEN0 = 1;
    CMU->LFAPRESC0 = CMU_LFAPRESC0_RTC_DIV4096;
    while (SyncRTC()) ;
    RTC->IEN = 0;                           /* ints off */
    RTC->IFC = 7;
    /* set compare */
    while (SyncRTC()) ;
    RTC->COMP0 = 0x666;
    while (SyncRTC()) ;
    /* run RTC */
    RTC->CTRL |= RTC_CTRL_EN | RTC_CTRL_DEBUGRUN | RTC_CTRL_COMP0TOP;
  }
#if (USE_SYSTICK == 1)
/*! @cond USE_SYSTICK */

  /* Setup SysTick Timer for 10 msec interrupts  */
  /* set reload register */
  SysTick->LOAD = ((14000000 / 100) & SysTick_LOAD_RELOAD_Msk) - 1;
  /* set Priority for Cortex-M3 System Interrupts */
  SCB->SHP[((uint32_t)(SysTick_IRQn) & 0xF) - 4] = ((((1 << __NVIC_PRIO_BITS) - 1) << (8 - __NVIC_PRIO_BITS)) & 0xff);
  /* Load the SysTick Counter Value */
  SysTick->VAL = 0;
  /* Enable SysTick IRQ and SysTick Timer */
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

/*! @endcond */
#elif (USE_TIMER0)
/*! @cond USE_TIMER0 */
/* start timer0 */
/* Enable Timer Clock */
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER0;
  /* Configure TIMER from HFPERCLK */
  TIMER0->CTRL = TIMER_CTRL_CLKSEL_PRESCHFPERCLK | TIMER_CTRL_PRESC_DIV32 | \
                 TIMER_CTRL_FALLA_NONE | TIMER_CTRL_RISEA_NONE |            \
                 (0 << _TIMER_CTRL_DEBUGRUN_SHIFT) | TIMER_CTRL_MODE_UP;
  /* 32K ticks interrupts */
  TIMER0->TOP = 4 * 1024;
  /* Clear pending interrupt */
  NVIC->ICPR[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));

  TIMER0->IFC = TIMER_IEN_OF;
  TIMER0->IEN = TIMER_IEN_OF;
  /* enable interrupt */
  NVIC->ISER[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));
  TIMER0->CMD                                = TIMER_CMD_START;
/*! @endcond */
#elif (USE_TIMER1)
/*! @cond USE_TIMER1 */
/* start timer1 */
/* TODO : implement timer1 setup here */
/*! @endcond */
#endif
}

/*!
 * This function represents the part of the IEC60335 Class B clock test which has to be executed within the main loop.
 * It checks several thing:
 * @arg If the clock test timer interrupts are triggered
 * @arg If the clock test RTC interrupt is triggered
 * @arg or, If in any of the two interrupts an error was detected...
 *
 * This function must be called once inside the main loop.
 *
 * @attention For this function, it is necessary to estimate the count how often this function could be called. This is important to find
 * valid threshold values, which are used to test timer and RTC interrupt occurrence.
 *
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if (defined (__ICCARM__))
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
testResult_t IEC60335_ClassB_Clocktest_PollHandler(void)
{
  testResult_t result = IEC60335_testFailed;
  uint32_t     absolutUP, absolutLOW, intermediate;
  ClockTest.result = IEC60335_testInProgress;

  /* wait on RTC flag if running */
  if (RTC->CTRL & RTC_CTRL_EN)
  {
    while (0 == (GetIRQRTC())) ;
    RTC->IFC = RTC_IFC_COMP0;
    ClockTest.baseTicks++;
  }
  else     /* else quit here */
  {
    return(result);
  }
  /* calculate tolerance band extremes */
  intermediate = ClockTest.Ratio * ClockTest.Tolerance / 100;
#if TESSY
/*! @cond TESSY */
  intermediate = TS_TessyDummyIntermediate(intermediate);
/*! @endcond */
#endif
  if (intermediate > (0x80000000 - 1))
  {
    ClockTest.result = IEC60335_testFailed;
    return(result);
  }
  else
  {
    /* absolutLOW must not be negative */
    if (ClockTest.Ratio >= intermediate)
    {
      absolutLOW = ClockTest.Ratio - intermediate;
    }
    else
    {
      absolutLOW = 0;
    }
    absolutUP = intermediate + ClockTest.Ratio;
  }
  /* calculate average counter/baseTicks */
  intermediate = ClockTest.timerCounter / ClockTest.baseTicks;
  if ((intermediate > absolutLOW) && (intermediate < absolutUP))
  {
    result = IEC60335_testPassed;
    /* reset the counters */
    IEC60335_resetClockTest(0, 0);
    /* stop Interrupts */
#if (USE_SYSTICK == 1)
/*! @cond USE_SYSTICK */
    /* Disable SysTick IRQ and SysTick Timer */
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
/*! @endcond */
#elif (USE_TIMER0)
/*! @cond USE_TIMER0 */

    /* Silicon Labs fix, this is a read-only register ! */
    /* if (TIMER0->CMD != TIMER_CMD_START)              */
    /* {                                                */
    /*   ClockTest.result = IEC60335_testFailed;        */
    /*   return(IEC60335_testFailed);                   */
    /* }                                                */

    ClockTest.result = IEC60335_testPassed;
    /* disable interrupt */
    NVIC->ICER[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));
    /* Clear pending interrupt */
    NVIC->ICPR[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));
    TIMER0->IFC                                = TIMER_IEN_OF;
    TIMER0->IEN                               &= ~TIMER_IEN_OF;
/*! @endcond */
#elif (USE_TIMER1)
/*! @cond USE_TIMER1 */
/* TODO : implement timer1 stop here */
/*! @endcond */
#endif
  }

  return(result);
}

/*!
 * This function is intended to be called once inside the custom RTC interrupt service handler.
 * It can be used as service handler by itself. The result can be evaluated by checking the
 * counters values are reseted after this.
 *
 * This function tests the timer time frame and in this case the CPU frequency.
 * Also, this function checks if the main loop function was called.
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC60335_ClassB_Clocktest_RTCHandler(void)
{
  uint32_t absolutUP, absolutLOW, intermediate;
  ClockTest.result = IEC60335_testInProgress;
  /* TODO: call this vector if RTC is used with interrupt */
  /* check comp0 interrupt */
  if (RTC->IF & RTC_IF_COMP0)
  {
    ClockTest.baseTicks++;
    RTC->IFC = RTC_IFC_COMP0;               /* clear the interrupt flag */
    /* calculate tolerance band extremes */
    intermediate = ClockTest.Ratio * ClockTest.Tolerance / 100;

      /* absolutLOW must not be negative */
      if (ClockTest.Ratio >= intermediate)
      {
        absolutLOW = ClockTest.Ratio - intermediate;
      }
      else
      {
        absolutLOW = 0;
      }
      absolutUP = intermediate + ClockTest.Ratio;

      /* calculate average counter/baseTicks */
    intermediate = ClockTest.timerCounter / ClockTest.baseTicks;
      if ((intermediate > absolutLOW) && (intermediate < absolutUP))
      {
        /* reset the counters */
        IEC60335_resetClockTest(0, 0);
        /* stop Interrupts */
#if (USE_SYSTICK == 1)
/*! @cond USE_SYSTICK */
        /* Disable SysTick IRQ and SysTick Timer */
        SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
/*! @endcond */
#elif (USE_TIMER0)
/*! @cond USE_TIMER0 */
      if (TIMER0->CMD != TIMER_CMD_START)
      {
        ClockTest.result = IEC60335_testFailed;
        return;
      }
      ClockTest.result = IEC60335_testPassed;
      /* disable interrupt */
      NVIC->ICER[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));
      /* Clear pending interrupt */
      NVIC->ICPR[((uint32_t)(TIMER0_IRQn) >> 5)] = (1 << ((uint32_t)(TIMER0_IRQn) & 0x1F));
      TIMER0->IFC                                = TIMER_IEN_OF;
      TIMER0->IEN                               &= ~TIMER_IEN_OF;
/*! @endcond */
#elif (USE_TIMER1)
/*! @cond USE_TIMER1 */
/* TODO : implement timer1 stop here */
/*! @endcond */
#endif
    }
  }
}


/*!
 * @}
 */

/************************************** EOF *********************************/
