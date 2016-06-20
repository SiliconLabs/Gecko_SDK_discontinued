/***************************************************************************//**
 * @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 * @version 4.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
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
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "em_gpio.h"
#include "em_prs.h"
#include "em_cmu.h"
#include "gpiointerrupt.h"

#include "radio.h"
#include "ezradio_cmd.h"
#include "ezradio_prop.h"
#include "ezradio_hal.h"
#include "ezradio_api_lib.h"
#include "ezradio_api_lib_add.h"

/* Radio configuration data array. */
const uint8_t Radio_Configuration_Data_Array[]  = \
                        RADIO_CONFIGURATION_DATA_ARRAY;

/* Radio and application configuration data array. */
const tRadioConfiguration RadioConfiguration = \
                        RADIO_CONFIGURATION_DATA;

/* Pointer to the radio cofiguration data array. */
const tRadioConfiguration * const pRadioConfiguration = \
                        &RadioConfiguration;

/* Packet data array */
uint8_t radioPkt[RADIO_MAX_PACKET_LENGTH];

/* Radio interrupt receive flag */
bool    radioIrqReceived = false;

void ezradioPowerUp(void);



/*!
 *  Radio nIRQ GPIO interrupt.
 *  @note
 */
void GPIO_EZRadio_INT_IRQHandler( uint8_t pin )
{
  (void)pin;

  /* Sign radio interrupt received */
  radioIrqReceived = true;
}

/*!
 *  Power up the Radio.
 *  @note
 */
void ezradioPowerUp(void)
{
  volatile uint16_t wDelay = 0u;

  /* Hardware reset the chip */
  ezradio_reset();

  /* Wait until reset timeout or Reset IT signal */
  for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
}

/*!
 *  Radio Initialization.
 *  @note
 *  @author Sz. Papp
 */
void ezradioInit(void)
{
  uint16_t wDelay;
  uint8_t  pktPosCnt;

  /* Initialize radio GPIOs and SPI port */
  ezradio_hal_GpioInit( GPIO_EZRadio_INT_IRQHandler );

  /* Additional GPIO init to support PTI for the current release */

  /* Setup PRS for PTI pins */
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Enable RF_GPIO0 and RF_GPIO1 pins. */
  GPIO_PinModeSet((GPIO_Port_TypeDef)RF_GPIO0_PORT, RF_GPIO0_PIN,
                  gpioModeInput, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)RF_GPIO1_PORT, RF_GPIO1_PIN,
                  gpioModeInput, 0);

  /* Pin PA0 and PA1 output the GPIO0 and GPIO1 via PRS to PTI */
  GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 0);

  /* Configure INT/PRS channels */
  GPIO_IntConfig((GPIO_Port_TypeDef)RF_GPIO0_PORT, RF_GPIO0_PIN,
                 false, false, false);
  GPIO_IntConfig((GPIO_Port_TypeDef)RF_GPIO1_PORT, RF_GPIO1_PIN,
                 false, false, false);

  /* Setup PRS */
  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN15);
  PRS_SourceAsyncSignalSet(1, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN14);
  PRS->ROUTE = (PRS_ROUTE_CH0PEN | PRS_ROUTE_CH1PEN);

  /* Make sure PRS sensing is enabled (should be by default) */
  GPIO_InputSenseSet(GPIO_INSENSE_PRS, GPIO_INSENSE_PRS);


  ezradio_hal_SpiInit();

  /* Power Up the radio chip */
  ezradioPowerUp();

  /* Load radio configuration */
  while (EZRADIO_CONFIG_SUCCESS!= ezradio_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
  {
    /* Error hook */
#ifdef ERROR_HOOK
    ERROR_HOOK;
#else
    printf("ERROR: Radio configuration failed!\n");
#endif
    for (wDelay = 0x7FFF; wDelay--; ) ;

    /* Power Up the radio chip */
    ezradioPowerUp();
  }

  /* Read ITs, clear pending ones */
  ezradio_get_int_status(0u, 0u, 0u);

  /* Initialize packet content */
  for(pktPosCnt = 0u; pktPosCnt < pRadioConfiguration->Radio_PacketLength; pktPosCnt++)
  {
    radioPkt[pktPosCnt] = pRadioConfiguration->Radio_CustomPayload[pktPosCnt];
  }

}

/*!
 *  Check if Packet received IT flag is pending.
 *  @return   true - Packet successfully received / false - No packet pending.
 *  @note
 */
bool ezradioCheckReceived(void)
{
  if (false == ezradio_hal_NirqLevel())
  {
    /* Read ITs, clear pending ones */
    ezradio_get_int_status(~(EZRADIO_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT), 0u, 0u);

    /* check the reason for the IT */
    if (ezradioReply.GET_INT_STATUS.PH_PEND & EZRADIO_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT)
    {
      /* Packet RX */
      ezradio_read_rx_fifo(RadioConfiguration.Radio_PacketLength, (uint8_t *) &radioPkt[0u]);

      return true;
    }

    /* Reset FIFO */
    ezradio_fifo_info(EZRADIO_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
  }

  return false;
}

/*!
 *  Check if Packet CRC error IT flag is pending.
 *  @return   true - Packet CRC error / false - No error.
 *  @note
 */
bool ezradioCheckCRCError(void)
{
  if (false == ezradio_hal_NirqLevel())
  {
    /* Read ITs, clear pending ones */
    ezradio_get_int_status(~(EZRADIO_CMD_GET_INT_STATUS_ARG_PH_CLR_PEND_CRC_ERROR_PEND_CLR_MASK), 0u, 0u);

    /* check the reason for the IT */
    if (ezradioReply.GET_INT_STATUS.PH_PEND & EZRADIO_CMD_GET_INT_STATUS_REP_PH_PEND_CRC_ERROR_PEND_BIT)
    {
      /* Reset FIFO */
      ezradio_change_state(EZRADIO_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_READY);
      ezradio_fifo_info(EZRADIO_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);
      return true;
    }
  }
  return false;
}

/*!
 *  Check if Packet sent IT flag is pending.
 *
 *  @return   true / false
 *
 *  @note
 *
 */
bool ezradioCheckTransmitted(void)
{
  if (false == ezradio_hal_NirqLevel())
  {
    /* Read ITs, clear pending ones */
    ezradio_get_int_status(~(EZRADIO_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT), 0u, 0u);

    /* check the reason for the IT */
    if (ezradioReply.GET_INT_STATUS.PH_PEND & EZRADIO_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT)
    {
      return true;
    }
  }

  return false;
}

/*!
 *  Set Radio to RX mode, fixed packet length.
 *
 *  @param channel Freq. Channel
 *
 *  @note
 *
 */
void ezradioStartRX(uint8_t channel)
{
  /* Start Receiving packet, channel 0, START immediately, Packet n bytes long */
    ezradio_start_rx(channel, 0u, 0u,
                  EZRADIO_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  EZRADIO_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_RX,
                  EZRADIO_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_RX );
}

/*!
 *  Set Radio to TX mode, fixed packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent
 *
 *  @return true if packet is issued to transmit
 *
 */
bool ezradioStartTx(uint8_t channel, uint8_t *pioRadioPacket)
{
  ezradio_request_device_state();

  if (ezradioReply.REQUEST_DEVICE_STATE.CURR_STATE == EZRADIO_CMD_REQUEST_DEVICE_STATE_REP_CURR_STATE_MAIN_STATE_ENUM_TX) {
    return false;
  }

  /* Fill the TX fifo with datas */
  ezradio_write_tx_fifo(RadioConfiguration.Radio_PacketLength, pioRadioPacket);

  /* Start sending packet, channel 0, START immediately, Packet n bytes long, go READY when done */
  ezradio_start_tx(channel, 0x30,  0u);

  return true;
}

/*!
 *  Resets both the TX and RX FIFOs.
 *  @note
 */
void ezradioResetTRxFifo(void)
{
  ezradio_fifo_info(EZRADIO_CMD_FIFO_INFO_ARG_FIFO_RX_BIT | EZRADIO_CMD_FIFO_INFO_ARG_FIFO_TX_BIT);
}
