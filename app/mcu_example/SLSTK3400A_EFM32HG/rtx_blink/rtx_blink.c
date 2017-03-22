/**************************************************************************//**
 * @file
 * @brief Simple LCD blink demo for SLSTK3400A-EFM32HG using CMSIS RTOS
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
#include "cmsis_os.h"
#include "em_chip.h"
#include "bsp.h"

/* Thread definition */
void ChangeLedThread(void const *argument);
osThreadDef(ChangeLedThread, osPriorityNormal, 1, 0);

/* Define message queue */
osMessageQDef(msgBox, 16, uint8_t);
osMessageQId msgBox;

/**************************************************************************//**
 * @brief
 *   Thread 1: Change LED thread
 *****************************************************************************/
void ChangeLedThread(void const *argument) {
  uint32_t leds;
  osEvent  evt;
  (void)argument;                 /* Unused parameter. */

  while (1)
  {
    /* Wait for message */
    evt = osMessageGet(msgBox, osWaitForever);
    if (evt.status == osEventMessage)
    {
      leds = (uint32_t)evt.value.p;
      BSP_LedsSet(leds);
    }
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
  uint32_t count = 0;

  /* Chip errata */
  CHIP_Init();

  /* Initialize LED driver */
  BSP_LedsInit();

  /* Initialize CMSIS RTOS structures */

  /* create msg queue */
  msgBox = osMessageCreate(osMessageQ(msgBox), NULL);
  /* create thread 1 */
  osThreadCreate(osThread(ChangeLedThread), NULL);

  /* Infinite loop */
  while (1)
  {
    count = (count+1)%4;

    /* Send message with LED status */
    osMessagePut(msgBox, count, osWaitForever);

    /* Wait now for half a second */
    osDelay(500);
  }
}
