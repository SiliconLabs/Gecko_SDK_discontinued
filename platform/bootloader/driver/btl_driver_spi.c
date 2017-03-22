/***************************************************************************//**
 * @file btl_driver_spi.c
 * @brief Hardware driver layer for simple SPI transactions.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "config/btl_config.h"

#include "btl_driver_spi.h"

#include "em_cmu.h"
#include "em_usart.h"
#include "em_gpio.h"

#define BTL_DRIVER_SPI_USART_TXLOC    (BTL_DRIVER_SPI_USART_TXLOC_NUMBER  << _USART_ROUTELOC0_TXLOC_SHIFT)
#define BTL_DRIVER_SPI_USART_RXLOC    (BTL_DRIVER_SPI_USART_RXLOC_NUMBER << _USART_ROUTELOC0_RXLOC_SHIFT)
#define BTL_DRIVER_SPI_USART_CLKLOC   (BTL_DRIVER_SPI_USART_CLKLOC_NUMBER << _USART_ROUTELOC0_CLKLOC_SHIFT)

#if BTL_DRIVER_SPI_USART_NUMBER == 0
#define BTL_DRIVER_SPI_USART        USART0
#define BTL_DRIVER_SPI_USART_CLOCK  cmuClock_USART0
#elif BTL_DRIVER_SPI_USART_NUMBER == 1
#define BTL_DRIVER_SPI_USART        USART1
#define BTL_DRIVER_SPI_USART_CLOCK  cmuClock_USART1
#elif BTL_DRIVER_SPI_USART_NUMBER == 2
#define BTL_DRIVER_SPI_USART        USART2
#define BTL_DRIVER_SPI_USART_CLOCK  cmuClock_USART2
#elif BTL_DRIVER_SPI_USART_NUMBER == 3
#define BTL_DRIVER_SPI_USART        USART3
#define BTL_DRIVER_SPI_USART_CLOCK  cmuClock_USART3
#elif BTL_DRIVER_SPI_USART_NUMBER == 4
#define BTL_DRIVER_SPI_USART        USART4
#define BTL_DRIVER_SPI_USART_CLOCK  cmuClock_USART4
#elif BTL_DRIVER_SPI_USART_NUMBER == 5
#define BTL_DRIVER_SPI_USART        USART5
#define BTL_DRIVER_SPI_USART_CLOCK  cmuClock_USART5
#elif BTL_DRIVER_SPI_USART_NUMBER == 6
#define BTL_DRIVER_SPI_USART        USART6
#define BTL_DRIVER_SPI_USART_CLOCK  cmuClock_USART6
#else
#error "Invalid BTL_DRIVER_SPI_USART"
#endif

void spi_init(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(BTL_DRIVER_SPI_USART_CLOCK, true);

  // MOSI
  GPIO_PinModeSet(BTL_DRIVER_SPI_MOSI_PORT, BTL_DRIVER_SPI_MOSI_PIN, gpioModePushPull,  0);
  // MISO
  GPIO_PinModeSet(BTL_DRIVER_SPI_MISO_PORT, BTL_DRIVER_SPI_MISO_PIN, gpioModeInputPull, 0);
  // CLK
  GPIO_PinModeSet(BTL_DRIVER_SPI_CLK_PORT, BTL_DRIVER_SPI_CLK_PIN, gpioModePushPull,  0);
  // CS#
  GPIO_PinModeSet(BTL_DRIVER_SPI_CS_PORT, BTL_DRIVER_SPI_CS_PIN, gpioModePushPull,  1);

  // Don't do USART_InitSync here since it pulls in ClockFreqGet which is a terrifyingly large function

  // Make sure disabled first, before resetting other registers
  BTL_DRIVER_SPI_USART->CMD = USART_CMD_RXDIS
                              | USART_CMD_TXDIS
                              | USART_CMD_MASTERDIS
                              | USART_CMD_RXBLOCKDIS
                              | USART_CMD_TXTRIDIS
                              | USART_CMD_CLEARTX
                              | USART_CMD_CLEARRX;
  BTL_DRIVER_SPI_USART->TRIGCTRL  = _USART_TRIGCTRL_RESETVALUE;
  BTL_DRIVER_SPI_USART->IEN       = _USART_IEN_RESETVALUE;
  BTL_DRIVER_SPI_USART->IFC       = _USART_IFC_MASK;

  // Set up for SPI
  BTL_DRIVER_SPI_USART->CTRL = _USART_CTRL_RESETVALUE
                               | USART_CTRL_SYNC
                               | USART_CTRL_CLKPOL_IDLELOW
                               | USART_CTRL_CLKPHA_SAMPLELEADING
                               | USART_CTRL_MSBF;

  // Configure databits, leave stopbits and parity at reset default (not used)
  BTL_DRIVER_SPI_USART->FRAME = _USART_FRAME_RESETVALUE
                                | USART_FRAME_DATABITS_EIGHT;

  // Configure baudrate
  uint64_t clkdiv;
  if(CMU->HFCLKSTATUS == CMU_HFCLKSTATUS_SELECTED_HFRCO) {
    clkdiv = (128ULL*19000000)/BTL_DRIVER_SPI_BAUDRATE - 256;
  } else {
    clkdiv = (128ULL*38400000)/BTL_DRIVER_SPI_BAUDRATE - 256;
  }
  clkdiv = ((clkdiv + 128)/256) << 8;
  clkdiv &= _USART_CLKDIV_DIV_MASK;
  BTL_DRIVER_SPI_USART->CLKDIV = clkdiv;

  // Finally enable (as specified)
  BTL_DRIVER_SPI_USART->CMD = USART_CMD_MASTEREN;

  BTL_DRIVER_SPI_USART->ROUTEPEN  = USART_ROUTEPEN_TXPEN
                                    | USART_ROUTEPEN_RXPEN
                                    | USART_ROUTEPEN_CLKPEN;

  BTL_DRIVER_SPI_USART->ROUTELOC0 = BTL_DRIVER_SPI_USART_TXLOC
                                    | BTL_DRIVER_SPI_USART_RXLOC
                                    | BTL_DRIVER_SPI_USART_CLKLOC;

  BTL_DRIVER_SPI_USART->CMD = USART_CMD_RXEN | USART_CMD_TXEN;
}

void spi_writeByte(uint8_t data)
{
  USART_SpiTransfer(BTL_DRIVER_SPI_USART, data);
}

void spi_writeHalfword(uint16_t data)
{
  USART_Tx(BTL_DRIVER_SPI_USART, (data >> 8) & 0xFF);
  USART_Tx(BTL_DRIVER_SPI_USART, data & 0xFF);
  USART_Rx(BTL_DRIVER_SPI_USART);
  USART_Rx(BTL_DRIVER_SPI_USART);
}

void spi_write3Byte(uint32_t data)
{
  USART_Tx(BTL_DRIVER_SPI_USART, (data >> 16) & 0xFF);
  USART_Tx(BTL_DRIVER_SPI_USART, (data >> 8) & 0xFF);
  USART_Tx(BTL_DRIVER_SPI_USART, data & 0xFF);
  USART_Rx(BTL_DRIVER_SPI_USART);
  USART_Rx(BTL_DRIVER_SPI_USART);
  USART_Rx(BTL_DRIVER_SPI_USART);
}
/*
void spi_writeWord(uint32_t data)
{
  USART_Tx(BTL_DRIVER_SPI_USART, (data >> 24) & 0xFF);
  USART_Tx(BTL_DRIVER_SPI_USART, (data >> 16) & 0xFF);
  USART_Tx(BTL_DRIVER_SPI_USART, (data >> 8) & 0xFF);
  USART_Tx(BTL_DRIVER_SPI_USART, data & 0xFF);
  USART_Rx(BTL_DRIVER_SPI_USART);
  USART_Rx(BTL_DRIVER_SPI_USART);
  USART_Rx(BTL_DRIVER_SPI_USART);
  USART_Rx(BTL_DRIVER_SPI_USART);
}
*/
uint8_t spi_readByte(void)
{
  return USART_SpiTransfer(BTL_DRIVER_SPI_USART, 0xFF);
}

uint16_t spi_readHalfword(void)
{
  uint16_t retval = 0;
  USART_Tx(BTL_DRIVER_SPI_USART, 0xFF);
  USART_Tx(BTL_DRIVER_SPI_USART, 0xFF);
  retval = USART_Rx(BTL_DRIVER_SPI_USART) << 8;
  retval |= USART_Rx(BTL_DRIVER_SPI_USART);

  return retval;
}

void spi_setCsActive(void)
{
  GPIO_PinOutClear(BTL_DRIVER_SPI_CS_PORT, BTL_DRIVER_SPI_CS_PIN);
}

void spi_setCsInactive(void)
{
  GPIO_PinOutSet(BTL_DRIVER_SPI_CS_PORT, BTL_DRIVER_SPI_CS_PIN);
}
