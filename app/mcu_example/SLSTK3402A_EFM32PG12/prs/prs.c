/**************************************************************************//**
 * @file
 * @brief PRS Example for SLSTK3402A
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "em_pcnt.h"
#include "em_prs.h"
#include "bsp.h"

#include <stdint.h>
#include <stdbool.h>

static void prsToggle(void);

void PCNT0_IRQHandler(void)
{
  prsToggle();
  PCNT_IntClear(PCNT0, PCNT_IFC_OF);
}

static void setupLetimer(void)
{
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;
  letimerInit.ufoa0 = letimerUFOAToggle;        /* Toggle outout on underflow */
  letimerInit.comp0Top = true;                  /* Reload CNT from TOP on underflow */

  CMU_ClockEnable(cmuClock_LETIMER0, true);

  LETIMER_Init(LETIMER0, &letimerInit);
  LETIMER_CompareSet(LETIMER0, 0, 32768 / 4);   /* Toggle every 250ms */
  LETIMER_RepeatSet(LETIMER0, 0, 1);            /* Set REP0 to a non-zero value to generate output */
  LETIMER0->ROUTEPEN = LETIMER_ROUTEPEN_OUT0PEN;
  LETIMER_Enable(LETIMER0, true);
}

static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;
  PCNT_Filter_TypeDef pcntFilterInit = PCNT_FILTER_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode     = pcntModeOvsSingle;
  pcntInit.top      = 4;                        /* Interrupt at every 5 BTN0 press */
  pcntInit.s1CntDir = false;                    /* Count up */
  pcntInit.s0PRS    = pcntPRSCh2;
  pcntInit.filter   = true;                     /* Filter GPIO to remove glitching LED behavior */

  /* Use max filter len for GPIO push button */
  pcntFilterInit.filtLen = _PCNT_OVSCFG_FILTLEN_MASK;

  /* Enable the PRS Input */
  PCNT_Init(PCNT0, &pcntInit);
  PCNT_FilterConfiguration(PCNT0, &pcntFilterInit, true);
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);
  PCNT_IntEnable(PCNT0, PCNT_IEN_OF);
}

static void setupPrs(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_LETIMER0, PRS_CH_CTRL_SIGSEL_LETIMER0CH0);
  PRS_SourceAsyncSignalSet(2, PRS_CH_CTRL_SOURCESEL_GPIOL, PRS_CH_CTRL_SIGSEL_GPIOPIN6);

  /* PRS Channel 0 -> Location 5 (PF5 on the SLSTK3402A) - LED1
     PRS Channel 1 -> Location 3 (PF4 on the SLSTK3402A) - LED0 */
  PRS->ROUTELOC0 = PRS_ROUTELOC0_CH0LOC_LOC5
                 | PRS_ROUTELOC0_CH1LOC_LOC3;
  PRS->ROUTEPEN = PRS_ROUTEPEN_CH0PEN | PRS_ROUTEPEN_CH1PEN;
}

static void setupNvic(void)
{
  NVIC_ClearPendingIRQ(PCNT0_IRQn);
  NVIC_EnableIRQ(PCNT0_IRQn);
}

static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  BSP_LedsInit();

  /* Configure pin I/O - BTN0 on PF6 */
  GPIO_PinModeSet(gpioPortF, 6, gpioModeInput, 1);
  GPIO_ExtIntConfig(gpioPortF, 6, 6, false, false, false);
}

static void prsToggle(void)
{
  PRS->SWLEVEL ^= 0x2;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);

  /* Use LFRCO as LFA clock for LETIMER and PCNT */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_CORELE, true);

  setupGpio();
  setupPrs();
  setupLetimer();
  setupPcnt();
  setupNvic();

  /* Enter EM2 forever */
  while(true)
  {
    EMU_EnterEM2(false);
  }
}
