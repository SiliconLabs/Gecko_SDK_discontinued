/**************************************************************************//**
 * @file
 * @brief USART1 prototypes and definitions
 * @author Energy Micro AS
 * @version 4.0.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2012 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 4. The source and compiled code may only be used on Energy Micro "EFM32"
 *    microcontrollers and "EFR4" radios.
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
 *****************************************************************************/
#ifndef __TFT_CONSOLE_H
#define __TFT_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int x, y;
} TFT_Pos;

void TFT_Init(void);
void TFT_CrLf(int on);
int  TFT_WriteChar(char c);
void TFT_Puts(char *pchar);
void TFT_Print(const char *format, ...);

void TFT_PosGet(TFT_Pos * pos);
void TFT_PosSet(const TFT_Pos * pos);

#ifdef __cplusplus
}
#endif

#endif /* __TFT_CONSOLE_H */
