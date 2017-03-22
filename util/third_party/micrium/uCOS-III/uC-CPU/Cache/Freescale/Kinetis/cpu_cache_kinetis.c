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
*                                           Freescale Kinetis
*
* Filename      : cpu_cache_kinetis.c
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
#define    KINETIS_CACHE_MODULE
#include  <cpu_cache.h>
#include  <lib_def.h>

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/
                                                                /* ----- KINETIS LOCAL MEMORY CONTROLLER ADDRESS ------ */
#define  CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR             (0xE0082000u)

                                                                /* ------------ CORE/CODE CACHE CONTROLLER ------------ */
#define  CPU_CACHE_KINETIS_PCCCR                           *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x000)
#define  CPU_CACHE_KINETIS_PCCLCR                          *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x004)
#define  CPU_CACHE_KINETIS_PCCSAR                          *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x008)
#define  CPU_CACHE_KINETIS_PCCCVR                          *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x00C)
#define  CPU_CACHE_KINETIS_PCCRMR                          *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x020)

                                                                /* ------------- SYSTEM CACHE CONTROLLER -------------- */
#define  CPU_CACHE_KINETIS_PSCCR                           *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x800)
#define  CPU_CACHE_KINETIS_PSCLCR                          *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x804)
#define  CPU_CACHE_KINETIS_PSCSAR                          *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x808)
#define  CPU_CACHE_KINETIS_PSCCVR                          *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x80C)
#define  CPU_CACHE_KINETIS_PSCRMR                          *(CPU_REG32 *)(CPU_CACHE_KINETIS_CONTROLLER_BASE_ADDR + 0x820)

                                                                /* -------------- CACHE CONTROL REGISTER -------------- */
#define  CPU_CACHE_KINETIS_CCR_GO                           DEF_BIT_31
#define  CPU_CACHE_KINETIS_CCR_PUSHW1                       DEF_BIT_27
#define  CPU_CACHE_KINETIS_CCR_INVW1                        DEF_BIT_26
#define  CPU_CACHE_KINETIS_CCR_PUSHW0                       DEF_BIT_25
#define  CPU_CACHE_KINETIS_CCR_INVW0                        DEF_BIT_24
#define  CPU_CACHE_KINETIS_CCR_ENWRBUF                      DEF_BIT_01
#define  CPU_CACHE_KINETIS_CCR_ENCACHE                      DEF_BIT_00

                                                                /* ----------- CACHE LINE CONTROL REGISTER ------------ */
#define  CPU_CACHE_KINETIS_CLCR_LACC                        DEF_BIT_27
#define  CPU_CACHE_KINETIS_CLCR_LADSEL                      DEF_BIT_26
#define  CPU_CACHE_KINETIS_CLCR_LCMD_SEARCH_RW              DEF_BIT_MASK(0u, 24u)
#define  CPU_CACHE_KINETIS_CLCR_LCMD_INV                    DEF_BIT_MASK(1u, 24u)
#define  CPU_CACHE_KINETIS_CLCR_LCMD_PUSH                   DEF_BIT_MASK(2u, 24u)
#define  CPU_CACHE_KINETIS_CLCR_LCMD_CLEAR                  DEF_BIT_MASK(3u, 24u)
#define  CPU_CACHE_KINETIS_CLCR_LCMD_MASK                   DEF_BIT_FIELD(2u, 24u)
#define  CPU_CACHE_KINETIS_CLCR_LCWAY                       DEF_BIT_22
#define  CPU_CACHE_KINETIS_CLCR_LCIMB                       DEF_BIT_21
#define  CPU_CACHE_KINETIS_CLCR_LCIVB                       DEF_BIT_20
#define  CPU_CACHE_KINETIS_CLCR_TDSEL                       DEF_BIT_16
#define  CPU_CACHE_KINETIS_CLCR_WSEL                        DEF_BIT_14
#define  CPU_CACHE_KINETIS_CLCR_CACHEADDR_SET(addr)         DEF_BIT_MASK((addr), 2u)
#define  CPU_CACHE_KINETIS_CLCR_CACHEADDR_MASK              DEF_BIT_FIELD(10u, 2u)
#define  CPU_CACHE_KINETIS_CLCR_LGO                         DEF_BIT_00
#define  CPU_CACHE_KINETIS_CLCR_INV_BY_ADDR                (CPU_CACHE_KINETIS_CLCR_LADSEL | \
                                                            CPU_CACHE_KINETIS_CLCR_LCMD_INV)
#define  CPU_CACHE_KINETIS_CLCR_PUSH_BY_ADDR               (CPU_CACHE_KINETIS_CLCR_LADSEL | \
                                                            CPU_CACHE_KINETIS_CLCR_LCMD_PUSH)

                                                                /* ---------- CACHE SEARCH ADDRESS REGISTER ----------- */
#define  CPU_CACHE_KINETIS_CSAR_PHYADDR_SET(addr)           DEF_BIT_MASK((addr), 2u)
#define  CPU_CACHE_KINETIS_CSAR_PHYADDR_MASK                DEF_BIT_FIELD(30u, 2u)
#define  CPU_CACHE_KINETIS_CSAR_PHYADDR_SHIFT              (2u)
#define  CPU_CACHE_KINETIS_CSAR_LGO                         DEF_BIT_00


                                                                /* ----------- CACHE REGIONS MODE REGISTER ------------ */
#define  CPU_CACHE_KINETIS_CCVR_NON_CACHEABLE              (0u)
#define  CPU_CACHE_KINETIS_CCVR_WRITE_THROUGH              (2u)
#define  CPU_CACHE_KINETIS_CCVR_WRITE_BACK                 (3u)
#define  CPU_CACHE_KINETIS_CCVR_R0_SET(mode)                DEF_BIT_MASK((mode), 30u)
#define  CPU_CACHE_KINETIS_CCVR_R0_MASK                     DEF_BIT_FIELD(2u, 30u)
#define  CPU_CACHE_KINETIS_CCVR_R1_SET(mode)                DEF_BIT_MASK((mode), 28u)
#define  CPU_CACHE_KINETIS_CCVR_R1_MASK                     DEF_BIT_FIELD(2u, 28u)
#define  CPU_CACHE_KINETIS_CCVR_R2_SET(mode)                DEF_BIT_MASK((mode), 26u)
#define  CPU_CACHE_KINETIS_CCVR_R2_MASK                     DEF_BIT_FIELD(2u, 26u)
#define  CPU_CACHE_KINETIS_CCVR_R3_SET(mode)                DEF_BIT_MASK((mode), 24u)
#define  CPU_CACHE_KINETIS_CCVR_R3_MASK                     DEF_BIT_FIELD(2u, 24u)
#define  CPU_CACHE_KINETIS_CCVR_R4_SET(mode)                DEF_BIT_MASK((mode), 22u)
#define  CPU_CACHE_KINETIS_CCVR_R4_MASK                     DEF_BIT_FIELD(2u, 22u)
#define  CPU_CACHE_KINETIS_CCVR_R5_SET(mode)                DEF_BIT_MASK((mode), 20u)
#define  CPU_CACHE_KINETIS_CCVR_R5_MASK                     DEF_BIT_FIELD(2u, 20u)
#define  CPU_CACHE_KINETIS_CCVR_R6_SET(mode)                DEF_BIT_MASK((mode), 18u)
#define  CPU_CACHE_KINETIS_CCVR_R6_MASK                     DEF_BIT_FIELD(2u, 18u)
#define  CPU_CACHE_KINETIS_CCVR_R7_SET(mode)                DEF_BIT_MASK((mode), 16u)
#define  CPU_CACHE_KINETIS_CCVR_R7_MASK                     DEF_BIT_FIELD(2u, 16u)
#define  CPU_CACHE_KINETIS_CCVR_R8_SET(mode)                DEF_BIT_MASK((mode), 14u)
#define  CPU_CACHE_KINETIS_CCVR_R8_MASK                     DEF_BIT_FIELD(2u, 14u)
#define  CPU_CACHE_KINETIS_CCVR_R9_SET(mode)                DEF_BIT_MASK((mode), 12u)
#define  CPU_CACHE_KINETIS_CCVR_R9_MASK                     DEF_BIT_FIELD(2u, 12u)
#define  CPU_CACHE_KINETIS_CCVR_R10_SET(mode)               DEF_BIT_MASK((mode), 10u)
#define  CPU_CACHE_KINETIS_CCVR_R10_MASK                    DEF_BIT_FIELD(2u, 10u)
#define  CPU_CACHE_KINETIS_CCVR_R11_SET(mode)               DEF_BIT_MASK((mode), 8u)
#define  CPU_CACHE_KINETIS_CCVR_R11_MASK                    DEF_BIT_FIELD(2u, 8u)
#define  CPU_CACHE_KINETIS_CCVR_R12_SET(mode)               DEF_BIT_MASK((mode), 6u)
#define  CPU_CACHE_KINETIS_CCVR_R12_MASK                    DEF_BIT_FIELD(2u, 6u)
#define  CPU_CACHE_KINETIS_CCVR_R13_SET(mode)               DEF_BIT_MASK((mode), 4u)
#define  CPU_CACHE_KINETIS_CCVR_R13_MASK                    DEF_BIT_FIELD(2u, 4u)
#define  CPU_CACHE_KINETIS_CCVR_R14_SET(mode)               DEF_BIT_MASK((mode), 2u)
#define  CPU_CACHE_KINETIS_CCVR_R14_MASK                    DEF_BIT_FIELD(2u, 2u)
#define  CPU_CACHE_KINETIS_CCVR_R15_SET(mode)               DEF_BIT_MASK((mode), 0u)
#define  CPU_CACHE_KINETIS_CCVR_R15_MASK                    DEF_BIT_FIELD(2u, 0u)


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

void  CPU_Cache_Do (CPU_INT32U   what,
                    void        *addr_start,
                    CPU_ADDR     len);


/*
*********************************************************************************************************
*                                       LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            CPU_Cache_Init()
*
* Description : Initializes the cache.
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
* Note(s)     : none.
*********************************************************************************************************
*/

void  CPU_Cache_Init (void)
{
#if (defined(CPU_CFG_CACHE_MGMT_EN) && CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)
                                                                /* ------------ INVALIDATE & ENABLE CACHES ------------ */
    CPU_CACHE_KINETIS_PCCCR = CPU_CACHE_KINETIS_CCR_GO      |
                              CPU_CACHE_KINETIS_CCR_INVW1   | CPU_CACHE_KINETIS_CCR_INVW0 |
                              CPU_CACHE_KINETIS_CCR_ENWRBUF | CPU_CACHE_KINETIS_CCR_ENCACHE;

    while ((CPU_CACHE_KINETIS_PCCCR & CPU_CACHE_KINETIS_CCR_GO) > 0) {
        ;
    }

    CPU_CACHE_KINETIS_PSCCR = CPU_CACHE_KINETIS_CCR_GO      |
                              CPU_CACHE_KINETIS_CCR_INVW1   | CPU_CACHE_KINETIS_CCR_INVW0 |
                              CPU_CACHE_KINETIS_CCR_ENWRBUF | CPU_CACHE_KINETIS_CCR_ENCACHE;

    while ((CPU_CACHE_KINETIS_PSCCR & CPU_CACHE_KINETIS_CCR_GO) > 0) {
        ;
    }
#endif
}


/*
*********************************************************************************************************
*                                        CPU_DCache_RangeFlush()
*
* Description : Flushes a range of the data cache to the main memory.
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
#if (defined(CPU_CFG_CACHE_MGMT_EN) && CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)
                                                                /* Flush range, if found, to main memory                */
    CPU_Cache_Do(CPU_CACHE_KINETIS_CLCR_PUSH_BY_ADDR, addr_start, len);
#else
                                                                /* Prevent possible compiler warning.                   */
   (void)&addr_start;
   (void)&len;
#endif
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
*               This function is a uC/CPU application interface (API) function &
*               MAY be called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  CPU_DCache_RangeInv (void      *addr_start,
                           CPU_ADDR   len)
{
#if (defined(CPU_CFG_CACHE_MGMT_EN) && CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)
                                                                /* Invalidate range, if found, in cache                 */
    CPU_Cache_Do(CPU_CACHE_KINETIS_CLCR_INV_BY_ADDR, addr_start, len);
#else
                                                                /* Prevent possible compiler warning.                   */
   (void)&addr_start;
   (void)&len;
#endif
}


/*
*********************************************************************************************************
*                                             CPU_Cache_Do()
*
* Description : Does 'what' to range within addr_start and addr_start + len.
*
* Argument(s) : what            Cache action to perform:
*
*                               CPU_CACHE_KINETIS_CLCR_INV_BY_ADDR          Invalidate range.
*                               CPU_CACHE_KINETIS_CLCR_PUSH_BY_ADDR         Flush range.
*
*               addr_start      Byte address of the beginning of the range.
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

void  CPU_Cache_Do (CPU_INT32U   what,
                    void        *addr_start,
                    CPU_ADDR     len)
{
#if (defined(CPU_CFG_CACHE_MGMT_EN) && CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)
    CPU_INT32U  addr;
    CPU_INT32U  start;
    CPU_INT32U  end;

                                                                /* Smallest addressable cache unit is one 4 byte word.  */
    start = ((CPU_INT32U)(addr_start)      ) >> CPU_CACHE_KINETIS_CSAR_PHYADDR_SHIFT;
    end   = ((CPU_INT32U)(addr_start) + len) >> CPU_CACHE_KINETIS_CSAR_PHYADDR_SHIFT;

                                                                /* Set CLCR command to what by Address                  */
    CPU_CACHE_KINETIS_PSCLCR = what;

    for (addr = start; addr <= end; ++addr) {
                                                                /* Flush or invalidate word, if found, to memory        */
        CPU_CACHE_KINETIS_PSCSAR = CPU_CACHE_KINETIS_CSAR_PHYADDR_SET(addr) | CPU_CACHE_KINETIS_CSAR_LGO;

        while ((CPU_CACHE_KINETIS_PSCSAR & CPU_CACHE_KINETIS_CSAR_LGO) > 0) {
            ;
        }
    }
#else
                                                                /* Prevent possible compiler warning.                   */
   (void)&what;
   (void)&addr_start;
   (void)&len;
#endif
}

