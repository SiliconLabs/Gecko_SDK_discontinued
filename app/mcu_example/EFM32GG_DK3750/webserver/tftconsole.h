/**************************************************************************//**
 * @file
 * @brief USART1 prototypes and definitions
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
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
