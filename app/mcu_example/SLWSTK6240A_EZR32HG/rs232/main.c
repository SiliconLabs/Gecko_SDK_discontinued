/**************************************************************************//**
 * @file
 * @brief USART/LEUART/VCOM RS232 example for SLWSTK6240A_EZR32HG starter kit
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "retargetserial.h"

/*
 * Note! You can set compile time define -DRETARGET_LEUART0 to build this
 * example to use LEUART0 instead of default VCOM.
 * See retargetserialconfig.h for details.
 */

/** Input buffer size */
#define ECHOBUFSIZE    80
/** Input buffer */
static char echoBuffer[ECHOBUFSIZE];

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  int c;
  int index;

  /* Chip errata */
  CHIP_Init();

  /* Initialize LEUART/USART and map LF to CRLF */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

#if defined(RETARGET_LEUART0)
  printf("\nEZR32 LEUART example\n");
#elif defined(RETARGET_VCOM)
  printf("\nEZR32 VCOM example\n");
#else
  printf("\nEZR32 USART example\n");
#endif

  for (index = 0; index < ECHOBUFSIZE; index++)
  {
    echoBuffer[index] = (char) 'a' + index;
  }

  /* Retrieve characters, print local echo and full line back */
  index = 0;
  while (1)
  {
    /* Retrieve new character */
    c = getchar();
    if (c > 0)
    {
      /* Output character - most terminals use CRLF */
      if (c == '\r')
      {
        echoBuffer[index] = '\0';
        /* Output entire line */
        printf("\n%s\n", echoBuffer);
        index = 0;
      }
      else
      {
        /* Filter non-printable characters */
        if ((c < ' ') || (c > '~'))
          continue;

        /* Enter into buffer */
        echoBuffer[index] = c;
        index++;
        if (index == ECHOBUFSIZE)
        {
          /* Flush buffer */
          index = 0;
        }
        /* Local echo */
        putchar(c);
      }
    }
  }
}
