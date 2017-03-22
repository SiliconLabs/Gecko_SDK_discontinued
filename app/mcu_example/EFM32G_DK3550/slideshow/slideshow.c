/**************************************************************************//**
 * @file
 * @brief FAT example using FatFS for access to the MicroSD card on the DK.
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

#include <string.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "ff.h"
#include "microsd.h"
#include "diskio.h"
#include "tftamapped.h"
#include "slides.h"

/* counts 1ms timeTicks */
volatile uint32_t msTicks;

/* File system */
FATFS Fatfs;
char path[ 100 ];

/* Manifest file */
FIL manifest;

/* Local prototypes */
int initFatFS(void);
DWORD get_fattime(void);
void Delay(uint32_t dlyTicks);

/***************************************************************************//**
 * @brief
 *   Initialize MicroSD driver.
 * @return
 *   Returns 0 if initialization succeded, non-zero otherwise.
 ******************************************************************************/
int initFatFS(void)
{
  MICROSD_Init();
  if (f_mount(0, &Fatfs) != FR_OK)
    return -1;
  return 0;
}

/***************************************************************************//**
 * @brief
 *   This function is required by the FAT file system in order to provide
 *   timestamps for created files. Since we do not have a reliable clock we
 *   hardcode a value here.
 *
 *   Refer to drivers/fatfs/doc/en/fattime.html for the format of this DWORD.
 * @return
 *    A DWORD containing the current time and date as a packed datastructure.
 ******************************************************************************/
DWORD get_fattime(void)
{
  return (28 << 25) | (2 << 21) | (1 << 16);
}

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter.
 *****************************************************************************/
void SysTick_Handler(void)
{
  /* Increment counter necessary in Delay()*/
  msTicks++;
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
  bool     redraw, firstRun;
  FRESULT  res;
  int      mountStatus;
  /* Used when iterating through directory */
  FILINFO Finfo;
  DIR     dir;
  FRESULT listDirStatus = FR_OK;
  /* Used for manifest operation */
  bool useManifest = false;
  char buffer[20];

  /* Use 32MHZ HFXO as core clock frequency */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Setup SysTick Timer for 10 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 100))
  {
    while (1) ;
  }

  /* Enable SPI access to MicroSD card */
  BSP_PeripheralAccess(BSP_MICROSD, true);

  /* Initialize filesystem */
  mountStatus = initFatFS();

  /* Open Manifest file. If a manifest file is not present, iterate through the
   * File system in filesystem order. */
  if (f_open(&manifest, "files.txt", FA_READ) == FR_OK)
    useManifest = true;

  firstRun = true;
  /* Update TFT display forever */
  while (1)
  {
    if (!useManifest)
    {
      /* Open root directory */
      strcpy(path, "");
      listDirStatus = f_opendir(&dir, path);
    }
    /* Iterate through files */
    while (1)
    {
      /* Check if we should control TFT display instead of
       * AEM/board control application. Read state of AEM pushbutton */
      redraw = TFT_AddressMappedInit();
      if (redraw)
      {
        if ( firstRun )
        {
          firstRun = false;
          SLIDES_init();
        }

        /* Check disk status */
        if (disk_status(0) != 0)
        {
          /* Filesystem not mounted, show fatal error. */
          SLIDES_showError(true, "Fatal:\n  Filesystem is not ready.\n  (%d)", disk_status(0));
        }

        /* Check if filesystem was successfully mounted. */
        if (mountStatus != 0)
        {
          /* Filesystem not mounted, show fatal error. */
          SLIDES_showError(true, "Fatal:\n  Filesystem could not be mounted.\n  (%d)", mountStatus);
        }

        /* Is there a manifest file present? */
        if (useManifest)
        {
          /* If we are at the end of the file, reset filepointer */
          if (f_eof(&manifest))
            f_lseek(&manifest, 0);
          /* Read next file from manifest file */
          f_gets(buffer, 20, &manifest);
          /* Display Bitmap */
          SLIDES_showBMP(buffer);
        }
        else
        {
          /* Check to see if the root directory was correctly opened. */
          if (listDirStatus != FR_OK)
          {
            SLIDES_showError(true, "Fatal:\n  Could not read root directory.\n  (%d)", listDirStatus);
          }

          /* Open the next entry in directory */
          res = f_readdir(&dir, &Finfo);
          if ((res != FR_OK) || !Finfo.fname[0])
          {
            /* End of directory listing. Break out of inner loop and reopen
             * directory entry */
            break;
          }
          /* Update display */
          SLIDES_showBMP(Finfo.fname);
        }

        /* Delay to allow the user to see the BMP file. */
        Delay(200);
      }
    }
  }
}
