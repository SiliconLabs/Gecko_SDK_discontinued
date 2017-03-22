/***************************************************************************//**
 * @file main.c
 * @brief EZRadio trx example with auto acknowledge option enabled.
 *
 * This example shows how to easily implement a trx code with auto acknowledge
 * option for your controller using EZRadio or EZRadioPRO devices.
 *
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "spidrv.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "rtcdriver.h"

#include "ezradio_cmd.h"
#include "ezradio_api_lib.h"
#include "ezradio_plugin_manager.h"

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include "bspconfig.h"
#include "image.h"

/* Display device */
static DISPLAY_Device_t displayDevice;

/* Image widht and height definitions */
#define IMAGE_HIGHT           62u
#define BYTES_PER_LINE        ( LS013B7DH03_WIDTH / 8 )
#define BYTES_PER_FRAME       ( IMAGE_HIGHT * BYTES_PER_LINE )

/* Push button callback functionns. */
static void GPIO_PB1_IRQHandler( uint8_t pin );
static void GPIO_PB0_IRQHandler( uint8_t pin );

#if (defined EZRADIO_VARIABLE_DATA_START)
#define APP_PKT_DATA_START EZRADIO_VARIABLE_DATA_START
#else
#define APP_PKT_DATA_START 1u
#endif

static void appPacketTransmittedCallback ( EZRADIODRV_Handle_t handle, Ecode_t status );
static void appPacketReceivedCallback ( EZRADIODRV_Handle_t handle, Ecode_t status );
static void appPacketCrcErrorCallback ( EZRADIODRV_Handle_t handle, Ecode_t status );
static void appAutoAckTransmittedCallback ( EZRADIODRV_Handle_t handle, Ecode_t status );

#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
/* sniprintf does not process floats, but occupy less flash memory ! */
#define snprintf    sniprintf
#endif

/* Defines the number of packets to send for one press of PB1.
 * Sends infinite number of packets if defined to 0xFFFF. */
#define APP_TX_PKT_SEND_NUM   0xFFFF

/* Length of the actual data in the Tx packet */
#define APP_TX_PKT_DATA_LENGTH   2u

/* Length of the actual data in the ACK packet */
#define APP_AUTO_ACK_PKT_DATA_LENGTH   3u

/* Rx packet data array */
static uint8_t radioRxPkt[EZRADIO_FIFO_SIZE];

/* Tx packet data array, initialized with the default payload in the generated header file */
static uint8_t radioTxPkt[EZRADIO_FIFO_SIZE] = RADIO_CONFIG_DATA_CUSTOM_PAYLOAD;

/* Auto ack packet data array */
static uint8_t radioAutoAckPkt[RADIO_CONFIG_DATA_MAX_PACKET_LENGTH];

#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE == TRUE)
/* Default length configuration for normal transmission */
static EZRADIODRV_PacketLengthConfig_t radioTxLengthConf =
    { ezradiodrvTransmitLenghtDefault, RADIO_CONFIG_DATA_MAX_PACKET_LENGTH, RADIO_CONFIG_DATA_FIELD_LENGTH };
#endif //#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE == TRUE)

/* Data counter in transmitted packet */
static volatile uint16_t appDataCntr = 0;

/* Packet counter */
static volatile uint16_t appTxPktCntr = 0;

/* Sign tx active state */
static volatile bool appTxActive = false;

/* RTC frequency */
#define APP_RTC_FREQ_HZ 4u
/* RTC timeout */
#define APP_RTC_TIMEOUT_MS ( 1000u / APP_RTC_FREQ_HZ )

/* RTC set time is expired */
static volatile bool rtcTick = false;

/** Timer used to issue time elapsed interrupt. */
static RTCDRV_TimerID_t rtcTickTimer;
static RTCDRV_TimerID_t rtcRepeateTimer;


/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void GpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize GPIO interrupt */
  GPIOINT_Init();

  /* Configure PB0 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
  GPIOINT_CallbackRegister( BSP_GPIO_PB0_PIN, GPIO_PB0_IRQHandler );

  /* Configure PB1 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);
  GPIOINT_CallbackRegister( BSP_GPIO_PB1_PIN, GPIO_PB1_IRQHandler );
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB1)
 *        Switches between analog and digital clock modes.
 *****************************************************************************/
static void GPIO_PB0_IRQHandler( uint8_t pin )
{
  (void)pin;

  /* Check if already transmitting some packets,
   * send one otherwise. */
  if ( !appTxPktCntr )
  {
    appTxPktCntr += 1;
  }
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB0)
 *        Increments the time by one minute.
 *****************************************************************************/
static void GPIO_PB1_IRQHandler( uint8_t pin )
{
  (void)pin;

  /* Check if already transmitting some packets, stop them if so,
   * otherwise, send the APP_TX_PKT_SEND_NUM number of packets
   * (infinite is defined to 0xFFFF). */
  if (appTxPktCntr)
  {
    appTxPktCntr = 0;
  }
  else
  {
    appTxPktCntr += APP_TX_PKT_SEND_NUM;
  }
}

/**************************************************************************//**
 * @brief Draws Silicon Labs logo.
 *****************************************************************************/
void drawPicture( void )
{
  char *pFrame;

  /* Retrieve the properties of the display. */
  if ( DISPLAY_DeviceGet(0, &displayDevice) != DISPLAY_EMSTATUS_OK)
    while(1);

  /* Load pointer to picture buffor */
  pFrame = (char *) image_bits;

    /* Write to LCD */
  displayDevice.pPixelMatrixDraw( &displayDevice, pFrame,
                                  /* start coloumn, width */
                                  0, displayDevice.geometry.width,
                                  /* start row, height */
                                  0, IMAGE_HIGHT);
}

/**************************************************************************//**
 * @brief   Register a callback function to be called repeatedly at the
 *          specified frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] pParameter Pointer argument to be passed to the callback function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency is not supported.
 *****************************************************************************/
int RepeatCallbackRegister (void(*pFunction)(void*),
                            void* pParameter,
                            unsigned int frequency)
{

  if (ECODE_EMDRV_RTCDRV_OK ==
      RTCDRV_AllocateTimer( &rtcRepeateTimer))
  {
    if (ECODE_EMDRV_RTCDRV_OK ==
        RTCDRV_StartTimer(rtcRepeateTimer, rtcdrvTimerTypePeriodic, frequency,
          (RTCDRV_Callback_t)pFunction, pParameter ))
    {
      return 0;
    }
  }

  return -1;
}


/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB0)
 *        Increments the time by one minute.
 *****************************************************************************/
void RTC_App_IRQHandler()
{
  rtcTick = true;
}

/**************************************************************************//**
 * @brief  Main function of the example.
 *****************************************************************************/
int main(void)
{
  /* EZRadio driver init data and handler */
  EZRADIODRV_HandleData_t appRadioInitData = EZRADIODRV_INIT_DEFAULT;
  EZRADIODRV_Handle_t appRadioHandle = &appRadioInitData;

  /* EZRadio response structure union */
  ezradio_cmd_reply_t ezradioReply;

  /* Chip errata */
  CHIP_Init();

  /* HFXO 48MHz, divided by 1 */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Setup GPIO for pushbuttons. */
  GpioSetup();

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retarget stdio to the display. */
  if (TEXTDISPLAY_EMSTATUS_OK != RETARGET_TextDisplayInit())
  {
    /* Text display initialization failed. */
    while(1);
  }

  /* Set RTC to generate interrupt 250ms. */
  RTCDRV_Init();
  if (ECODE_EMDRV_RTCDRV_OK !=
      RTCDRV_AllocateTimer( &rtcTickTimer) )
  {
    while (1);
  }
  if (ECODE_EMDRV_RTCDRV_OK !=
      RTCDRV_StartTimer(rtcTickTimer, rtcdrvTimerTypePeriodic, APP_RTC_TIMEOUT_MS,
                        (RTCDRV_Callback_t)RTC_App_IRQHandler, NULL ) )
  {
    while (1);
  }

  /* Print header */
  printf("\n\n\n\n\n\n\n\n EZRadio TRx with ACK\n");

  /* Configure packet transmitted callback. */
  appRadioInitData.packetTx.userCallback = &appPacketTransmittedCallback;
#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE == TRUE)
  /* Store length for variable length packet */
  radioTxPkt[EZRADIO_LENGTH_WORD_START + RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE - 1] = APP_TX_PKT_DATA_LENGTH;

  /* Set the length of the variable length to the actual data */
#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 1)
  radioTxLengthConf.fieldLen.f1 = APP_TX_PKT_DATA_LENGTH;
#elif (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 2)
  radioTxLengthConf.fieldLen.f2 = APP_TX_PKT_DATA_LENGTH;
#elif (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 3)
  radioTxLengthConf.fieldLen.f3 = APP_TX_PKT_DATA_LENGTH;
#elif (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 4)
  radioTxLengthConf.fieldLen.f4 = APP_TX_PKT_DATA_LENGTH;
#elif (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 5)
  radioTxLengthConf.fieldLen.f5 = APP_TX_PKT_DATA_LENGTH;
#endif

#endif //#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE == TRUE)

  /* Configure packet received buffer and callback. */
  appRadioInitData.packetRx.userCallback = &appPacketReceivedCallback;
  appRadioInitData.packetRx.pktBuf = radioRxPkt;

  /* Configure packet received with CRC error callback. */
  appRadioInitData.packetCrcError.userCallback = &appPacketCrcErrorCallback;

  /* Configure auto ack packet buffer and callback. */
  appRadioInitData.autoAck.pktBuf = radioAutoAckPkt;
  appRadioInitData.autoAck.userCallback = &appAutoAckTransmittedCallback;

  /*Store auto ack packet data */
  radioAutoAckPkt[APP_PKT_DATA_START]     = 'A';
  radioAutoAckPkt[APP_PKT_DATA_START + 1] = 'C';
  radioAutoAckPkt[APP_PKT_DATA_START + 2] = 'K';

#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE == TRUE)
  /* Auto ack data length differs from the normal packet length, so handle that accordingly. */
  appRadioInitData.autoAck.pktMode = ezradiodrvAutoAckPktCustom;
  appRadioInitData.autoAck.lenConfig.lenMode = ezradiodrvTransmitLenghtCustomFieldLen;

  /* Store length for variable length packet */
  radioAutoAckPkt[EZRADIO_LENGTH_WORD_START + RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE - 1] = APP_AUTO_ACK_PKT_DATA_LENGTH;

  /* Set the length of the variable length to the auto ack data */
#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 1)
  appRadioInitData.autoAck.lenConfig.fieldLen.f1 = APP_AUTO_ACK_PKT_DATA_LENGTH;
#elif (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 2)
  appRadioInitData.autoAck.lenConfig.fieldLen.f2 = APP_AUTO_ACK_PKT_DATA_LENGTH;
#elif (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 3)
  appRadioInitData.autoAck.lenConfig.fieldLen.f3 = APP_AUTO_ACK_PKT_DATA_LENGTH;
#elif (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 4)
  appRadioInitData.autoAck.lenConfig.fieldLen.f4 = APP_AUTO_ACK_PKT_DATA_LENGTH;
#elif (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD == 5)
  appRadioInitData.autoAck.lenConfig.fieldLen.f5 = APP_AUTO_ACK_PKT_DATA_LENGTH;
#endif

#endif //#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE == TRUE)

  /* Initialize EZRadio device. */
  ezradioInit( appRadioHandle );

  /* Enable auto acknowledge feature. */
  ezradioEnableAutoAck( &(appRadioHandle->autoAck) );


  /* Print EZRadio device number. */
  ezradio_part_info(&ezradioReply);
  printf("   Device: Si%04x\n\n", ezradioReply.PART_INFO.PART);

  /* Print instructions. */
  printf(" Press PB0 to send\n  one packet.\n");
#if (APP_TX_PKT_SEND_NUM == 0xFFFF)
  printf(" Press PB1 to send\n  unlimited packets.\n");
#else
  printf(" Press PB1 to send\n  %d packets.\n", APP_TX_PKT_SEND_NUM);
#endif

  /* Draw logo */
  drawPicture();

  /* Reset radio fifos and start reception. */
  ezradioResetTRxFifo();
  ezradioStartRx( appRadioHandle );

  /* Enter infinite loop that will take care of ezradio plugin manager, packet transmission
   * and auto acknowledge. */
  while (1)
  {
    /* Run radio plug-in manager */
    ezradioPluginManager( appRadioHandle );

    if (rtcTick)
    {
      rtcTick = false;

      /* Send a packet if requested */
      if (appTxPktCntr)
      {
        /* Try to send the packet */
        if ( !appTxActive )
        {
          /* Sing tx active state */
          appTxActive = true;

          /* Add data cntr as the data to be sent to the packet */
          radioTxPkt[APP_PKT_DATA_START]   = (uint8_t)( ((uint16_t)appDataCntr) >> 8 );
          radioTxPkt[APP_PKT_DATA_START+1] = (uint8_t)( ((uint16_t)appDataCntr) & 0x00FF );

          /* Note: The following line issues the auto acknowledge feature to skip
           *       one session.
           *        - Should be used for links where both nodes transmits ACK packets,
           *          in order to skip auto ACK for received ACK packets.
           *        - Should be commented out if the receiver node does not send back
           *          auto acknowledge packets. */
          ezradioSkipAutoAck( &(appRadioHandle->autoAck) );

          /* Transmit packet */
#if (RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE == FALSE)
          ezradioStartTransmitConfigured( appRadioHandle, radioTxPkt );
#else
          ezradioStartTransmitCustom( appRadioHandle, radioTxLengthConf, radioTxPkt );
#endif

          printf("<--Data TX: %05d\n", appDataCntr);

          /* Increase data counter */
          appDataCntr++;

          /* Decrease number of requested packets,
           * if not configured to infinite. */
          if (appTxPktCntr != 0xFFFF)
          {
            /* Decrease request counter */
            if (appTxPktCntr)
            {
              appTxPktCntr--;
            }
          }
        }
        else
        {
          printf("---Data TX: need to wait\n");
        }
      }
    }
  }
}


/**************************************************************************//**
 * @brief  Packet transmitted callback of the application.
 *
 * @param[in] handle EzRadio plugin manager handler.
 * @param[in] status Callback status.
 *****************************************************************************/
static void appPacketTransmittedCallback ( EZRADIODRV_Handle_t handle, Ecode_t status )
{
  if ( status == ECODE_EMDRV_EZRADIODRV_OK )
  {
    /* Sign tx passive state */
    appTxActive = false;

    /* Change to RX state */
    ezradioStartRx( handle );
  }
}

/**************************************************************************//**
 * @brief  Packet received callback of the application.
 *
 * @param[in] handle EzRadio plugin manager handler.
 * @param[in] status Callback status.
 *****************************************************************************/
static void appPacketReceivedCallback ( EZRADIODRV_Handle_t handle, Ecode_t status )
{
  //Silent warning.
  (void)handle;

  if ( status == ECODE_EMDRV_EZRADIODRV_OK )
  {
    /* Read out and print received packet data:
     *  - print 'ACK' in case of ACK was received
     *  - print the data if some other data was received. */
    if ( (radioRxPkt[APP_PKT_DATA_START] == 'A') &&
         (radioRxPkt[APP_PKT_DATA_START + 1] == 'C') &&
         (radioRxPkt[APP_PKT_DATA_START + 2] == 'K') )
    {
      printf("-->Data RX: ACK\n");
    }
    else
    {
      uint16_t rxData;

      rxData =  (uint16_t)(radioRxPkt[APP_PKT_DATA_START]) << 8;
      rxData += (uint16_t)(radioRxPkt[APP_PKT_DATA_START+1]);

      printf("-->Data RX: %05d\n", rxData);
    }
  }
}

/**************************************************************************//**
 * @brief  Packet received with CRC error callback of the application.
 *
 * @param[in] handle EzRadio plugin manager handler.
 * @param[in] status Callback status.
 *****************************************************************************/
static void appPacketCrcErrorCallback ( EZRADIODRV_Handle_t handle, Ecode_t status )
{
  if ( status == ECODE_EMDRV_EZRADIODRV_OK )
  {
    printf("-->Pkt  RX: CRC Error\n");

    /* Change to RX state */
    ezradioStartRx( handle );
  }
}

/**************************************************************************//**
 * @brief  Packet transmitted callback of the application.
 *
 * @param[in] handle EzRadio plugin manager handler.
 * @param[in] status Callback status.
 *****************************************************************************/
static void appAutoAckTransmittedCallback ( EZRADIODRV_Handle_t handle, Ecode_t status )
{
  if ( status == ECODE_EMDRV_EZRADIODRV_OK )
  {
    printf("<--Data TX: ACK \n");

    ezradioStartRx( handle );
  }
}
