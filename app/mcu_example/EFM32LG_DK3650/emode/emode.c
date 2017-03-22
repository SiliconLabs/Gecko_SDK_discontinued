/**************************************************************************//**
 * @file
 * @brief Energy Mode example for EFM32LG-DK3650 development kit
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

#include <stdio.h>
#include <string.h>
#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"
#include "bsp_trace.h"
/* Address Mapped TFT mode initialization code */
#include "tftamapped.h"
#include "rtcdriver.h"
/* Graphics library */
#include "glib/glib.h"

/** Counts 1ms timeTicks */
static volatile uint32_t msTicks;

/** Graphics context */
static GLIB_Context_t    gc;

/** Selected Energy Mode for demo */
static int eModeDemo = 0;
static bool runDemo = false;

/** Available demos */
static char *eModeDesc[] =
{
  "Energy Mode 0 - 48MHz / Primes",
  "    Energy Mode 1 - 48MHz     ",
  "Energy Mode 0 - 14MHz / Primes",
  "    Energy Mode 1 - 14MHz     ",
  "    Energy Mode 2 - LFRCO     ",
  "  Energy Mode 2 - LFRCO/RTC   ",
  "       Energy Mode 3          ",
  " Energy Mode 3 - GPIO wake up ",
  "       Energy Mode 4          ",
};

/** Usage instructions */
static char *description[] =
{"\
Use joystick Up and Down to select\n\
various energy mode demos.\n\n\
Press PB1 to activate demo.\n\n\
After activition, press AEM button to\n\
go back to board control and AEM screen\n\
The EFM32LG display will not be\n\
refreshed until restart.",
"\
Reset MCU to try other demos.\n\
Make sure debugger is disconnected."};

/** Timer used for bringing the system back to EM0. */
RTCDRV_TimerID_t xTimerForWakeUp;


/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}


/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}


/**************************************************************************//**
 * @brief Initialize GPIO interrupt
 *****************************************************************************/
void GPIO_IRQInit(void)
{
  /* Configure interrupt pin as input with pull-up */
  GPIO_PinModeSet(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, gpioModeInputPull, 1);

  /* Set falling edge interrupt and clear/enable it */
  GPIO_IntConfig(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}


/**************************************************************************//**
 * @brief Initialize GPIO interrupt
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint16_t flags, joystick, pb;

  /* Get BSP interrupt flags. Clear GPIO interrupt first, then the BSP interrupts. */
  flags = BSP_InterruptFlagsGet();
  GPIO_IntClear(1 << BSP_GPIO_INT_PIN);
  BSP_InterruptFlagsClear(flags);

  /* Move selection */
  if(flags & BC_INTEN_JOYSTICK)
  {
    /* Read joystick status */
    joystick = BSP_JoystickGet();

    /* Move selection around */
    if(joystick == BC_UIF_JOYSTICK_UP)
    {
      if(eModeDemo>0) eModeDemo=eModeDemo-1;
    }
    if(joystick == BC_UIF_JOYSTICK_DOWN)
    {
      if(eModeDemo<8) eModeDemo=eModeDemo+1;
    }
  }

  /* Activate demo */
  if(flags & BC_INTEN_PB)
  {
    pb = BSP_PushButtonsGet();
    if(pb == BC_UIF_PB1)
    {
      runDemo = true;
    }
    /* Wait until key is released */
    while(BSP_PushButtonsGet()!=0) ;
  }
}


/**************************************************************************//**
 * @brief Clear and enable board controller interrupts
 *****************************************************************************/
void DK_IRQInit(void)
{
  /* Enable interrupts on joystick events only */
  BSP_InterruptDisable(0xffff);
  BSP_InterruptFlagsClear(0xffff);
  BSP_InterruptEnable(BC_INTEN_JOYSTICK | BC_INTEN_PB);
}


/**************************************************************************//**
 * @brief Run demo calculating prime numbers at given clock frequency
 * @param[in] clock Select oscillator to use
 * @param[in] HFRCO band
 *****************************************************************************/
void Demo_Primes(CMU_Select_TypeDef clock, CMU_HFRCOBand_TypeDef band)
{
  /* Set HF clock  */
  CMU_ClockSelectSet(cmuClock_HF, clock);

  /* If HFRCO, select band */
  if(clock == cmuSelect_HFRCO)
  {
    CMU_HFRCOBandSet(band);
  }

  /* Disable HFRCO, LFRCO and all unwanted clocks */
  if(clock != cmuSelect_HFXO)
  {
    CMU->OSCENCMD = CMU_OSCENCMD_HFXODIS;
  }
  if(clock != cmuSelect_HFRCO)
  {
    CMU->OSCENCMD = CMU_OSCENCMD_HFRCODIS;
  }
  if(clock != cmuSelect_LFRCO)
  {
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCODIS;
  }
  if(clock != cmuSelect_LFXO)
  {
    CMU->OSCENCMD = CMU_OSCENCMD_LFXODIS;
  }

  /* Disable peripheral clocks */
  CMU->HFPERCLKEN0  = 0x00000000;
  CMU->HFCORECLKEN0 = 0x00000000;
  CMU->LFACLKEN0    = 0x00000000;
  CMU->LFBCLKEN0    = 0x00000000;
  {
#define PRIM_NUMS 64
    uint32_t i, d, n;
    uint32_t primes[PRIM_NUMS];

    /* Find prime numbers forever */
    while (1)
    {
      primes[0] = 1;
      for (i = 1; i < PRIM_NUMS;)
      {
        for (n = primes[i - 1] + 1; ;n++)
        {
          for (d = 2; d <= n; d++)
          {
            if (n == d)
            {
              primes[i] = n;
              goto nexti;
            }
            if (n%d == 0) break;
          }
        }
      nexti:
        i++;
      }
    }
  }
}


/**************************************************************************//**
 * @brief Energy Mode 1 demonstration, no active peripherals
 * @param[in] clock Select oscillator to use
 * @param[in] HFRCO band
 *****************************************************************************/
void Demo_EM1(CMU_Select_TypeDef clock, CMU_HFRCOBand_TypeDef band)
{
  /* Set HF clock  */
  CMU_ClockSelectSet(cmuClock_HF, clock);

  /* If HFRCO, select band */
  if(clock == cmuSelect_HFRCO)
  {
    CMU_HFRCOBandSet(band);
  }

  /* Disable HFRCO, LFRCO and all unwanted clocks */
  if(clock != cmuSelect_HFXO)
  {
    CMU->OSCENCMD = CMU_OSCENCMD_HFXODIS;
  }
  if(clock != cmuSelect_HFRCO)
  {
    CMU->OSCENCMD = CMU_OSCENCMD_HFRCODIS;
  }
  if(clock != cmuSelect_LFRCO)
  {
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCODIS;
  }
  if(clock != cmuSelect_LFXO)
  {
    CMU->OSCENCMD = CMU_OSCENCMD_LFXODIS;
  }
  CMU->HFPERCLKEN0  = 0x00000000;
  CMU->HFCORECLKEN0 = 0x00000000;
  CMU->LFACLKEN0    = 0x00000000;
  CMU->LFBCLKEN0    = 0x00000000;

  /* Enter Energy Mode 1, no active peripherals */
  EMU_EnterEM1();
}


/**************************************************************************//**
 * @brief Energy Mode 2 demonstration, no active perpherals
 *****************************************************************************/
void Demo_EM2(void)
{
  /* Disable DK interrupts */
  BSP_InterruptDisable(0xffff);

  /* Enable LFRCO */
  CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;

  /* Disable all peripheral clocks */
  CMU->HFPERCLKEN0  = 0x00000000;
  CMU->HFCORECLKEN0 = 0x00000000;
  CMU->LFACLKEN0    = 0x00000000;
  CMU->LFBCLKEN0    = 0x00000000;

  /* Enter Energy Mode 2 - this will never wake up */
  EMU_EnterEM2(false);
  while(1) BSP_LedsSet(0xffff);
}


/**************************************************************************//**
 * @brief Energy Mode 2 demonstration, with RTC wake up every 2nd second
 *****************************************************************************/
void Demo_EM2_RTC(void)
{
  /* Disable DK interrupts */
  BSP_InterruptDisable(0xffff);

  /* Disable unused peripheral clocks */
  CMU->HFPERCLKEN0  = 0x00000000;
  CMU->LFBCLKEN0    = 0x00000000;

  /* Enter Energy Mode 2 - this will never wake up */
  while(1)
  {
    /* Wake up every 2nd second */
    RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
    EMU_EnterEM2( false);
  }
}


/**************************************************************************//**
 * @brief Energy Mode 3 demonstration, no active peripherals
 *****************************************************************************/
void Demo_EM3(void)
{
  /* Disable DK interrupts */
  BSP_InterruptDisable(0xffff);

  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);

  /* Disable peripheral clocks */
  CMU->HFPERCLKEN0  = 0x00000000;
  CMU->HFCORECLKEN0 = 0x00000000;
  CMU->LFACLKEN0    = 0x00000000;
  CMU->LFBCLKEN0    = 0x00000000;


  /* Enter Energy Mode 3 - never wake up */
  EMU_EnterEM3(false);
  while(1) BSP_LedsSet(0xffff);
}


/**************************************************************************//**
 * @brief Energy Mode 3 demonstration, GPIO wake up - will restart application
 *****************************************************************************/
void Demo_EM3_GPIO(void)
{
  /* Keep GPIO and EBI active, do not disable peripherals */

  /* Disable LF peripherals */
  CMU->LFACLKEN0 = 0x00000000;
  CMU->LFBCLKEN0 = 0x00000000;

  /* Enter Energy Mode 3 - wake up on GPIO interrupt */
  /* Press AEM button again to read EFM, and then any PB/Joystick button */
  EMU_EnterEM3(false);

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClockGet() / 1000)) while (1) ;

  /* Show LED pattern after wake up, to show we're alive */
  while(1)
  {
    BSP_LedsSet(0xf00f);
    Delay(200);
    BSP_LedsSet(0x0ff0);
    Delay(200);
  }
}


/**************************************************************************//**
 * @brief Energy Mode 4 demonstration
 *****************************************************************************/
void Demo_EM4(void)
{
  EMU_EnterEM4();
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  bool     redraw = false;
  bool     prevRedraw = false;
  EMSTATUS status;
  volatile int i;

  /* Initialize DK board register access */
  /* This demo currently only works in EBI mode */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize DK interrupt enable */
  DK_IRQInit();

  /* Initialize GPIO interrupt */
  GPIO_IRQInit();

  /* Misc timers. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  /* Clear LEDs */
  BSP_LedsSet(0x0000);

  /* Wait until we have control over display */
  while(!redraw)
  {
    redraw = TFT_AddressMappedInit();
  }

  /* Init graphics context - abort on failure */
  status = GLIB_contextInit(&gc);
  if (status != GLIB_OK) while (1) ;

  /* Update TFT display forever */
  while (1)
  {
    /* Check if we should control TFT display instead of AEM/board controller */
    redraw = TFT_AddressMappedInit();
    if(redraw)
    {
      /* This indicated a BC -> EFM control transfer */
      if(prevRedraw != redraw)
      {
        /* Update information message */
        GLIB_setFont(&gc, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
        gc.foregroundColor = GLIB_rgbColor(200, 200, 200);
        gc.backgroundColor = GLIB_rgbColor(0, 0, 0);
        GLIB_drawString(&gc, description[0], strlen(description[0]), 20, 10, 1);
        gc.foregroundColor = GLIB_rgbColor(200, 200, 100);
        gc.font.charSpacing = 2;
        gc.font.lineSpacing = 4;
        GLIB_drawString(&gc, description[1], strlen(description[1]), 20, 110, 1);
      }
      /* Update selected demo  */
      GLIB_setFont(&gc, (GLIB_Font_t *)&GLIB_FontNormal8x8);
      gc.foregroundColor = GLIB_rgbColor(100, 200, 100);
      gc.backgroundColor = GLIB_rgbColor(50, 50, 50);
      GLIB_drawString(&gc, eModeDesc[eModeDemo], strlen(eModeDesc[eModeDemo]), 20, 175, 1);
    }
    else
    {
      /* No need to refresh display when BC is active */
    }
    prevRedraw = redraw;
    if(runDemo)
    {
      GLIB_setFont(&gc, (GLIB_Font_t *)&GLIB_FontNormal8x8);
      gc.foregroundColor = GLIB_rgbColor(50, 50, 50);
      gc.backgroundColor = GLIB_rgbColor(100, 200, 100);
      GLIB_drawString(&gc, eModeDesc[eModeDemo], strlen(eModeDesc[eModeDemo]), 20, 175, 1);
      for(i=0; i<14000; i++) ;
      break;
    }
  }

  /* Run demo */
  switch(eModeDemo)
  {
  case 0:
    Demo_Primes(cmuSelect_HFXO, (CMU_HFRCOBand_TypeDef) 0);
    break;
  case 1:
    Demo_EM1(cmuSelect_HFXO, (CMU_HFRCOBand_TypeDef) 0);
    break;
  case 2:
    Demo_Primes(cmuSelect_HFRCO, cmuHFRCOBand_14MHz);
    break;
  case 3:
    Demo_EM1(cmuSelect_HFRCO, cmuHFRCOBand_14MHz);
    break;
  case 4:
    Demo_EM2();
    break;
  case 5:
    Demo_EM2_RTC();
    break;
  case 6:
    Demo_EM3();
    break;
  case 7:
    Demo_EM3_GPIO();
    break;
  case 8:
    Demo_EM4();
    break;
  default:
    while(1);
  }

  return 0;
}
