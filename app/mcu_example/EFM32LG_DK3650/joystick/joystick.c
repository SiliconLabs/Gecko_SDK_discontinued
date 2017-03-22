/**************************************************************************//**
 * @file
 * @brief Joystick interrupt example for EFM32LG_DK3650
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"

volatile uint32_t msTicks; /* counts 1ms timeTicks */

/* Local prototypes */
void Delay(uint32_t dlyTicks);
void DK_IRQInit(void);
void GPIO_IRQInit(void);

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
 * @brief Handle joystick interrupt
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint16_t data, joystick;

  /* Get BSP interrupt flags. Clear GPIO interrupt first, then the BSP interrupts. */
  data = BSP_InterruptFlagsGet();
  GPIO_IntClear(1 << BSP_GPIO_INT_PIN);
  BSP_InterruptFlagsClear(data);

  /* Read joystick status */
  joystick = BSP_JoystickGet();

  /* Light up LEDs according to joystick status */
  BSP_LedsSet(joystick);
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
 * @brief Clear and enable board controller interrupt
 *****************************************************************************/
void DK_IRQInit(void)
{
  /* Enable interrupts on joystick events only */
  BSP_InterruptDisable(0xffff);
  BSP_InterruptFlagsClear(0xffff);
  BSP_InterruptEnable(BC_INTEN_JOYSTICK);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
  {
    while (1) ;
  }

  /* Initialize DK interrupt enable */
  DK_IRQInit();

  /* Initialize GPIO interrupt */
  GPIO_IRQInit();

  /* Turn off LEDs */
  BSP_LedsSet(0x0000);

  while (1)
  {
    /* Wait 5 seconds */
    Delay(5000);
    /* Quick flash to show we're alive */
    BSP_LedsSet(0xffff);
    Delay(20);
    /* Restore leds according to joystick status. */
    BSP_LedsSet(BSP_JoystickGet());
  }
}
