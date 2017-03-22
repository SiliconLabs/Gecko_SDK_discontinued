/*
*********************************************************************************************************
*                                                uC/CPU
*                                    CPU CONFIGURATION & PORT LAYER
*
*                          (c) Copyright 2004-2016; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/CPU is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                       CPU CACHE IMPLEMENTATION
*                                           ARMv7-M L1 Cache
*
* Filename      : cpu_cache_armv7m_generic_l1.c
* Version       : V1.31.00
* Programmer(s) : JBL
*                 SB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/
#include <cpu.h>
#include "../../../cpu_cache.h"
#include <lib_def.h>


#ifdef __cplusplus
extern  "C" {
#endif

/*
*********************************************************************************************************
*                                        MACROS AND DEFINIITIONS
*********************************************************************************************************
*/
                                                                /* ------ CACHE CONTROL IDENTIFICATION REGISTERS ------ */
#define  SCS_CLIDR      (*((CPU_REG32 *)(0xE000ED78u)))
#define  SCS_CTR        (*((CPU_REG32 *)(0xE000ED7Cu)))
#define  SCS_CCSIDR     (*((CPU_REG32 *)(0xE000ED80u)))
#define  SCS_CCSELR     (*((CPU_REG32 *)(0xE000ED84u)))
                                                                /* ----------- CACHE MAINTENANCE OPERATIONS ----------- */
#define  SCS_ICIALLU    (*((CPU_REG32 *)(0xE000EF50u)))         /* Invalidate I-cache to PoU                            */
#define  SCS_ICIMVAU    (*((CPU_REG32 *)(0xE000EF58u)))         /* Invalidate I-cache to PoU by MVA                     */
#define  SCS_DCIMVAC    (*((CPU_REG32 *)(0xE000EF5Cu)))         /* Invalidate D-cache to PoC by MVA                     */
#define  SCS_DCISW      (*((CPU_REG32 *)(0xE000EF60u)))         /* Invalidate D-cache by Set/Way                        */
#define  SCS_DCCMVAU    (*((CPU_REG32 *)(0xE000EF64u)))         /* Clean D-cache to PoU by MVA                          */
#define  SCS_DCCMVAC    (*((CPU_REG32 *)(0xE000EF68u)))         /* Clean D-cache to PoC by MVA                          */
#define  SCS_DCCSW      (*((CPU_REG32 *)(0xE000EF6Cu)))         /* Clean D-cache by Set/Way                             */
#define  SCS_DCCIMVAC   (*((CPU_REG32 *)(0xE000EF70u)))         /* Clean and invalidate D-cache by MVA                  */
#define  SCS_DCCISW     (*((CPU_REG32 *)(0xE000EF74u)))         /* Clean and invalidate D-cache by Set/Way              */
#define  SCS_BPIALL     (*((CPU_REG32 *)(0xE000EF78u)))         /* Invalidate Branch predictor                          */


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  CPU_INT32U  CPU_DCache_LineSizeGet (void);


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  CPU_INT32U CPU_Cache_Linesize;                          /* Cache line size.                                     */


/*
*********************************************************************************************************
*                                           CPU_Cache_Init()
*
* Description : Initialize cpu cache module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_Init().
*
*               This function is an INTERNAL CPU module function & MUST NOT be called by application
*               function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  CPU_Cache_Init (void)
{
    CPU_Cache_Linesize = CPU_DCache_LineSizeGet();
}


/*
*********************************************************************************************************
*                                       CPU_DCache_LineSizeGet()
*
* Description : Returns the cache line size.
*
* Prototypes  : CPU_INT32U  CPU_DCache_LineSizeGet (void)
*
* Argument(s) : none.
*
* Note(s)     : Line Size = 2^(CCSIDR[2:0] + 2)
*********************************************************************************************************
*/

static  CPU_INT32U  CPU_DCache_LineSizeGet (void)
{
    return (1u << ((SCS_CCSIDR & 0x7u)) + 2u);
}


/*
*********************************************************************************************************
*                                      INVALIDATE DATA CACHE RANGE
*
* Description : Invalidate a range of data cache by MVA.
*
* Prototypes  : void  CPU_DCache_RangeInv  (void      *p_mem,
*                                           CPU_ADDR   len);
*
* Argument(s) : p_mem    Start address of the region to invalidate.
*
*               range    Size of the region to invalidate in bytes.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  CPU_DCache_RangeInv (void      *addr_start,
                           CPU_ADDR   len)
{
                                                                /* Align the address according to the line size.        */
    addr_start = (void *)((CPU_ADDR)addr_start & ~(CPU_Cache_Linesize - 1u));

    CPU_MB();

    while(len > CPU_Cache_Linesize) {
        SCS_DCIMVAC = (CPU_ADDR)addr_start;
        addr_start = (void *)((CPU_ADDR)addr_start + CPU_Cache_Linesize);
        len -= CPU_Cache_Linesize;
    }

    if (len > 0u) {
        SCS_DCIMVAC = (CPU_ADDR)addr_start;
    }

    CPU_MB();
}


/*
*********************************************************************************************************
*                                        FLUSH DATA CACHE RANGE
*
* Description : Flush (clean) a range of data cache by MVA.
*
* Prototypes  : void  CPU_DCache_RangeFlush  (void      *p_mem,
*                                             CPU_ADDR   len);
*
* Argument(s) : p_mem    Start address of the region to flush.
*
*               range    Size of the region to invalidate in bytes.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  CPU_DCache_RangeFlush  (void      *addr_start,
                              CPU_ADDR   len)
{
                                                                /* Align the address according to the line size.        */
    addr_start = (void *)((CPU_ADDR)addr_start & ~(CPU_Cache_Linesize - 1u));

    CPU_MB();

    while(len > CPU_Cache_Linesize) {
        SCS_DCCMVAC = (CPU_ADDR)addr_start;
        addr_start = (void *)((CPU_ADDR)addr_start + CPU_Cache_Linesize);
        len -= CPU_Cache_Linesize;
    }

    if (len > 0u) {
        SCS_DCCMVAC = (CPU_ADDR)addr_start;
    }

    CPU_MB();
}

#ifdef __cplusplus
}
#endif
