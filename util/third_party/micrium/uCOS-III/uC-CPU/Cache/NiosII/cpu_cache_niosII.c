/*
*********************************************************************************************************
*                                                 uC/CPU
*                                    CPU CONFIGURATION & PORT LAYER
*
*                             (c) Copyright 2004-2016; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at: https://doc.micrium.com
*
*               You can contact us at: www.micrium.com
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*
*                                        CPU CACHE IMPLEMENTATION
*
*                                             Altera Nios II
*
* Filename      : cpu_cache_niosII.c
* Version       : V1.31.00
* Programmer(s) : JFT
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define    MICRIUM_SOURCE
#define    NIOSII_CACHE_MODULE
#include  <cpu_cache.h>

#include  <system.h>
#include  <sys/alt_cache.h>


/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            CPU_Cache_Init()
*
* Description : Initializes the data and instruction caches of the Nios II.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_Init().
*
*               This function is an INTERNAL uC/CPU function & MUST NOT be called by application
*               function(s).
*
* Note(s)     : The Nios-II cache(s) are initialized before main() is called by the Altera HAL.
*********************************************************************************************************
*/

void  CPU_Cache_Init (void)
{

}


/*
*********************************************************************************************************
*                                        CPU_DCache_RangeFlush()
*
* Description : Flushes a range of the data cache.
*
* Argument(s) : addr_start      Byte address of the beginning of the range.
*
*               len             Size in bytes of the range.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a uC/CPU application interface (API) function &
*               MAY be called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  CPU_DCache_RangeFlush (void      *addr_start,
                             CPU_ADDR   len)
{
    alt_dcache_flush(addr_start, len);
}


/*
*********************************************************************************************************
*                                         CPU_DCache_RangeInv()
*
* Description : Invalidates a range of the data cache.
*
* Argument(s) : addr_start      Byte address of the beginning of the range.
*
*               len             Size in bytes of the range.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a Nios II BSP application interface (API) function &
*               MAY be called by application function(s).
*
* Note(s)     : The Nios-II CPU doesn't support clearing the cache in two steps, i.e. invalidate then
*               flush. The flush instruction invalidates the cache line before flushing it.
*********************************************************************************************************
*/

void  CPU_DCache_RangeInv (void      *addr_start,
                           CPU_ADDR   len)
{
   (void)&addr_start;                                           /* Prevent possible 'variable unused' warning.          */
   (void)&len;                                                  /* Prevent possible 'variable unused' warning.          */
}

