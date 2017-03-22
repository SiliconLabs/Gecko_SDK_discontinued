/**************************************************************************//**
 * @file
 * @brief RTX tick-less mode demo for EFM32ZG_STK3200 using CMSIS RTOS
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

#include "cmsis_os.h"
#include "em_chip.h"


/* Thread definition */
void DummyTask(void const *arg);
osThreadDef(DummyTask, osPriorityNormal, 1, 0);

/**************************************************************************//**
 * @brief Dummy task that is going sleep for long time
 *****************************************************************************/
 void DummyTask(void const *arg)
{
  (void) arg;
  while (1)
  {
    osDelay(5000);
  }
}

/**************************************************************************//**
 * @brief
 *   Main function is a CMSIS RTOS thread in itself
 *
 * @note
 *   This example uses threads and message queue to demonstrate the usage of
 *   these CMSIS RTOS features. In this simple example, the same functionality
 *   could more easily be achieved by doing everything in the main loop.
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Initialize CMSIS RTOS structures */

  osThreadCreate(osThread(DummyTask), NULL);

  /* Infinite loop */
  while (1)
  {
    osDelay(500);
  }
}
