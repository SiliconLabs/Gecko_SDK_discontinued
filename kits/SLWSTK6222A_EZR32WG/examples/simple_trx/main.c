/***************************************************************************//**
 * @file main.c
 * @brief EZRadio simple trx example
 *
 * This example shows how to easily implement a simple rtx code for your
 * controller using EZRadio or EZRadioPRO devices.
 *
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
#include "radio.h"

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include "bspconfig.h"
#include "image.h"

/* Display device */
DISPLAY_Device_t displayDevice;

/* Image widht and height definitions */
#define IMAGE_HIGHT     62u
#define BYTES_PER_LINE        ( LS013B7DH03_WIDTH / 8 )
#define BYTES_PER_FRAME       ( IMAGE_HIGHT * BYTES_PER_LINE )

/* Push button callback functionns. */
void GPIO_PB1_IRQHandler( uint8_t pin );
void GPIO_PB0_IRQHandler( uint8_t pin );

#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
/* sniprintf does not process floats, but occupy less flash memory ! */
#define snprintf    sniprintf
#endif

/* Defines the number of packets to send for one press of PB1.
 * Sends infinite number of packets if defined to 0xFFFF. */
#define APP_TX_PKT_SEND_NUM   0xFFFF

/* Data counter in transmitted packet */
uint16_t appDataCntr = 0;

/* Packet counter */
uint16_t appTxPktCntr = 0;

/* Received data */
uint16_t appRxData;

/* Sign tx active state */
bool     appTxActive = false;

/* RTC frequency */
#define APP_RTC_FREQ_HZ 4u

/* RTC set time is expired */
bool rtcTick = false;

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
void GPIO_PB0_IRQHandler( uint8_t pin )
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
void GPIO_PB1_IRQHandler( uint8_t pin )
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
  pFrame= (char *) image_bits;

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
 * @brief  Main function of clock example.
 *
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Configure HFRC frequency */
  CMU_HFRCOBandSet( cmuHFRCOBand_28MHz );

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
//  RtcInit();
  RTCDRV_Init();
  if (ECODE_EMDRV_RTCDRV_OK !=
      RTCDRV_AllocateTimer( &rtcTickTimer) )
  {
    while (1);
  }
  if (ECODE_EMDRV_RTCDRV_OK !=
      RTCDRV_StartTimer(rtcTickTimer, rtcdrvTimerTypePeriodic, APP_RTC_FREQ_HZ,
                        (RTCDRV_Callback_t)RTC_App_IRQHandler, NULL ) )
  {
    while (1);
  }

  /* Print header */
  printf("\n\n\n\n\n\n\n\n  EZRadio Simple TRx\n");

  /* Initialize EZRadio device. */
  ezradioInit();

  /* Print EZRadio device number. */
  ezradio_part_info();
  printf("   Device: Si%04x\n\n", ezradioReply.PART_INFO.PART);
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
  ezradioStartRX( 0 );

  /* Enter infinite loop that manages packet transmission and reception. */
  while (1)
  {
    /* Check is radio interrupt is received. */
    if (radioIrqReceived)
    {
      radioIrqReceived = false;

      /* Check if packet is sent */
      if (ezradioCheckTransmitted())
      {
        /* Sign tx passive state */
        appTxActive = false;

        /* Change to RX state */
        ezradioStartRX( 0 );
      }

      /* Check if packet is received */
      if (ezradioCheckReceived())
      {
        /* Read out and print received packet data */
        uint16_t rxData = *(uint16_t *)(radioPkt + APP_PKT_DATA_LOC);
        printf("Data RX: %05d\n", rxData);
      }

      /* Check if packet is received with CRC error */
      if (ezradioCheckCRCError())
      {
        printf("Pkt rxd - CRC Error\n");

        /* Change to RX state */
        ezradioStartRX( 0 );
      }
    }

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
          *(uint16_t *)(radioPkt + APP_PKT_DATA_LOC) = appDataCntr;

          /* Transmit packet */
          ezradioStartTx( 0, radioPkt);

          printf("Data TX: %05d\n", appDataCntr);

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
          printf("Data TX: need to wait\n");
        }
      }
    }
  }
}
