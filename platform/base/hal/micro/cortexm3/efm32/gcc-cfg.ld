/* GCC linker configuration file.                                             */
/*                                                                            */
/* Copyright 2015 Silicon Laboratories, Inc.                               *80*/
/*----------------------------------------------------------------------------*/

/* Memory map for the EFR32/EZR32 */
/* GCC ld will not accept symbols for origin or length. This memory block
   definiton will have to be generated per variant or moved to a parameterized
   include file in the future. */
MEMORY {
  flash (rx) : ORIGIN = 0x00000000, LENGTH = 0x00040000
  ram  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

/* Default to 256k Flash and 32K Ram */
FLASH_SIZE = DEFINED(FLASH_SIZE) ? FLASH_SIZE : 256K;
RAM_SIZE = DEFINED(RAM_SIZE) ? (RAM_SIZE==31K ? 32K : RAM_SIZE) : 32K;

/* Default to 2 kB flash page size */
FLASH_PAGE_SIZE     = DEFINED(FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : 0x800;

/* Create useful defines for the gcc.ld file to use */
RAM_BOTTOM          = 0x20000000;
RAM_TOP             = RAM_BOTTOM + RAM_SIZE - 1;

MFB_BOTTOM          = 0x00000000;
MFB_TOP             = MFB_BOTTOM + FLASH_SIZE - 1;

/* The FIB does not exist on the EFR32/EZR32 */
FIB_BOTTOM          = 0xFFFFFFFF;
FIB_TOP             = 0xFFFFFFFF;

/* Default bootloader size is hardcoded to 16 kB -- also see memmap.h */
DEFAULT_BTL_SIZE    = 0x4000;

/* If we were given a RAM_RETAIN_SIZE then make sure that we don't use */
/* any RAM outside of that range by setting it to be in the unused region. */
UNUSED_RAM_SIZE = DEFINED(RAM_RETAIN_SIZE) ? (RAM_TOP - RAM_BOTTOM + 1 - RAM_RETAIN_SIZE): 0;

/* Include the common gcc.ld linker script */
INCLUDE "gcc.ld"
