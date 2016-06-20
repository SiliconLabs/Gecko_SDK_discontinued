/***************************************************************************//**
 * @file    iec60335_class_b_wdt_test.c
 * @brief   IEC60335 Class B c based WDT test
 *                POST and BIST interrupt test for all compiler
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2010 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
/*! @addtogroup IEC60335_WDOG_TEST
 * @{
 */

#include "iec60335_class_b_wdt_test.h"


#ifdef TESSY
/*! @cond TESSY */
uint32_t TS_TessyDummyWDOGCTRL(void);
uint32_t TS_TessyDummyWDOGCMD(void);
#define SyncWDOGCTRL()   (TS_TessyDummyWDOGCTRL())
#define SyncWDOGCMD()    (TS_TessyDummyWDOGCMD())
/*! @endcond */
#else
#define SyncWDOGCTRL()   (WDOG->SYNCBUSY & WDOG_SYNCBUSY_CTRL)
#define SyncWDOGCMD()    (WDOG->SYNCBUSY & WDOG_SYNCBUSY_CMD)
#endif


/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
testResult_t IEC60335_ClassB_initWDT(void)
{
  testResult_t result = IEC60335_testFailed;
  uint32_t     resetcause;
  /* check for core locked condition */
  resetcause = RMU->RSTCAUSE;
  if (!(RMU->CTRL & (1 << _RMU_CTRL_LOCKUPRDIS_SHIFT)))
  {
    /* check POR or Ext-reset reset as last reset event */
    if ((resetcause == RMU_RSTCAUSE_PORST)
        || ((resetcause & RMU_RSTCAUSE_EXTRST) && ((~resetcause) & 0x3)))
    {
      /* enable LE clock */
      CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_LE;
      /* clear flag */
      RMU->CMD     = RMU_CMD_RCCLR;
      EMU->AUXCTRL = EMU_AUXCTRL_HRCCLR;
      EMU->AUXCTRL = 0;
      /* init WDOG and start */
      while (SyncWDOGCTRL()) ;
      WDOG->CTRL = 0x80D;
      /* optional WDOG_CTRL_DEBUGRUN */
      result = IEC60335_testInProgress;
    }
    /* WDOG reset detected (second entry) */
    else if ((resetcause & RMU_RSTCAUSE_WDOGRST) && (~resetcause & 0x03))
    {
      /* test has been successful */
      result = IEC60335_testPassed;
      /* optional clear flag */
/*    RMU->CMD = RMU_CMD_RCCLR; */
    }
  }

  return result;
}

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC60335_ClassB_Refresh_WDT(void)
{
  /* If a previous clearing is being synchronized to LF domain, then there
   * is no point in waiting for it to complete before clearing over again.
   * This avoids stalling the core in the typical use case where some idle loop
   * keeps clearing the watchdog. */
  if (SyncWDOGCMD())
  {
    return;
  }

  WDOG->CMD = WDOG_CMD_CLEAR;
}

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
testResult_t IEC60335_ClassB_Force_WDT_Reset(void)
{
  testResult_t result = IEC60335_testFailed;
  /* no debugger allowed here */
  result = IEC60335_ClassB_initWDT();

  return result;
}

/*!
 * @}
 */

/************************************** EOF *********************************/
