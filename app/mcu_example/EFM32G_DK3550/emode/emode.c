/**************************************************************************//**
 * @file
 * @brief Small demo for illustrating current in energy modes 0-4
 *
 * @note  EFM32G_DK3550
 *
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_rtc.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_lcd.h"
#include "em_chip.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"
#include "rtcdriver.h"

static int eMode = 0;           /* selected energy mode */
volatile uint32_t msTicks;      /* counts 1ms timeTicks */
int msCountDown;

/** Timer used for bringing the system back to EM0. */
static RTCDRV_TimerID_t xTimerForWakeUp;

/* Local prototypes */
static void GpioIrqInit(void);
static void DkIrqInit(void);
static void GpioDisablePins(void);
static void Delay(uint32_t dlyTicks);

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}


/**************************************************************************//**
 * @brief Clear and enable board controller interrupt
 *****************************************************************************/
static void DkIrqInit(void)
{
  /* Enable interrupts on joystick events only */
  BSP_InterruptDisable(0xffff);
  BSP_InterruptFlagsClear(0xffff);
  BSP_InterruptEnable(BC_INTEN_JOYSTICK);
}

/**************************************************************************//**
 * @brief Initialize GPIO interrupt on PC14
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint16_t flags, joystick;

  /* Get BSP interrupt flags. Clear GPIO interrupt first, then the BSP interrupts. */
  flags = BSP_InterruptFlagsGet();
  GPIO_IntClear(1 << BSP_GPIO_INT_PIN);
  BSP_InterruptFlagsClear(flags);

  /* Read joystick status */
  joystick = BSP_JoystickGet();

  if ( joystick == BC_UIF_JOYSTICK_UP )
  {
    if ( eMode > 0 ) eMode = eMode - 1;
  }
  if ( joystick == BC_UIF_JOYSTICK_DOWN )
  {
    if ( eMode < 6 ) eMode = eMode + 1;
  }

  /* Restart counter */
  msCountDown = 4000;

  /* Light up LEDs according to joystick status */
  BSP_LedsSet(joystick);
}


/**************************************************************************//**
 * @brief Initialize GPIO interrupt
 *****************************************************************************/
static void GpioIrqInit(void)
{
  /* Configure interrupt pin as input with pull-up */
  GPIO_PinModeSet(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, gpioModeInputPull, 1);

  /* Set falling edge interrupt and clear/enable it */
  GPIO_IntConfig(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}


/**************************************************************************//**
 * @brief Disable all GPIO pins
 *****************************************************************************/
static void GpioDisablePins(void)
{
  int i;

  GPIO_PinModeSet(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, gpioModeDisabled, 0);
  for(i=0; i<6; i++) {
    GPIO->P[i].DOUT = 0x00000000;
    GPIO->P[i].MODEH = 0x00000000;
    GPIO->P[i].MODEL = 0x00000000;
  }
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
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  const int msDelay=100;
  char displayString[8];
  LCD_AnimInit_TypeDef anim = {
    true,
    0x00,
    lcdAnimShiftNone,
    0x03,
    lcdAnimShiftLeft,
    lcdAnimLogicOr
  };
  LCD_FrameCountInit_TypeDef fc = {
    true,
    2, /* Update each 2nd frame */
    lcdFCPrescDiv1,
  };

  /* Chip errata */
  CHIP_Init();

  /* Configure push button interrupts */
  BSP_Init(BSP_INIT_DK_SPI);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize DK interrupt enable */
  DkIrqInit();

  /* Initialize GPIO interrupt */
  GpioIrqInit();

  /* Initialize RTC timer. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClockGet() / 1000)) while (1) ;

  /* Initialize LCD controller */
  SegmentLCD_Init(false);

  /* Run countdown for user to select energy mode */
  msCountDown = 4000; /* milliseconds */
  eMode = 0;
  while(msCountDown > 0)
  {
    if ( eMode >=3 && eMode<=4) {
      sprintf(displayString, "EM%d", eMode);
      SegmentLCD_Write(displayString);
    }
    switch( eMode )
    {
    case 0:
      SegmentLCD_Write("EM0 32M");
      break;
    case 1:
      SegmentLCD_Write("EM1 32M");
      break;
    case 2:
      SegmentLCD_Write("EM2 32K");
      break;
    case 3:
      SegmentLCD_Write("EM3    ");
      break;
    case 4:
      SegmentLCD_Write("EM4    ");
      break;
    case 5:
      SegmentLCD_Write("EM2+RTC");
      break;
    case 6:
    default:
      SegmentLCD_Write("RTC+LCD");
      break;
    }
    SegmentLCD_Number(msCountDown);
    Delay(msDelay);
    msCountDown -= msDelay;
  }
  /* Disable components, reenable when needed */
  SegmentLCD_Disable();
  RTC_Enable(false);

  GPIO->IEN = 0x00000000;
  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
  BSP_Disable();
  GpioDisablePins();

  /* Go to energy mode and wait for reset */
  switch(eMode)
  {
   case 0:
    /* Disable systick timer */
    SysTick->CTRL  = 0;

    /* 32Mhz primes demo - running off HFXO */
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    /* Disable HFRCO, LFRCO and all unwanted clocks */
    CMU->OSCENCMD = CMU_OSCENCMD_HFRCODIS;
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    /* Supress Conditional Branch Target Prefetch */
    MSC->READCTRL = MSC_READCTRL_MODE_WS1SCBTP;
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
  case 1:
    /* Disable systick timer */
    SysTick->CTRL  = 0;

    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    /* Disable HFRCO, LFRCO and all unwanted clocks */
    CMU->OSCENCMD = CMU_OSCENCMD_HFRCODIS;
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    EMU_EnterEM1();
    break;
  case 2:
    /* Enable LFRCO */
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;
    /* Disable everything else */
    CMU->OSCENCMD = CMU_OSCENCMD_LFXODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    EMU_EnterEM2(false);
    break;
  case 3:
    CMU->OSCENCMD = CMU_OSCENCMD_LFXODIS;
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    EMU_EnterEM3(false);
    break;
  case 4:
    EMU_EnterEM4();
    break;
  case 5:
    /* EM2 + RTC - only briefly wake up to reconfigure each second */
    RTC_Enable( true);
    while(1)
    {
      RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
      EMU_EnterEM2(false);
    }
  case 6:
    /* EM2 + RTC + LCD */
    SegmentLCD_Init(false);
    /* Animate LCD */
    LCD_FrameCountInit(&fc);
    LCD_AnimInit(&anim);
    RTC_Enable( true);
    while(1)
    {
      SegmentLCD_Write("Silicon");
      /* Sleep in EM2 */
      RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
      EMU_EnterEM2(false);

      SegmentLCD_Write(" Labs");
      /* Sleep in EM2 */
      RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
      EMU_EnterEM2(false);
    }
  case 7:
    break;
  }

  return 0;
}
