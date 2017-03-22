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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "em_device.h"
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include "ff.h"
#include "microsd.h"
#include "diskio.h"

/* File system */
FATFS Fatfs;
FIL   fh;
char path[ 100 ];

/* Command buffer and read data buffer */
#define CBUFSIZE    80
static char buffer[CBUFSIZE];
static UINT bufRead;
static char commandLine[CBUFSIZE];
static int  commandIndex = 0;

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
 * @brief Output an array of characters
 * @param buf Pointer to string buffer
 * @param length Number of characters to output
 *****************************************************************************/
void PrintBuf(char *buf, int length)
{
  while (length--) putchar(*buf++);
}


/**************************************************************************//**
 * @brief scan_files from FatFS documentation
 * @param path to traverse
 *****************************************************************************/
FRESULT scan_files(char* path, bool loption)
{
  FRESULT     res;
  FILINFO     fno;
  DIR         dir;
  int         i;
  char        *fn;
#if _USE_LFN
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

  res = f_opendir(&dir, path);
  if (res == FR_OK)
  {
    i = strlen(path);
    for (;;)
    {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0)
      {
        /* printf("f_readdir failure %d\n", res); */
        break;
      }
      if (fno.fname[0] == '.') continue;
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
      if (fno.fattrib & AM_DIR)
      {
        if(loption)
        {
          char attrib[7];

          attrib[0]='\0';
          if(fno.fattrib & AM_RDO) strcat(attrib,"R");
          if(fno.fattrib & AM_HID) strcat(attrib,"H");
          if(fno.fattrib & AM_SYS) strcat(attrib,"S");
          if(fno.fattrib & AM_LFN) strcat(attrib,"L");
          if(fno.fattrib & AM_DIR) strcat(attrib,"D");
          if(fno.fattrib & AM_ARC) strcat(attrib,"A");
          printf("%10u %s ", (unsigned int) fno.fsize, attrib);
        }
        if(strlen(path))
        {
          printf("%s/", path);
        }
        printf("%s\n", fn);
        sprintf(&path[i], "/%s", fn);
        res = scan_files(path, loption);
        if (res != FR_OK) break;
        path[i] = 0;
      }
      else
      {
        if(loption)
        {
          char attrib[7];

          attrib[0]='\0';
          if(fno.fattrib & AM_RDO) strcat(attrib,"R");
          if(fno.fattrib & AM_HID) strcat(attrib,"H");
          if(fno.fattrib & AM_SYS) strcat(attrib,"S");
          if(fno.fattrib & AM_LFN) strcat(attrib,"L");
          if(fno.fattrib & AM_DIR) strcat(attrib,"D");
          if(fno.fattrib & AM_ARC) strcat(attrib,"A");
          printf("%10u %s ", (unsigned int) fno.fsize, attrib);
        }
        if(strlen(path))
        {
          printf("%s/", path);
        }
        printf("%s\n", fn);
      }
    }
  }
  else
  {
    printf("f_opendir failure %d\n", res);
  }

  return res;
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  FRESULT res;
  int     c;
  char    *command;

  /* Use 32MHZ HFXO as core clock frequency */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize USART and map LF to CRLF */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  printf("\nEFM32 FAT Console Example. Type \"h\" (+ Enter) for command list.\n");

  /* Enable SPI access to MicroSD card */
  BSP_PeripheralAccess(BSP_MICROSD, true);

  /* Initialize filesystem */
  MICROSD_Init();
  res = f_mount(0, &Fatfs);
  if (res != FR_OK)
  {
    printf("FAT-mount failed: %d\n", res);
  }
  else
  {
    printf("FAT-mount successful\n");
  }

  /* Read command lines, and perform requested action */
  while (1)
  {
    /* Read line */
    printf("\n$ ");
    do
    {
      c = getchar();
      if (c == '\b')
      {
        printf( "\b \b" );
        if ( commandIndex ) commandIndex--;
      } else if (c > 0)
      {
        /* Local echo */
        putchar(c);
        commandLine[commandIndex] = c;
        commandIndex++;
      }
    } while ((c != '\r') && (c != '\n') && (commandIndex < (CBUFSIZE - 1)));
    commandLine[--commandIndex] = '\0';
    commandIndex = 0;
    if (strlen(commandLine) == 0 )
    {
      continue;
    }

    /* Get command */
    command = strtok(commandLine, " ");

    /* HELP command */
    if (!strcmp(command, "h"))
    {
      printf("h                  - help\n");
      printf("ls [-l]            - list files\n");
      printf("rm <file>          - remove file\n");
      printf("mkdir <dirname>    - create dir\n");;
      printf("cat <file>         - display file\n");
      printf("stat               - disk information\n");
      printf("mv <old> <new>     - rename file\n");
      printf("mount              - mount file system\n");
      printf("umount             - unmount file system\n");
      continue;
    }

    /* LS command */
    if (!strcmp(command, "ls"))
    {
      bool loption=false;
      char *option = strtok(NULL, " ");
      if ( option != NULL && !strcmp(option, "-l") )
      {
        loption = true;
      }
      strcpy(path, "");
      scan_files(path, loption);
      continue;
    }

    /* RM command */
    if (!strcmp(command, "rm"))
    {
      char *fileName = strtok(NULL, " ");
      if(fileName == NULL)
      {
        printf("usage: rm <file>");
        continue;
      }
      res = f_unlink(fileName);
      if( res != FR_OK )
      {
        printf("rm %s failed, error %u\n", fileName, res);
      }
      continue;
    }

    /* MV (rename) command */
    if (!strcmp(command, "mv"))
    {
      char *old = strtok(NULL, " ");
      char *new = strtok(NULL, " ");
      if((old == NULL) || (new == NULL))
      {
        printf("usage: mv <old> <new>");
        continue;
      }
      res = f_rename(old, new);
      if (res != FR_OK)
      {
        printf("mv %s %s failed, error %u\n", old, new, res);
      }
      continue;
    }

    /* MKDIR command */
    if (!strcmp(command, "mkdir"))
    {
      char *fileName = strtok(NULL, " ");
      if(fileName == NULL)
      {
        printf("usage: mkdir <dirname>");
        continue;
      }
      res = f_mkdir(fileName);
      if( res != FR_OK )
      {
        printf("mkdir %s failed, error %u\n", fileName, res);
      }
      continue;
    }

    /* CAT text file */
    if (!strcmp(command, "cat"))
    {
      /* Get first argument */
      char *fileName = strtok(NULL, " ");

      if (fileName == (char *) NULL)
      {
        printf("cat: Missing argument\n");
        continue;
      }

      res = f_open(&fh, fileName, FA_READ);
      if (res == FR_OK)
      {
        printf("Content of file %s\n", fileName);
        printf("-----------------------------------------\n");
        res = f_read(&fh, buffer, CBUFSIZE, &bufRead);
        if (res == FR_OK)
        {
          PrintBuf(buffer, bufRead);
        }
        else
        {
          printf("Read Failure: %d\n", res);
        }
        f_close(&fh);
      }
      else
      {
        printf("Failed to open %s, error %u\n", fileName, res);
      }
      continue;
    }

    /* DISK STATUS command */
    if (!strcmp(command, "stat"))
    {
      uint32_t sectors, bsize;
      uint16_t ssize;

      disk_initialize(Fatfs.drv);

      disk_ioctl(Fatfs.drv, GET_SECTOR_COUNT, &sectors);
      disk_ioctl(Fatfs.drv, GET_SECTOR_SIZE, &ssize);
      disk_ioctl(Fatfs.drv, GET_BLOCK_SIZE, &bsize);

      printf("Sectors:     %u\n", (unsigned int) sectors);
      printf("Sector size: %u\n", (unsigned int) ssize);
      printf("Block size:  %u\n", (unsigned int) bsize);
      continue;
    }

    /* UNMOUNT drive */
    if (!strcmp(command, "umount"))
    {
      f_mount(0, NULL);
      continue;
    }

    /* MOUNT drive */
    if (!strcmp(command, "mount"))
    {
      res = f_mount(0, &Fatfs);
      if (res != FR_OK)
      {
        printf("FAT-mount failed: %d\n", res);
      }
      continue;
    }

    printf("Unknown command: %s, \"h\" for help.", command);
  } /* end of eternal while loop */
}
