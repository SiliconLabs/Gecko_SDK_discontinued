/*
*********************************************************************************************************
*                                               uC/Common
*                                   Common Features for Micrium Stacks
*
*                         (c) Copyright 2013-2015; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/Common is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com
*
*               You can contact us at http://www.micrium.com
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    uC/Common - Common Definitions
*
* Filename      : common.h
* Version       : V1.01.00
* Programmer(s) : OD
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This library header file is protected from multiple pre-processor inclusion through
*               use of the library module present pre-processor macro definition.
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  COMMON_MODULE_PRESENT                                  /* See Note #1.                                         */
#define  COMMON_MODULE_PRESENT


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     COMMON MODULE VERSION NUMBER
*
* Note(s) : (1) (a) The common module software version is denoted as follows :
*
*                       Vx.yy.zz
*
*                           where
*                                   V               denotes 'Version' label
*                                   x               denotes     major software version revision number
*                                   yy              denotes     minor software version revision number
*                                   zz              denotes sub-minor software version revision number
*
*               (b) The software version label #define is formatted as follows :
*
*                       ver = x.yyzz * 100 * 100
*
*                           where
*                                   ver             denotes software version number scaled as an integer value
*                                   x.yyzz          denotes software version number, where the unscaled integer
*                                                       portion denotes the major version number & the unscaled
*                                                       fractional portion denotes the (concatenated) minor
*                                                       version numbers
*********************************************************************************************************
*/

#define  COMMON_VERSION                                10100u   /* See Note #1.                                         */


#endif /* COMMON_MODULE_PRESENT */

