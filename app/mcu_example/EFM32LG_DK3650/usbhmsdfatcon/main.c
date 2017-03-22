/**************************************************************************//**
 * @file  main.c
 * @brief USB host MSD example project.
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

#include <inttypes.h>
#include "em_device.h"
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include "em_usb.h"
#include "msdh.h"
#include "ff.h"

static void CommandLoop( void );
static void ConsoleDebugInit( void );
static char *FindArgument( char *str, int stringLength );
static void PrintBuf( char *buf, int length );
static FRESULT ScanFiles( char* path );
static void PrintHelp( void );

/* File system */
static FATFS Fatfs;
static FIL   fh;

/* USB related data */
STATIC_UBUF( tmpBuf, 1024 );

/*
 * !!! IMPORTANT !!!
 *
 * The path array length is important when running ScanFiles( path ).
 * ScanFiles is recursive and will traverse the directory tree.
 * The length of "path" must equal the maximum "path + filename" length
 * existing on the disk.
 * Max path length on MSDOS FAT12/16 is 260, on FAT32 there is no limit.
 * Keep in mind that the FatFs library uses 8.3 name notation.
 *
 * When using ScanFiles() on large directory trees the default stacksize
 * might be too small due to ScanFiles() recursive operation.
 */
char path[ 260 ];

/* Command buffer and read data buffer */
#define CBUFSIZE    80
static char buffer[ CBUFSIZE ];
static UINT bufRead;
static char command[ CBUFSIZE ];

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main( void )
{
  int connectionResult;
  USBH_Init_TypeDef is = USBH_INIT_DEFAULT;

  BSP_Init(BSP_INIT_DEFAULT);   /* Initialize DK board register access */

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();
  CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFXO );

  ConsoleDebugInit();           /* Initialize DK UART port. */

  printf( "\n\nEFM32 USB Host MSD demo.\n" );

  USBH_Init( &is );             /* Initialize USB HOST stack */

  for (;;)
  {
    /* Wait for device connection */
    printf( "\nWaiting for USB MSD device plug-in...\n" );
    connectionResult = USBH_WaitForDeviceConnectionB( tmpBuf, 0 );

    if ( connectionResult == USB_STATUS_OK )
    {
      printf( "\nA device was attached\n" );

      if ( MSDH_Init( tmpBuf, sizeof( tmpBuf ) ) )
      {
        /* Execute user commands issued in console. */
        CommandLoop();
      }
      else
      {
        printf( "\nMSD initialization error, please remove device.\n" );
      }
    }

    else if ( connectionResult == USB_STATUS_DEVICE_MALFUNCTION )
    {
      printf( "\nA malfunctioning device was attached, please remove device.\n" );
    }

    else if ( connectionResult == USB_STATUS_PORT_OVERCURRENT )
    {
      printf( "\nVBUS overcurrent condition, please remove device.\n" );
    }

    while ( USBH_DeviceConnected() ){}
    printf( "\n\nDevice removal detected..." );
  }
}

/**************************************************************************//**
 * @brief Execute filesystem commands (unix'ish cat, ls and echo).
 *****************************************************************************/
static void CommandLoop( void )
{
  FRESULT res;
  char *fileName;
  int c, commandIndex;

  /* Initialize filesystem */
  res = f_mount( 0, &Fatfs );

  if ( res != FR_OK )
  {
    printf( "FAT-mount failed: %d\n", res );
    return;
  }
  else
  {
    printf( "FAT-mount successful\n" );
  }

  /* Read command lines, and perform requested action */
  while ( USBH_DeviceConnected() )
  {
    /* Read line */
    printf( "\n$ " );
    commandIndex = 0;
    do
    {
      c = getchar();
      if ( c > 0 )
      {
        /* Local echo */
        putchar( c );
        command[ commandIndex ] = c;
        commandIndex++;
      }
    } while ( ( c != '\r'                     ) &&
              ( c != '\n'                     ) &&
              ( commandIndex < (CBUFSIZE - 1) ) &&
              ( USBH_DeviceConnected()        )    );

    if ( !USBH_DeviceConnected() )
      break;

    command[ --commandIndex ] = '\0';
    commandIndex              = 0;

    /*--- HELP command -------------------------------------------------------*/
    if ( !strcmp( command, "h" ) )
    {
      PrintHelp();
    }

    /*--- LS command, traverse entire media ----------------------------------*/
    else if ( !strcmp( command, "ls" ) )
    {
      strcpy( path, "" );
      ScanFiles( path );
    }

    /*--- CAT text file ------------------------------------------------------*/
    else if ( !strncmp( command, "cat", 3 ) )
    {
      /* Get first argument */
      fileName = FindArgument( command, strlen( command ) );

      if ( fileName == (char *) NULL )
      {
        printf( "cat: Missing argument\n" );
        continue;
      }

      res = f_open( &fh, fileName, FA_READ );
      if ( res == FR_OK )
      {
        RETARGET_SerialCrLf( 0 );
        while ( 1 )
        {
          res = f_read( &fh, buffer, CBUFSIZE, &bufRead );

          if ( ( res == FR_OK ) && ( bufRead > 0 ) )
            PrintBuf( buffer, bufRead );
          else
            break;
        }
        RETARGET_SerialCrLf( 1 );
      }
      else
      {
        printf( "Failed to open %s %d\n", fileName, res );
      }
      f_close( &fh );
    }

    /*--- ECHO text to file --------------------------------------------------*/
    else if ( !strncmp( command, "echo", 4 ) )
    {
      /* Get first argument */
      fileName = FindArgument( command, strlen( command ) );

      if ( fileName == (char *) NULL )
      {
        printf( "echo: Missing argument\n" );
        continue;
      }

      res = f_open( &fh, fileName, FA_WRITE );
      if ( res != FR_OK )
      {
        res = f_open( &fh, fileName, FA_CREATE_NEW | FA_WRITE );
        if ( res != FR_OK )
        {
          printf( "Failed to open %s %d\n", fileName, res );
          continue;
        }
      }
      printf( "File size is %ld bytes.\n", fh.fsize );

      /* Set position at end of file. */
      f_lseek( &fh, fh.fsize );

      /* Append text. */
      f_printf( &fh, "The file was %d bytes long.\n", fh.fsize );

      f_close( &fh );
    }

    /*--- Display HELP info when unknown command -----------------------------*/
    else
    {
      PrintHelp();
    }
  } /* while ( USBH_DeviceConnected() ) */


  /* UNMOUNT drive */
  f_mount( 0, NULL );
}

/***************************************************************************//**
 * @brief Print help information.
 ******************************************************************************/
static void PrintHelp( void )
{
  printf( "h                  : help, display this information\n"
          "ls                 : list all files on media\n"
          "cat  <file>        : display \"file\"\n"
          "echo <file>        : append some text to \"file\", creates \"file\" if needed\n" );
}

/***************************************************************************//**
 * @brief
 *   This function is required by the FAT file system in order to provide
 *   timestamps for created files. Since we do not have a reliable clock we
 *   hardcode a value here.
 *
 *   Refer to reptile/fatfs/doc/en/fattime.html for the format of this DWORD.
 * @return
 *    A DWORD containing the current time and date as a packed datastructure.
 ******************************************************************************/
DWORD get_fattime( void )
{
  return (28 << 25) | (2 << 21) | (1 << 16);
}

/**************************************************************************//**
 * @brief ScanFiles from FatFS documentation
 * @param path to traverse
 *****************************************************************************/
static FRESULT ScanFiles( char* path )
{
  FRESULT     res;
  FILINFO     fno;
  DIR         dir;
  int         i;
  char        *fn;
#if _USE_LFN
  static char lfn[ _MAX_LFN * (_DF1S ? 2 : 1) + 1 ];
  fno.lfname = lfn;
  fno.lfsize = sizeof( lfn );
#endif

  res = f_opendir( &dir, path );
  if ( res == FR_OK )
  {
    i = strlen( path );
    for (;;)
    {
      res = f_readdir( &dir, &fno );
      if ( res != FR_OK || fno.fname[0] == 0 )
      {
        /* printf( "f_readdir failure %d\n", res ); */
        break;
      }
      if ( fno.fname[ 0 ] == '.' ) continue;
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
      if ( fno.fattrib & AM_DIR )
      {
        sprintf( &path[i], "/%s", fn );
        res = ScanFiles( path );
        if ( res != FR_OK ) break;
        path[ i ] = 0;
      }
      else
      {
        printf( "%s/%s\n", path, fn );
      }
    }
  }
  else
  {
    printf( "f_opendir failure %d\n", res );
  }

  return res;
}

/**************************************************************************//**
 * @brief Return pointer to 1st argument
 * @param stringLength - max length of string to scan for argument
 *****************************************************************************/
static char *FindArgument( char *str, int stringLength )
{
  /* Search for space, then first non-space character */
  while ( *str != ' ' && *str != '\0' )
  {
    str++;
    stringLength--;
    if ( stringLength == 0 ) return (char*)NULL;
  }

  /* Search for first non-space character */
  while ( *str == ' ' )
  {
    str++;
    stringLength--;
    if ( stringLength == 0 ) return (char*)NULL;
  }

  return str;
}

/**************************************************************************//**
 * @brief Output an array of characters
 * @param buf Pointer to string buffer
 * @param length Number of characters to output
 *****************************************************************************/
static void PrintBuf( char *buf, int length )
{
  while ( length-- ) putchar( *buf++ );
}

/**************************************************************************//**
 * @brief Initialize console I/O redirection.
 *****************************************************************************/
static void ConsoleDebugInit( void )
{
  RETARGET_SerialInit();                        /* Initialize USART   */
  RETARGET_SerialCrLf( 1 );                     /* Map LF to CRLF     */
}
