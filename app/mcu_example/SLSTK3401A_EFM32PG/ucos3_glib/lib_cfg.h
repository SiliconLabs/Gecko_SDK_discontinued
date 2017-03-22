/**************************************************************************//**
 * @file lib_cfg.h
 * @brief uC/Lib Configuration file
 * @version 5.1.2
 *
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef  LIB_CFG_MODULE_PRESENT
#define  LIB_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*********************************************************************************************************
*                                    MEMORY LIBRARY CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                             MEMORY LIBRARY ARGUMENT CHECK CONFIGURATION
*
* Note(s) : (1) Configure LIB_MEM_CFG_ARG_CHK_EXT_EN to enable/disable the memory library suite external
*               argument check feature :
*
*               (a) When ENABLED,     arguments received from any port interface provided by the developer
*                   or application are checked/validated.
*
*               (b) When DISABLED, NO arguments received from any port interface provided by the developer
*                   or application are checked/validated.
*********************************************************************************************************
*/

                                                                /* External argument check.                             */
                                                                /* Indicates if arguments received from any port ...    */
                                                                /* ... interface provided by the developer or ...       */
                                                                /* ... application are checked/validated.               */
#define  LIB_MEM_CFG_ARG_CHK_EXT_EN     DEF_DISABLED


/*
*********************************************************************************************************
*                         MEMORY LIBRARY ASSEMBLY OPTIMIZATION CONFIGURATION
*
* Note(s) : (1) Configure LIB_MEM_CFG_OPTIMIZE_ASM_EN to enable/disable assembly-optimized memory function(s).
*********************************************************************************************************
*/

                                                                /* Assembly-optimized function(s).                      */
                                                                /* Enable/disable assembly-optimized memory ...         */
                                                                /* ... function(s). [see Note #1]                       */
#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN    DEF_DISABLED


/*
*********************************************************************************************************
*                                   MEMORY ALLOCATION CONFIGURATION
*
* Note(s) : (1) Configure LIB_MEM_CFG_DBG_INFO_EN to enable/disable memory allocation usage tracking
*               that associates a name with each segment or dynamic pool allocated.
*
*           (2) (a) Configure LIB_MEM_CFG_HEAP_SIZE with the desired size of heap memory (in octets).
*
*               (b) Configure LIB_MEM_CFG_HEAP_BASE_ADDR to specify a base address for heap memory :
*
*                   (1) Heap initialized to specified application memory, if LIB_MEM_CFG_HEAP_BASE_ADDR
*                                                                                #define'd in 'lib_cfg.h';
*                                                                         CANNOT #define to address 0x0
*
*                   (2) Heap declared to Mem_Heap[] in 'lib_mem.c',       if LIB_MEM_CFG_HEAP_BASE_ADDR
*                                                                            NOT #define'd in 'lib_cfg.h'
*********************************************************************************************************
*/

                                                                /* Allocation debugging information.                    */
                                                                /* Enable/disable allocation of debug information ...   */
                                                                /* ... associated to each memory allocation.            */
#define  LIB_MEM_CFG_DBG_INFO_EN        DEF_DISABLED


                                                                /* Heap memory size (in bytes).                         */
                                                                /* Configure the desired size of the heap memory. ...   */
                                                                /* ... Set to 0 to disable heap allocation features.    */
#define  LIB_MEM_CFG_HEAP_SIZE                  1u


                                                                /* Heap memory padding alignment (in bytes).            */
                                                                /* Configure the desired size of padding alignment ...  */
                                                                /* ... of each buffer allocated from the heap.          */
#define  LIB_MEM_CFG_HEAP_PADDING_ALIGN    LIB_MEM_PADDING_ALIGN_NONE

#if 0                                                           /* Remove this to have heap alloc at specified addr.    */
#define  LIB_MEM_CFG_HEAP_BASE_ADDR       0x00000000            /* Configure heap memory base address (see Note #2b).   */
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                    STRING LIBRARY CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                 STRING FLOATING POINT CONFIGURATION
*
* Note(s) : (1) Configure LIB_STR_CFG_FP_EN to enable/disable floating point string function(s).
*
*           (2) Configure LIB_STR_CFG_FP_MAX_NBR_DIG_SIG to configure the maximum number of significant
*               digits to calculate &/or display for floating point string function(s).
*
*               See also 'lib_str.h  STRING FLOATING POINT DEFINES  Note #1'.
*********************************************************************************************************
*/

                                                                /* Floating point feature(s).                           */
                                                                /* Enable/disable floating point to string functions.   */
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED


                                                                /* Floating point number of significant digits.         */
                                                                /* Configure the maximum number of significant ...      */
                                                                /* ... digits to calculate &/or display for ...         */
                                                                /* ... floating point string function(s).               */
#define  LIB_STR_CFG_FP_MAX_NBR_DIG_SIG         LIB_STR_FP_MAX_NBR_DIG_SIG_DFLT


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of lib cfg module include.                       */

