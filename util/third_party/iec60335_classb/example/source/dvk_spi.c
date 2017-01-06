/***************************************************************************//**
 * @file    dvk_spi.c
 * @brief   main code for applications for IEC60335 verification on EFM32 devices.
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "iec60335_class_b.h"
#include "efm32g890f128.h"
#include "dvk_spi.h"

/*! @addtogroup IEC60335_TOOLS
* @{
*/

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
#pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
/*! private function for SPI communicaTION.
 */
void DVK_SPI_Com(uint8_t Addr, uint16_t Data)
{
  uint16_t Dummy = Addr;
  GPIO->P[2].DOUTCLR = (1 << 5);          // CS
  while (!(USART2->STATUS & USART_STATUS_TXBL)) ;
  USART2->TXDATA = Dummy;                 // addr
  while (!(USART2->STATUS & USART_STATUS_RXDATAV)) ;
  Dummy = USART2->RXDATA;
  while (!(USART2->STATUS & USART_STATUS_TXBL)) ;
  USART2->TXDATA = (Data & 0x00FF);       // LSB
  while (!(USART2->STATUS & USART_STATUS_RXDATAV)) ;
  Dummy = USART2->RXDATA;
  while (!(USART2->STATUS & USART_STATUS_TXBL)) ;
  USART2->TXDATA = ((Data & 0xFF00)>>8);  // MSB
  while (!(USART2->STATUS & USART_STATUS_RXDATAV)) ;
  Dummy = USART2->RXDATA;
  GPIO->P[2].DOUTSET = (1 << 5);          // CS
}

/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
#pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
/*! private function to enable LED communication, set LED0, set LED0 and 1
 * use this function to show that the WDOG reset occurred and changed the system.
 * a complete check will enable the LED0 until the WDOG reset occurred and
 * than change the LEDs to LED0 and LED1.
 */
void DVK_SPI(uint8_t mode)
{
  if (mode == 0) {
    /* Enable module clocks */
    CMU->HFCORECLKEN0 |= (1<<0x0C)|(1<<0x02);
    CMU->HFPERCLKDIV |= (1<<0x08);
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO | CMU_HFPERCLKEN0_USART2 ;

    /* Configure SPI pins */
    GPIO->P[2].DOUTCLR = (1<<0x0D)|(1<<0x02)|(1<<0x03)|(1<<0x04)|(1<<0x05);
    GPIO->P[2].MODEH |= 0x00400000;
    GPIO->P[2].MODEL |= 0x00444400;
    /* Keep CS high to not activate slave */

    /* Enable pins at default location */
    USART2->ROUTE = 0x0B;
    /* Reset USART just in case */
    USART2->CMD = USART_CMD_RXDIS | USART_CMD_TXDIS | USART_CMD_MASTERDIS |
                  USART_CMD_RXBLOCKDIS | USART_CMD_TXTRIDIS | USART_CMD_CLEARTX | USART_CMD_CLEARRX;
    USART2->CTRL     = _USART_CTRL_RESETVALUE;
    USART2->FRAME    = _USART_FRAME_RESETVALUE;
    USART2->TRIGCTRL = _USART_TRIGCTRL_RESETVALUE;
    USART2->CLKDIV   = _USART_CLKDIV_RESETVALUE;
    USART2->IEN      = _USART_IEN_RESETVALUE;
    USART2->IFC      = _USART_IFC_MASK;
    USART2->IRCTRL   = _USART_IRCTRL_RESETVALUE;

    /* Configure to use SPI master with manual CS */
    /* For now, configure SPI for worst case 32MHz clock in order to work for all */
    /* configurations. */

    USART2->CMD = USART_CMD_RXDIS | USART_CMD_TXDIS | USART_CMD_MASTERDIS;
    USART2->CTRL &= ~(_USART_CTRL_CLKPOL_MASK |  _USART_CTRL_CLKPHA_MASK |
                      _USART_CTRL_MSBF_MASK | _USART_CTRL_SCMODE_MASK |
                      _USART_CTRL_SCRETRANS_MASK);

    /* Set bits for synchronous mode */
    USART2->CTRL |= USART_CTRL_SYNC;
    USART2->IRCTRL &= ~USART_IRCTRL_IREN;
    USART2->FRAME = USART_FRAME_DATABITS_EIGHT | USART_FRAME_STOPBITS_DEFAULT | USART_FRAME_PARITY_DEFAULT;
    USART2->CTRL = USART_CTRL_SYNC;
    USART2->CMD = USART_CMD_MASTEREN;
    USART2->CMD = USART_CMD_RXEN | USART_CMD_TXEN;
    USART2->CLKDIV = 0x200;

    // clear LED
    DVK_SPI_Com(0, 0x0006);
    DVK_SPI_Com(1, 0x8C00);
    DVK_SPI_Com(2, 0x0003);
    DVK_SPI_Com(3, 0x0000);

  }
  else if (mode == 1) {
    // set LED0
    DVK_SPI_Com(0, 0x0006);
    DVK_SPI_Com(1, 0x8C00);
    DVK_SPI_Com(2, 0x0003);
    DVK_SPI_Com(3, 0x0001);
  }
  else{
    // set LED0,1
    DVK_SPI_Com(0, 0x0006);
    DVK_SPI_Com(1, 0x8C00);
    DVK_SPI_Com(2, 0x0003);
    DVK_SPI_Com(3, 0x0003);
  }
}
/*!
 * @}
*/
/************************************** EOF *********************************/
