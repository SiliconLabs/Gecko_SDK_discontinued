/**************************************************************************//**
 * @file scrolllcd.c
 * @brief Scroll functions for LCD display.
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

#include "scrolllcd.h"

#define BYTES_PER_LINE  ( LS013B7DH03_WIDTH / 8 )

/**************************************************************************//**
 * @brief scrollLcd
 *   Function which is scrolling images on the display.
 *
 * @param displayDevice
 *   Pointer to display device.
 * @param pOldImg
 *   Pointer to old image which is going out.
 * @param pNewImg
 *   Pointer to image which appears on display.
 *****************************************************************************/
void scrollLcd( DISPLAY_Device_t *displayDevice,
                scrollDirection_t direction,
                const char *pOldImg,
                const char *pNewImg )
{
  int step, crossover, x, y;
  char line[ BYTES_PER_LINE ];
  const char *pNewLine, *pOldLine, *pLine;

  if ( direction == scrollLeft )
  {
    /* Iterate over all the steps */
    for ( step = 0; step <= BYTES_PER_LINE; step++ )
    {
      /* Iterate over each line */
      for ( y = 0; y < LS013B7DH03_HEIGHT; y++ )
      {
        pNewLine = &pNewImg[ BYTES_PER_LINE * y ];
        pOldLine = &pOldImg[ BYTES_PER_LINE * y ];

        /* Iterate over each byte of the line */
        for ( x = 0; x < BYTES_PER_LINE; x++ )
        {
          if ( x < ( BYTES_PER_LINE - step ) )
          {
            line[x] = pOldLine[ x + step ];
          }
          else
          {
            line[x] = pNewLine[ x - ( BYTES_PER_LINE - step ) ];
          }
        }
        displayDevice->pPixelMatrixDraw( displayDevice,
                                         line,
                                         /* start coloumn, width */
                                         0, displayDevice->geometry.width,
                                         /* start row, height */
                                         y, 1 );
      }
    }
  }
  else /* direction == scrollUp/Down */
  {
    /* Iterate over all steps. */
    for ( step = 0; step <= LS013B7DH03_HEIGHT; step+=2 )
    {
      if ( direction == scrollUp )
      {
        pLine     = &pOldImg[ BYTES_PER_LINE * step ];
        crossover = LS013B7DH03_HEIGHT - step;
        pNewLine  = pNewImg;
      }
      else
      {
        pLine     = &pNewImg[ BYTES_PER_LINE * ( LS013B7DH03_HEIGHT - step ) ];
        crossover = step;
        pNewLine  = pOldImg;
      }

      /* Iterate over each line. */
      for ( y = 0; y < LS013B7DH03_HEIGHT; y++ )
      {
        if ( y == crossover )
        {
          pLine = pNewLine;
        }

        displayDevice->pPixelMatrixDraw( displayDevice,
                                         (void*)pLine,
                                         /* start coloumn, width */
                                         0, displayDevice->geometry.width,
                                         /* start row, height */
                                         y, 1 );
        pLine += BYTES_PER_LINE;
      }
    }
  }
}
