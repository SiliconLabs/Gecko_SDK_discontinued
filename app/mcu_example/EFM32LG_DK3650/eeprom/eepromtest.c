/**************************************************************************//**
 * @file
 * @brief EEPROM example for EFM32LG_DK3650
 * @details
 *   Read/write data to EEPROM on DK.
 *
 * @par Usage
 * @li Joystick Up/Down increases/decreases data stored in first 3 bytes.
 *       of EEPROM.
 *
 * @note
 *   This example requires BSP version 1.0.6 or later.
 *
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
#include "em_emu.h"
#include "em_dbg.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "i2cspm.h"
#include "eeprom.h"
#include "retargettft.h"

/** Byte stored in first 3 bytes of EEPROM */
static volatile uint8_t eepromData;

/** Reset first 3 bytes of EEPROM to 0xFF */
static volatile bool eepromReset = false;

/* Local prototypes */
void eepromtestIRQInit(void);
void eepromtestUpdateLCD(uint8_t *data);

/**************************************************************************//**
 * @brief GPIO Interrupt handler
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint16_t flags;
  uint16_t joystick;
  uint16_t pb;

  /* Clear interrupt */
  flags = BSP_InterruptFlagsGet();
  GPIO_IntClear(1 << BSP_GPIO_INT_PIN);
  BSP_InterruptFlagsClear(flags);

  if (flags & BC_INTFLAG_JOYSTICK)
  {
    /* LEDs on to indicate joystick used */
    BSP_LedsSet(0xffff);

    /* Read and store joystick activity */
    joystick = BSP_JoystickGet();

    /* Up increases data to store in EEPROM */
    if (joystick & BC_UIF_JOYSTICK_UP)
    {
      eepromData++;
      while(BSP_JoystickGet() & BC_UIF_JOYSTICK_UP);
    }

    /* Down decreases data to store in EEPROM */
    if (joystick & BC_UIF_JOYSTICK_DOWN)
    {
      eepromData--;
      while(BSP_JoystickGet() & BC_UIF_JOYSTICK_DOWN);
    }
    /* LEDs off to indicate joystick release */
    BSP_LedsSet(0x0000);
  }

  if (flags & BC_INTFLAG_PB)
  {
    /* Read and store push button activity */
    pb = BSP_PushButtonsGet();

    /* Reset modified data to factory default */
    if (pb & BC_UIF_PB4)
    {
      eepromReset = true;
      while(BSP_PushButtonsGet() & BC_UIF_PB4);
    }
  }
}


/**************************************************************************//**
 * @brief Initialize GPIO interrupt for joystick and button
 *****************************************************************************/
void eepromtestIRQInit(void)
{
  /* Disable and clear interrupts in the board controller */
  BSP_InterruptDisable(0xffff);
  BSP_InterruptFlagsClear(0xffff);

  /* Configure interrupt pin as input with pull-up */
  GPIO_PinModeSet(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, gpioModeInputPull, 1);

  /* Set falling edge interrupt and clear/enable it */
  GPIO_IntConfig(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  /* Enable board controller interrupts */
  BSP_InterruptEnable(BC_INTEN_JOYSTICK | BC_INTEN_PB);
}


/**************************************************************************//**
 * @brief Update LCD with data stored
 * @param[in] data Data to dispaly in hex.
 *****************************************************************************/
void eepromtestUpdateLCD(uint8_t *data)
{
  printf("0x%02X 0x%02X 0x%02X\n", data[0], data[1], data[2]);
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
  uint8_t             data[3];

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize the TFT stdio retarget module. */
  RETARGET_TftInit();
  printf("\nEFM32 I2C EEPROM example\n\n");

  /* Enable board control interrupts */
  eepromtestIRQInit();

#if !defined( BSP_STK )
  BSP_PeripheralAccess(BSP_I2C, true);
#endif

  /* Initialize I2C driver, using standard rate. Devices on DK itself */
  /* supports fast mode, but in case some slower devices are added on */
  /* prototype board, we use standard mode. */
  I2CSPM_Init(&i2cInit);

  if (EEPROM_Read(i2cInit.port, EEPROM_DVK_ADDR, 0, data, 3) < 0)
  {
    printf("RD ERR\n");
    /* Enter EM2, no wakeup scheduled */
    EMU_EnterEM2(true);
  }
  eepromtestUpdateLCD(data);

  /* Main loop - just read data and update LCD */
  while (1)
  {
    /* Should data be reset to factory default? */
    if (eepromReset)
    {
      eepromReset = false;

      /* Data changed by user? */
      data[0] = 0xFF;
      data[1] = 0xFF;
      data[2] = 0xFF;
      if (EEPROM_Write(i2cInit.port, EEPROM_DVK_ADDR, 0, data, 3) < 0)
      {
        printf("RST ERR\n");
        /* Enter EM2, no wakeup scheduled */
        EMU_EnterEM2(true);
      }
      eepromtestUpdateLCD(data);
    }

    if (EEPROM_Read(i2cInit.port, EEPROM_DVK_ADDR, 0, data, 3) < 0)
    {
      printf("RD ERR\n");
      /* Enter EM2, no wakeup scheduled */
      EMU_EnterEM2(true);
    }

    eepromData = data[0];

    /* Just enter EM2 until joystick pressed */
    EMU_EnterEM2(true);

    /* Data changed by user? */
    if (eepromData != data[0])
    {
      data[0] = eepromData;
      data[1] = eepromData + 1;
      data[2] = eepromData + 2;
      eepromtestUpdateLCD(data);
      if (EEPROM_Write(i2cInit.port, EEPROM_DVK_ADDR, 0, data, 3) < 0)
      {
        printf("WR ERR\n");
        /* Enter EM2, no wakeup scheduled */
        EMU_EnterEM2(true);
      }
    }
  }
}
