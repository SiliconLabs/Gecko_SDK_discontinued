/**************************************************************************//**
 * @file ezradio_hal.c
 * @brief This file contains EZRadio HAL.
 * @version 4.0.0
 ******************************************************************************
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

#include <stdlib.h>
#include <stdint.h>
#include "ezradiodrv_config.h"

#include "em_gpio.h"

#if (defined EZRADIODRV_USE_SPIDRV)
#include "spidrv.h"
#else //EZRADIO_USE_SPIDRV
#include "em_cmu.h"
#include "em_usart.h"
#endif //EZRADIO_USE_SPIDRV

#include "gpiointerrupt.h"
#include "ezradio_hal.h"

#if (defined EZRADIODRV_USE_SPIDRV)
SPIDRV_HandleData_t  ezradioSpiHandle;
SPIDRV_Handle_t      ezradioSpiHandlePtr = &ezradioSpiHandle;
SPIDRV_Init_t        ezradioSpiInitData = SPIDRV_MASTER_USART1;
#endif //EZRADIO_USE_SPIDRV

/*!
 * Configures the EZRadio GPIO port and pins
 *
 * @param radioIrqCallback EZRadio interrupt callback configuration
 */
void ezradio_hal_GpioInit( GPIOINT_IrqCallbackPtr_t ezradioIrqCallback )
{

#if !(defined EZRADIODRV_USE_SPIDRV)
   /* Ensure gpio clock is set up */
   CMU_ClockEnable( cmuClock_GPIO, true );

   /* Setup pins used for SPI communication to radio */
   GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_USARTRF_MOSI_PORT, RF_USARTRF_MOSI_PIN, gpioModePushPull, 0 );
   GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_USARTRF_MISO_PORT, RF_USARTRF_MISO_PIN, gpioModeInput,    0 );
   GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_USARTRF_CLK_PORT,  RF_USARTRF_CLK_PIN,  gpioModePushPull, 0 );
#endif // !EZRADIO_USE_SPIDRV

   GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_USARTRF_CS_PORT,   RF_USARTRF_CS_PIN,   gpioModePushPull, 1 );

   /* Setup enable and interrupt pins to radio */
   GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_SDN_PORT,  RF_SDN_PIN,   gpioModePushPull, 0 );
   GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN, gpioModeInputPull, 1 );

   if (NULL != ezradioIrqCallback)
   {
       /* Register callback and enable interrupt */
       GPIOINT_CallbackRegister( RF_INT_PIN, ezradioIrqCallback );
       GPIO_IntConfig( (GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN, false, true, true );
   }

   return;
}

void ezradio_hal_SpiInit( void )
{

#if !(defined EZRADIODRV_USE_SPIDRV)
   USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;


   /* Enable USART clock */
   CMU_ClockEnable( cmuClock_USARTRF0, true );

   init.baudrate     = 1000000;
   init.databits     = usartDatabits8;
   init.msbf         = 1;
   init.master       = 1;
   init.clockMode    = usartClockMode0;
   init.prsRxEnable  = 0;
   init.autoTx       = 0;

   USART_InitSync(USARTRF0, &init);

   /* Enable output pins and set location */
   USARTRF0->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN;

   ezradio_hal_SetNsel();
#else // !EZRADIO_USE_SPIDRV
   ezradioSpiInitData.port         = USARTRF0;
   ezradioSpiInitData.portLocation = RF_USARTRF_CLK_PORT;
   ezradioSpiInitData.csControl    = spidrvCsControlApplication;
   SPIDRV_Init( ezradioSpiHandlePtr, &ezradioSpiInitData );

#endif // !EZRADIO_USE_SPIDRV

   return;
}

/*!
 * Asserts SDN pin of the EZRadio device.
 */
void ezradio_hal_AssertShutdown(void)
{
  GPIO_PinOutSet( (GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN);
}

/*!
 * Deasserts SDN pin of the EZRadio device.
 */
void ezradio_hal_DeassertShutdown(void)
{
  GPIO_PinOutClear( (GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN);
}

/*!
 * Clears nSEL pin of the EZRadio device.
 */
void ezradio_hal_ClearNsel(void)
{
  GPIO_PinOutClear( (GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN);
}

/*!
 * Sets nSEL pin of the EZRadio device.
 */
void ezradio_hal_SetNsel(void)
{
  GPIO_PinOutSet( (GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN);
}

/*!
 * Reads nIRQ pin of the EZRadio device.
 *
 * @return Value of nIRQ pin.
 */
uint8_t ezradio_hal_NirqLevel(void)
{
  return GPIO_PinInGet( (GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN);
}

/*!
 * Writes a single byte to the EZRadio SPI port.
 *
 * @param byteToWrite Byte to write.
 */
void ezradio_hal_SpiWriteByte(uint8_t byteToWrite)
{
#if (defined EZRADIODRV_USE_SPIDRV)
  SPIDRV_MTransmitB(ezradioSpiHandlePtr, &byteToWrite, 1);
#else //EZRADIO_USE_SPIDRV
  USART_SpiTransfer(USARTRF0, byteToWrite);
#endif //EZRADIO_USE_SPIDRV
}

/*!
 * Reads a single byte from the EZRadio SPI port.
 *
 * @param readByte Read byte.
 */
void ezradio_hal_SpiReadByte(uint8_t* readByte)
{

#if (defined EZRADIODRV_USE_SPIDRV)
  SPIDRV_MReceiveB(ezradioSpiHandlePtr, readByte, 1);
#else  //EZRADIO_USE_SPIDRV
  *readByte = USART_SpiTransfer(USARTRF0, 0xFF);
#endif //EZRADIO_USE_SPIDRV
}

/*!
 * Writes byteCount number of bytes to the EZRadio SPI port.
 *
 * @param byteCount Number of bytes to write.
 * @param pData Pointer to the byte array.
 */
void ezradio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData)
{

#if (defined EZRADIODRV_USE_SPIDRV)
  SPIDRV_MTransmitB(ezradioSpiHandlePtr, pData, byteCount);
#else //EZRADIO_USE_SPIDRV
  for (; byteCount; byteCount--)
  {
    USART_SpiTransfer(USARTRF0, *pData);
    pData++;
  }
#endif //EZRADIO_USE_SPIDRV
}

/*!
 * Reads byteCount number of bytes from the EZRadio SPI port.
 *
 * @param byteCount Number of bytes to write.
 * @param pData Pointer to the byte array.
 */
void ezradio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData)
{

#if (defined EZRADIODRV_USE_SPIDRV)
  SPIDRV_MReceiveB(ezradioSpiHandlePtr, pData, byteCount);
#else //EZRADIO_USE_SPIDRV
  for (; byteCount; byteCount--)
  {
    *pData = USART_SpiTransfer(USARTRF0, 0xFF);
    pData++;
  }
#endif //EZRADIO_USE_SPIDRV

}
