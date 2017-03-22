/**************************************************************************//**
 * @file
 * @brief NORFLASH example for EFM32LG_DK3650 development kit
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_ebi.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include "norflash.h"
#include "bsp.h"

/**************************************************************************//**
 *
 * This example demonstrates use of the NORFLASH driver.
 *
 * Connect a terminal to the serialport of the DK (115200-N-8-1).
 * Operations on the flash are initiated by issuing commands on the terminal.
 * Command "h" will print a help screen on the terminal.
 *
 *****************************************************************************/

/** Input buffer size */
#define CMDBUFSIZE    80
#define MAXARGC       5

typedef void (*pFunc)( void );

/** Input buffer */
static char cmdBuffer[ CMDBUFSIZE + 1 ];
static int  argc;
static char *argv[ MAXARGC ];

static uint16_t pattern = 0;

/* An executable that blinks the user leds on the DK. */
static const uint8_t blink[] =
{
  0x82, 0xB0, 0x4F, 0xF0, 0x00, 0x03, 0x01, 0x93,
  0x2F, 0xE0, 0x4F, 0xF0, 0x06, 0x03, 0xC8, 0xF2,
  0x00, 0x03, 0x4F, 0xF4, 0x7F, 0x42, 0x1A, 0x80,
  0x4F, 0xF0, 0x00, 0x03, 0x00, 0x93, 0x03, 0xE0,
  0x00, 0x9B, 0x03, 0xF1, 0x01, 0x03, 0x00, 0x93,
  0x00, 0x9A, 0x48, 0xF2, 0x00, 0x03, 0xC0, 0xF2,
  0x00, 0x03, 0x9A, 0x42, 0xF4, 0xDD, 0x4F, 0xF0,
  0x06, 0x03, 0xC8, 0xF2, 0x00, 0x03, 0x4F, 0xF0,
  0xFF, 0x02, 0x1A, 0x80, 0x4F, 0xF0, 0x00, 0x03,
  0x00, 0x93, 0x03, 0xE0, 0x00, 0x9B, 0x03, 0xF1,
  0x01, 0x03, 0x00, 0x93, 0x00, 0x9A, 0x48, 0xF2,
  0x00, 0x03, 0xC0, 0xF2, 0x00, 0x03, 0x9A, 0x42,
  0xF4, 0xDD, 0x01, 0x9B, 0x03, 0xF1, 0x01, 0x03,
  0x01, 0x93, 0x01, 0x9B, 0x09, 0x2B, 0xCC, 0xDD,
  0x02, 0xB0, 0x70, 0x47
};

static bool blankCheck( uint32_t addr, uint32_t nbytes );
static void dump16( uint32_t addr );
static void getCommand( void );
static void printHelp( void );
static void splitCommandLine( void );

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main(void)
{
  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Select 48MHz clock. */
  CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFXO );

  /* Initialize USART and map LF to CRLF */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  printf("\nEFM32LG_DK3650 NORFLASH example\n\n  ");
  NORFLASH_Init();
  printHelp();

  while (1)
  {
    getCommand();

    /* Get misc. flash info */
    if ( !strcmp( argv[0], "fi" ) )
    {
      printf( " - Flash device information:\n" );
      printf( "\n  Manufacturer ID   :  0x%04X",  NORFLASH_DeviceInfo()->manufacturerId );
      printf( "\n  Device ID         :  0x%06lX", NORFLASH_DeviceInfo()->deviceId       );
      printf( "\n  Sector count      :  %ld",     NORFLASH_DeviceInfo()->sectorCount    );
      printf( "\n  Sector size       :  %ld",     NORFLASH_DeviceInfo()->sectorSize     );
      printf( "\n  Device size       :  %ld (%ldMB)", NORFLASH_DeviceInfo()->deviceSize,
                                            NORFLASH_DeviceInfo()->deviceSize/1024/1024 );
      putchar( '\n' );
    }

    /* Dump entire sector */
    else if ( !strcmp( argv[0], "dumps" ) )
    {
      uint32_t secnum, addr, end;

      secnum = strtoul( argv[1], NULL, 0 );
      addr = ( secnum * NORFLASH_DeviceInfo()->sectorSize ) + NORFLASH_DeviceInfo()->baseAddress;
      if ( !NORFLASH_AddressValid( addr ) )
      {
        printf( " - Show entire sector content, sector %ld is not a valid sector\n", secnum );
      }
      else
      {
        printf( " - Show entire sector %ld content\n", secnum );
        end = addr + NORFLASH_DeviceInfo()->sectorSize;
        while ( addr < end )
        {
          dump16( addr );
          addr += 16;
          putchar( '\n' );
        }
      }
    }

    /* Dump first 16 bytes of a sector */
    else if ( !strcmp( argv[0], "dump" ) )
    {
      uint32_t secnum, addr;

      secnum = strtoul( argv[1], NULL, 0 );
      addr = ( secnum * NORFLASH_DeviceInfo()->sectorSize ) + NORFLASH_DeviceInfo()->baseAddress;
      if ( !NORFLASH_AddressValid( addr ) )
      {
        printf( " - Show sector contents, sector %ld is not a valid sector\n", secnum );
      }
      else
      {
        printf( " - Show sector %ld contents\n", secnum );
        dump16( addr );
        putchar( '\n' );
      }
    }

    /* Erase entire device */
    else if ( !strcmp( argv[0], "eraseall" ) )
    {
      printf( " - Erasing entire device (worst case erase time is 256 seconds)\n" );
      if ( NORFLASH_EraseDevice() != NORFLASH_STATUS_OK )
      {
        printf( " ---> Device erase failure <---\n" );
      }
    }

    /* Erase a sector */
    else if ( !strcmp( argv[0], "erase" ) )
    {
      uint32_t secnum, addr;

      secnum = strtoul( argv[1], NULL, 0 );
      addr = ( secnum * NORFLASH_DeviceInfo()->sectorSize ) + NORFLASH_DeviceInfo()->baseAddress;
      if ( !NORFLASH_AddressValid( addr ) )
      {
        printf( " - Erase sector, sector %ld is not a valid sector\n", secnum );
      }
      else
      {
        printf( " - Erasing sector %ld\n", secnum );
        if ( NORFLASH_EraseSector( addr ) != NORFLASH_STATUS_OK )
          printf( " ---> Erase failure <---\n" );
      }
    }

    /* Program first 16 bytes of a sector with a pattern */
    else if ( !strcmp( argv[0], "prog" ) )
    {
      int result;
      uint32_t secnum, addr;

      secnum = strtoul( argv[1], NULL, 0 );
      addr = ( secnum * NORFLASH_DeviceInfo()->sectorSize ) + NORFLASH_DeviceInfo()->baseAddress;
      if ( !NORFLASH_AddressValid( addr ) )
      {
        printf( " - Program sector, sector %ld is not a valid sector\n", secnum );
      }
      else
      {
        printf( " - Program sector %ld with pattern 0x%04X\n", secnum, pattern );
        result = NORFLASH_STATUS_OK;
        result += NORFLASH_ProgramWord16( addr, pattern );
        result += NORFLASH_ProgramWord16( addr+2, pattern );
        result += NORFLASH_ProgramWord16( addr+4, pattern );
        result += NORFLASH_ProgramWord16( addr+6, pattern );
        result += NORFLASH_ProgramWord16( addr+8, pattern );
        result += NORFLASH_ProgramWord16( addr+10, pattern );
        result += NORFLASH_ProgramWord16( addr+12, pattern );
        result += NORFLASH_ProgramWord16( addr+14, pattern );
        if ( result != NORFLASH_STATUS_OK )
          printf( " ---> Program failure <---\n" );

        pattern += 0x0101;
      }
    }

    /* Blankcheck a sector */
    else if ( !strcmp( argv[0], "blc" ) )
    {
      uint32_t secnum, addr, size;

      secnum = strtoul( argv[1], NULL, 0 );
      size = NORFLASH_DeviceInfo()->sectorSize;
      addr = ( secnum * size ) + NORFLASH_DeviceInfo()->baseAddress;
      if ( !NORFLASH_AddressValid( addr ) )
      {
        printf( " - Blankcheck sector, sector %ld is not a valid sector\n", secnum );
      }
      else
      {
        printf( " - Blankchecking sector %ld\n", secnum );
        if ( blankCheck( addr, size ) )
        {
          printf( " Sector %ld is blank\n", secnum );
        }
      }
    }

    /* Blankcheck entire device */
    else if ( !strcmp( argv[0], "bld" ) )
    {
      uint32_t i, secsize, addr, size;

      secsize = NORFLASH_DeviceInfo()->sectorSize;
      size    = NORFLASH_DeviceInfo()->deviceSize;
      addr    = NORFLASH_DeviceInfo()->baseAddress;

      printf( " - Blankchecking entire device\n" );
      for ( i = addr; i<addr + size; i+=secsize )
      {
        if ( !blankCheck( i, secsize ) )
        {
          break;
        }
      }
      if ( i == (addr + size ) )
      {
        printf( " Device is blank\n" );
      }
    }

    /* Execute "blink" from flash sector <n> */
    else if ( !strcmp( argv[0], "exec" ) )
    {
      int result;
      pFunc exec;
      uint32_t secnum, addr;

      secnum = strtoul( argv[1], NULL, 0 );
      addr = ( secnum * NORFLASH_DeviceInfo()->sectorSize ) + NORFLASH_DeviceInfo()->baseAddress;
      if ( !NORFLASH_AddressValid( addr ) )
      {
        printf( " - Execute \"blink\", sector %ld is not a valid sector\n", secnum );
      }
      else
      {
        printf( " - Executing \"blink\" at sector %ld\n", secnum );
        result = NORFLASH_STATUS_OK;
        result += NORFLASH_EraseSector( addr );
        result += NORFLASH_Program( addr, (void*)blink, sizeof( blink ) );

        if ( result != NORFLASH_STATUS_OK )
        {
          printf( " Flash erase/program failure\n" );
        }
        else
        {
          exec = (pFunc)addr;
          /* Thumb-2 execution address must have bit0=1 !! */
          exec = (pFunc)((uint32_t)exec + 1 );
          exec();
        }
      }
    }

    /* Test misc. NORFLASH API functions */
    else if ( !strcmp( argv[0], "misc" ) )
    {
      int result;
      uint8_t array[ 6 ] = { 1,2,3,4,5,6 };

      printf( " - Testing byte, halfword and word programming in sector 0\n" );
      printf( "        Testing array programming in sector 1\n" );
      result = NORFLASH_STATUS_OK;
      result += NORFLASH_ProgramByte(   NORFLASH_DeviceInfo()->baseAddress  , 1 );
      result += NORFLASH_ProgramByte(   NORFLASH_DeviceInfo()->baseAddress+1, 15 );
      result += NORFLASH_ProgramByte(   NORFLASH_DeviceInfo()->baseAddress+2, 99 );
      result += NORFLASH_ProgramWord16( NORFLASH_DeviceInfo()->baseAddress+4, 0x1234 );
      result += NORFLASH_ProgramWord16( NORFLASH_DeviceInfo()->baseAddress+6, 0x55AA );
      result += NORFLASH_ProgramWord32( NORFLASH_DeviceInfo()->baseAddress+8, 0x11223344 );
      result += NORFLASH_ProgramWord32( NORFLASH_DeviceInfo()->baseAddress+12,0x33CC55AA );
      result += NORFLASH_Program(       NORFLASH_DeviceInfo()->baseAddress +
                                        NORFLASH_DeviceInfo()->sectorSize + 1,
                                        array, sizeof( array ) );
      printf( " Result: %s\n", result == NORFLASH_STATUS_OK ? "OK" : "Failure" );
    }

    /* Measure flash read speed */
    else if ( !strcmp( argv[0], "rspeed" ) )
    {
      #define DWT_CYCCNT      *(volatile uint32_t*)0xE0001004
      #define DWT_CTRL        *(volatile uint32_t*)0xE0001000
      uint32_t time, i;
      volatile uint32_t *p;
      uint64_t speed;

      printf( " - Flash read speed is " );
      p = (uint32_t*)NORFLASH_DeviceInfo()->baseAddress;
      i = NORFLASH_DeviceInfo()->deviceSize / 4;

      /* Make sure CYCCNT is running */
      DWT_CTRL |= 1;
      time = DWT_CYCCNT;

      while ( i-- )
      {
        *p++;
      }

      time = DWT_CYCCNT - time;
      speed = (uint64_t)CMU_ClockFreqGet(cmuClock_CORE);
      speed = (speed * NORFLASH_DeviceInfo()->deviceSize)/1024;
      speed = speed / time;
      printf( "%lu KB/sec\n", (uint32_t)speed );

      #undef DWT_CYCCNT
      #undef DWT_CTRL
    }

    /* Display help */
    else if ( !strcmp( argv[0], "h" ) )
    {
      printf( " - " );
      printHelp();
    }
    else
    {
      printf( "\n Unknown command" );
    }
  }
}

/**************************************************************************//**
 * @brief Print a help screen.
 *****************************************************************************/
static void printHelp( void )
{
  printf(
    "Available commands:\n"
    "\n    fi        : Show flash information"
    "\n    h         : Show this help"
    "\n    dump <n>  : Show first 16 bytes of sector <n>"
    "\n    dumps <n> : Show entire sector <n> content"
    "\n    blc <n>   : Blankcheck sector <n>"
    "\n    bld       : Blankcheck entire device"
    "\n    erase <n> : Erase sector <n>"
    "\n    eraseall  : Erase entire device"
    "\n    prog <n>  : Program first 16 bytes of sector <n> with byte pattern"
    "\n    exec <n>  : Execute \"blink\" from sector <n>"
    "\n    misc      : Test misc. NORFLASH API functions"
    "\n    rspeed    : Measure flash read speed"
    "\n" );
}

/**************************************************************************//**
 * @brief Get a command from the terminal on the serialport.
 *****************************************************************************/
static void getCommand( void )
{
  int c;
  int index = 0;

  printf( "\n>" );
  while (1)
  {
    c = getchar();
    if (c > 0)
    {
      /* Output character - most terminals use CRLF */
      if (c == '\r')
      {
        cmdBuffer[index] = '\0';
        splitCommandLine();
        return;
      }
      else if (c == '\b')
      {
        printf( "\b \b" );
        if ( index )
          index--;
      }
      else
      {
        /* Filter non-printable characters */
        if ((c < ' ') || (c > '~'))
          continue;

        /* Enter into buffer */
        cmdBuffer[index] = c;
        index++;
        if (index == CMDBUFSIZE)
        {
          cmdBuffer[index] = '\0';
          splitCommandLine();
          return;
        }
        /* Echo char */
        putchar(c);
      }
    }
  }
}

/**************************************************************************//**
 * @brief Split a command line into separate arguments.
 *****************************************************************************/
static void splitCommandLine( void )
{
  char *result = strtok( cmdBuffer, " " );

  argc = 0;
  while( ( result != NULL ) && ( argc < MAXARGC ) )
  {
    argc++;
    argv[ argc-1 ] = result;
    result = strtok( NULL, " " );
  }
}

/**************************************************************************//**
 * @brief Print 16 bytes in both hex and ascii form on terminal.
 *
 * @param[in] addr
 *   The address to start at.
 *****************************************************************************/
static void dump16( uint32_t addr )
{
  int i;

  printf( "%08lX: ", addr );
  /* Print data in hex format */
  for ( i=0; i<16; i++ )
  {
    printf( "%02X ", *(uint8_t*)(addr + i) );
  }
  printf( "   " );
  /* Print data in ASCII format */
  for ( i=0; i<16; i++ )
  {
    if ( isprint( *(uint8_t*)(addr + i) ) )
    {
      printf( "%c", *(uint8_t*)(addr + i) );
    }
    else
    {
      printf( " " );
    }
  }
}

/**************************************************************************//**
 * @brief Blankcheck memory.
 *
 * @param[in] addr
 *   The first address in the flash to start blankchecking.
 *
 * @param[in] nbytes
 *   Number of bytes to check.
 *
 *****************************************************************************/
static bool blankCheck( uint32_t addr, uint32_t nbytes )
{
  uint32_t i, *p;

  p = (uint32_t*)addr;
  for ( i = addr; i<addr + nbytes; i+=4 )
  {
    if ( *p != 0xFFFFFFFF )
    {
      i = (uint32_t)p;
      i = ( i - NORFLASH_DeviceInfo()->baseAddress ) / NORFLASH_DeviceInfo()->sectorSize;
      printf( " ---> Blankcheck failure at address 0x%08lX (sector %ld) <---\n",
              (uint32_t)p, i );
      return false;
    }
    p++;
  }
  return true;
}
