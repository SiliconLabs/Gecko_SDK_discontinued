/**************************************************************************//**
 * @file
 * @brief FreeRTOS Tickless:  Silicon Labs EFM32ZG_STK3200 Starter Kit

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


#include "FreeRTOS.h"
#include "task.h"

#include "em_device.h"
#include "em_chip.h"

#include "display.h"
#include "displaypal.h"
#include "image.h"

#include "sleep.h"

#define BYTES_PER_LINE        ( LS013B7DH03_WIDTH / 8 )
#define BYTES_PER_FRAME       ( LS013B7DH03_HEIGHT * BYTES_PER_LINE )

#define STACK_SIZE_FOR_TASK   ( configMINIMAL_STACK_SIZE + 10 )
#define TASK_PRIORITY         ( tskIDLE_PRIORITY + 1 )
#define DELAY_PRIORITY        ( tskIDLE_PRIORITY + 2 )

/* Variables used to display pictures on display */
static int numImages = sizeof (image_bits) / BYTES_PER_FRAME;
static int newImage, oldImage  = 0;
DISPLAY_Device_t displayDevice;

/* Repeat callback variables. */
static void (*repeatCallback)(void*)  = 0;
static void*  repeatCallbackParameter = 0;
static int    repeatCallbackFrequency = 1; /* Callback frequency is 1Hz per
                                              default. */

/**************************************************************************//**
 * @brief MemlcdScrollLeft function which is scrolling images on the display
 * @param *pOldImg pointer to old image which is going out, *pNewImg pointer
 * to image which appears on display
 *****************************************************************************/
static void MemlcdScrollLeft(const char *pOldImg, const char *pNewImg)
{
  int step, x, y;
  char line[BYTES_PER_LINE];
  const char *pNewLine, *pOldLine;

  /* Iterate over all the steps */
  for (step = 0; step <= BYTES_PER_LINE; step++)
  {
    /* Iterate over each line */
    for (y = 0; y < LS013B7DH03_HEIGHT; y++)
    {
      pNewLine = &pNewImg[BYTES_PER_LINE * y];
      pOldLine = &pOldImg[BYTES_PER_LINE * y];

      /* Iterate over each byte of the line */
      for (x = 0; x < BYTES_PER_LINE; x++)
      {
        if (x < (BYTES_PER_LINE - step))
        {
          line[x] = pOldLine[x + step];
        }
        else
        {
          line[x] = pNewLine[x - (BYTES_PER_LINE - step)];
        }
      }
      displayDevice.pPixelMatrixDraw( &displayDevice,
                                      line,
                                      /* start coloumn, width */
                                      0, displayDevice.geometry.width,
                                      /* start row, height */
                                      y, 1 );
    }
  }
}


/**************************************************************************//**
 * @brief DrawPicture task which is showing new image on the display
 * @param *pParameters pointer to parameters passed to the function
 *****************************************************************************/
static void DrawPicture(void *pParameters)
{
  (void)pParameters;
  for (;;)
  {
    /* Delay between changing pictures */
    vTaskDelay(pdMS_TO_TICKS(3000));

    /* Output new image on Memory LCD */
    oldImage = newImage;

    if (++newImage >= numImages)
    {
      newImage = 0;
    }
    MemlcdScrollLeft(&image_bits[oldImage*BYTES_PER_FRAME], &image_bits[newImage*BYTES_PER_FRAME]);
  }
}

/**************************************************************************//**
 * @brief   Register a callback function to be called repeatedly at the
 *          specified frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] pParameter Pointer argument to be passed to the callback function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency is not supported.
 *****************************************************************************/
int RepeatCallbackRegister (void(*pFunction)(void*),
                            void* pParameter,
                            unsigned int frequency)
{
  /* Check that the specified frequency is not faster than the tick rate. */
  if (frequency > 1000/portTICK_RATE_MS)
    return -1;

  repeatCallback          = pFunction;
  repeatCallbackParameter = pParameter;
  repeatCallbackFrequency = frequency;

  return 0;
}

/**************************************************************************//**
 * @brief   Task that calls callback repeatedly at the specified frequency.
 * @details The intention of this function is to support the Sharp Memory
 *          LCD device driver which needs to toggle the polarity inversion of
 *          the liquid crystal display cells every second.
 * @param   *pParameters pointer to parameters passed to the function
 *****************************************************************************/
static void RepeatCallbackTask(void *pParameters)
{
  (void)pParameters;

  for (;;)
  {
    /* Execute callback function if registered. */
    if (repeatCallback)
    {
      (*repeatCallback)(repeatCallbackParameter);
    }
    vTaskDelay(pdMS_TO_TICKS(1000)/repeatCallbackFrequency);
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  char *pFrame;

  /* Chip errata */
  CHIP_Init();

  /* Initialize SLEEP driver, no calbacks are used */
  SLEEP_Init(NULL, NULL);
#if (configSLEEP_MODE < 3)
  /* do not let to sleep deeper than define */
  SLEEP_SleepBlockBegin((SLEEP_EnergyMode_t)(configSLEEP_MODE+1));
#endif

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retrieve the properties of the display. */
  if ( DISPLAY_DeviceGet(0, &displayDevice) != DISPLAY_EMSTATUS_OK)
    while(1);

  /* Load pointer to picture buffor */
  pFrame= (char *) &image_bits[LS013B7DH03_WIDTH * BYTES_PER_LINE * newImage];

  /* Write to LCD */
  displayDevice.pPixelMatrixDraw( &displayDevice, pFrame,
                                  /* start coloumn, width */
                                  0, displayDevice.geometry.width,
                                  /* start row, height */
                                  0, displayDevice.geometry.height);


  /* Create task to show pictures */
  xTaskCreate(DrawPicture, (const char *) "DrawPicture", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  /* Create task to toggle the polarity inversion of the Sharp memory LCD.  */
  xTaskCreate(RepeatCallbackTask, (const char *) "RepeatToggle", STACK_SIZE_FOR_TASK, NULL, DELAY_PRIORITY, NULL);

  /* Start FreeRTOS Scheduler */
  vTaskStartScheduler();

  return 0;
}
