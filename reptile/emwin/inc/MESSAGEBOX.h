/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.28 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to Energy  Micro AS whose registered
office is  situated at  Sandakerveien 118,  N-0484 Oslo, NORWAY solely
for  the  purposes  of  creating  libraries  for   Energy  Micros  ARM
Cortex-M3,  M4Fb processor-based devices,  sublicensed and distributed
under  the  terms  and conditions of  the End  User License  Agreement
supplied by Energy Micro AS. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : MESSAGEBOX.h
Purpose     : Message box interface
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "WM.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

WM_HWIN MESSAGEBOX_Create(const char * sMessage, const char * sCaption, int Flags);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void MESSAGEBOX_Callback(WM_MESSAGE * pMsg);

#if defined(__cplusplus)
  }
#endif

#endif /* GUI_WINSUPPORT */

#endif /* MESSAGEBOX */
