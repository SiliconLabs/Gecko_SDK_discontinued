/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2016  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.34 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to Silicon Labs Norway, a subsidiary
of Silicon Labs Inc. whose registered office is 400 West Cesar Chavez,
Austin,  TX 78701, USA solely for  the purposes of creating  libraries 
for its  ARM Cortex-M3, M4F  processor-based devices,  sublicensed and 
distributed  under the  terms and conditions  of the  End User License  
Agreement supplied by Silicon Labs.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information

Licensor:                 SEGGER Software GmbH
Licensed to:              Silicon Laboratories Norway
Licensed SEGGER software: emWin
License number:           GUI-00140
License model:            See Agreement, dated 20th April 2012
Licensed product:         - 
Licensed platform:        Cortex M3, Cortex M4F
Licensed number of seats: -
----------------------------------------------------------------------
File        : GUI_HOOK.h
Purpose     : Hook handling routines
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef GUI_HOOK_H
#define GUI_HOOK_H

#include "WM_Intern.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef int GUI_HOOK_FUNC(WM_MESSAGE* pMsg);

typedef struct GUI_HOOK {
  struct GUI_HOOK* pNext;
  GUI_HOOK_FUNC*   pHookFunc;
} GUI_HOOK;

/*********************************************************************
*
*       Functions
*
**********************************************************************
*/

void GUI_HOOK_Add   (GUI_HOOK** ppFirstHook, GUI_HOOK* pNewHook, GUI_HOOK_FUNC* pHookFunc);
void GUI_HOOK_Remove(GUI_HOOK** ppFirstHook, GUI_HOOK* pHook);

#if defined(__cplusplus)
  }
#endif

#endif   /* GUI_WINSUPPORT */
#endif   /* GUI_HOOK_H */

/*************************** End of file ****************************/
