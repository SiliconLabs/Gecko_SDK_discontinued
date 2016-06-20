/***************************************************************************//**
 * @file    iec60335_class_b_def.h
 * @brief   Defines used in the IEC60335 test assembly files
 *              header for asm based POST and BIST CPU register test for all compiler
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2010 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#ifndef __IEC60335_CLASS_B_DEF_H__
#define __IEC60335_CLASS_B_DEF_H__

/*! @addtogroup IEC60335_CONFIG
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* define compiler specific symbols */
#if defined (__CC_ARM)      /*--RealView Compiler --*/
/*! @cond RealView */
    #ifndef __ASM
    #define __ASM             __asm                               /*!< asm keyword for ARM Compiler */
    #endif
    #ifndef __INLINE
    #define __INLINE          __inline                            /*!< inline keyword for ARM Compiler */
    #endif
    #define FLASH_CRC_ADDR    0x000000E0                          /*!< start address of Flash information */
    #define __STEXT           FLASH_CRC_ADDR + sizeof(FlashCRC_t) /*!< start address of Flash code */
/*! @endcond */

#elif (defined (__ICCARM__))    /*-- ICC Compiler --*/
/*! @cond ICC */
    #ifndef __ASM
    #define __ASM             __asm                 /*!< asm keyword for IAR Compiler */
    #endif
    #ifndef __INLINE
    #define __INLINE          inline                /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
    #endif
    #define FLASH_CRC_ADDR    0x000010BA            /* LENGTH = 0x20 */
    #define __STEXT           0x100
/*! @endcond */

#elif (defined (__GNUC__))      /*-- GNU Compiler --*/
/*! @cond GCC */
    #ifndef __ASM
    #define __ASM             __asm                 /*!< asm keyword for GNU Compiler          */
    #endif
    #undef __INLINE
    #define __INLINE          __inline__            /*!< inline keyword for GNU Compiler       */
extern unsigned long _crc_location;                 /* linker reference : start CRC entry      */
    #define FLASH_CRC_ADDR    &_crc_location        /*!< CRC location in Flash                 */
extern unsigned long __text_start;                  /* linker reference : start text section   */
    #define __STEXT           &__text_start         /*!< start address of Flash code           */
/*! @endcond */

#elif defined   (__TASKING__)
/*! @cond TASKING */
    #error "not implemented yet !"                  /*!< TASKING Compiler is not implemented   */
/*! @endcond */

#else
    #error "iec60355_class_b_def.h: undefined compiler"
#endif

#ifdef TESSY
/*! @cond TESSY */
#define LoopForever()    return                     /*!< macro enables automated test */
/*! @endcond */
#else
#define LoopForever()    while (1)                  /*!< macro enables automated test */
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif /* __IEC60335_CLASS_B_DEF_H__ */

/************************************** EOF *********************************/
