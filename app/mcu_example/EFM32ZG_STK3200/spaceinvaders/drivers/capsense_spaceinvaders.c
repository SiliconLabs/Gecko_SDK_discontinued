/**************************************************************************//**
 * @file
 * @brief Capacitive sense driver
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


/* EM header files */
#include "em_device.h"

/* Drivers */
#include "capsense_spaceinvaders.h"
#include "em_emu.h"
#include "em_acmp.h"
#include "em_cmu.h"
#include "capsenseconfig_spaceinvaders.h"

/** The current channel we are sensing */
static volatile uint8_t currentChannel;

/**************************************************************************//**
 * @brief This vector stores the latest read values from the ACMP
 * @param ACMP_CHANNELS Vector of channels.
 *****************************************************************************/
static volatile uint32_t channelValues[ACMP_CHANNELS] = { 0, 0, 0, 0, 0};

/**************************************************************************//**
 * @brief  This stores the maximum values seen by a channel
 * @param ACMP_CHANNELS Vector of channels.
 *****************************************************************************/
static volatile uint32_t channelMaxValues[ACMP_CHANNELS] = { 1, 1, 1, 1, 1};


/**************************************************************************//**
 * @brief CAPSENSE timer callback.
 *        When SysTick expires the number of pulses on TIMER1 is inserted into
 *        channelValues. If this values is bigger than what is recorded in
 *        channelMaxValues, channelMaxValues is updated.
 *        Finally, the next ACMP channel is selected.
 *****************************************************************************/
void CAPSENSE_timerEventHandler(void)
{
  uint32_t count;
  uint8_t ch;

  /* Stop timers */
  TIMER1->CMD = TIMER_CMD_STOP;

  /* Read out value of TIMER1 */
  count = TIMER1->CNT;

  /* Store value in channelValues */
  channelValues[currentChannel] = count;

  /* Update channelMaxValues */
  if (count > channelMaxValues[currentChannel])
    channelMaxValues[currentChannel] = count;

  /* Select next channel */
  if (currentChannel == BUTTON0_CHANNEL)
  {
    currentChannel = BUTTON1_CHANNEL;
  }
  else
  {
    currentChannel = BUTTON0_CHANNEL;
  }

  /* Set up this channel in the ACMP. */
  ch = currentChannel;
  ACMP_CapsenseChannelSet(ACMP_CAPSENSE, (ACMP_Channel_TypeDef) ch);

  /* Reset timer */
  TIMER1->CNT = 0;

  /* Start timer */
  TIMER1->CMD = TIMER_CMD_START;

}


/**************************************************************************//**
 * @brief Get the current channelValue for a channel
 * @param channel The channel.
 * @return The channelValue.
 *****************************************************************************/
uint32_t CAPSENSE_getVal(uint8_t channel)
{
  return channelValues[channel];
}

/**************************************************************************//**
 * @brief Get the channelValue for a sensor segment
 * @return channel
 *****************************************************************************/
uint8_t  CAPSENSE_getButton0Channel(void)
{
  uint8_t channel = BUTTON0_CHANNEL;
  return channel;

}

/**************************************************************************//**
 * @brief Get the channelValue for a sensor segment
 * @return channel
 *****************************************************************************/
uint8_t  CAPSENSE_getButton1Channel(void)
{
  uint8_t channel = BUTTON1_CHANNEL;
  return channel;

}


/**************************************************************************//**
 * @brief Get the current normalized channelValue for a channel
 * @param channel The channel.
 * @return The channel value in range (0-256).
 *****************************************************************************/
uint32_t CAPSENSE_getNormalizedVal(uint8_t channel)
{
  uint32_t max = channelMaxValues[channel];
  return (channelValues[channel] << 8) / max;
}

/**************************************************************************//**
 * @brief Get the state of the Gecko Button
 * @param channel The channel.
 * @return true if the button is "pressed"
 *         false otherwise.
 *****************************************************************************/
bool CAPSENSE_getPressed(uint8_t channel)
{
  uint32_t treshold;
  /* Treshold is set to 25% below the maximum value */
  /* This calculation is performed in two steps because channelMaxValues is
   * volatile. */
  treshold  = channelMaxValues[channel];
  treshold -= channelMaxValues[channel] >> 2;

  if (channelValues[channel] < treshold)
  {
    return true;
  }
  return false;
}

/**************************************************************************//**
 * @brief Get the position of the slider
 * @return The position of the slider if it can be determined,
 *         -1 otherwise.
 *****************************************************************************/
int32_t CAPSENSE_getSliderPosition(void)
{
  int      i;
  int      minPos = -1;
  uint32_t minVal = 224; /* 0.875 * 256 */
  /* Values used for interpolation. There is two more which represents the edges.
   * This makes the interpolation code a bit cleaner as we do not have to make special
   * cases for handling them */
  uint32_t interpol[6] = { 255, 255, 255, 255, 255, 255 };

  /* The calculated slider position. */
  int position;

  /* Iterate through the 4 slider bars and calculate the current value divided by
   * the maximum value multiplied by 256.
   * Note that there is an offset of 1 between channelValues and interpol.
   * This is done to make interpolation easier.
   */
  for (i = 1; i < 5; i++)
  {
    /* interpol[i] will be in the range 0-256 depending on channelMax */
    interpol[i]  = channelValues[i - 1] << 8;
    interpol[i] /= channelMaxValues[i - 1];
    /* Find the minimum value and position */
    if (interpol[i] < minVal)
    {
      minVal = interpol[i];
      minPos = i;
    }
  }
  /* Check if the slider has not been touched */
  if (minPos == -1)
    return -1;

  /* Start position. Shift by 4 to get additional resolution. */
  /* Because of the interpol trick earlier we have to substract one to offset that effect */
  position = (minPos - 1) << 4;

  /* Interpolate with pad to the left */
  position -= ((256 - interpol[minPos - 1]) << 3)
              / (256 - interpol[minPos]);

  /* Interpolate with pad to the right */
  position += ((256 - interpol[minPos + 1]) << 3)
              / (256 - interpol[minPos]);

  return position;
}

/**************************************************************************//**
 * @brief Initializes the capacative sense system.
 *        Capacative sensing uses two timers: TIMER0 and TIMER1 as well as ACMP.
 *        ACMP is set up in cap-sense (oscialltor mode).
 *        TIMER1 counts the number of pulses generated by ACMP_CAPSENSE.
 *        When TIMER0 expires it generates an interrupt.
 *        The number of pulses counted by TIMER0 is then stored in channelValues
 *****************************************************************************/
void CAPSENSE_Init(void)
{
  /* Use the default STK capacative sensing setup */
  ACMP_CapsenseInit_TypeDef capsenseInit = ACMP_CAPSENSE_INIT_DEFAULT;

  /* Enable TIMER0, TIMER1, ACMP_CAPSENSE and PRS clock */
  CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER1
                      | ACMP_CAPSENSE_CLKEN
                      | CMU_HFPERCLKEN0_PRS;

  /* Initialize TIMER1 - Prescaler 2^10, clock source CC1, top value 0xFFFF */
  TIMER1->CTRL = TIMER_CTRL_PRESC_DIV1024 | TIMER_CTRL_CLKSEL_CC1;
  TIMER1->TOP  = 0xFFFF;

  /*Set up TIMER1 CC1 to trigger on PRS channel */
  TIMER1->CC[1].CTRL = TIMER_CC_CTRL_MODE_INPUTCAPTURE /* Input capture      */
                       | TIMER_CC_PRS_CH_CAPSENSE      /* PRS channel defined in config */
                       | TIMER_CC_CTRL_INSEL_PRS       /* PRS input selected */
                       | TIMER_CC_CTRL_ICEDGE_RISING;    /* PRS on rising edge */

  /*Set up PRS channel to trigger on ACMP output*/
  PRS->CH[PRS_CH_CAPSENSE].CTRL =   PRS_CH_CTRL_SOURCESEL_ACMP_CAPSENSE      /* PRS source */
                                   | PRS_CH_CTRL_SIGSEL_ACMPOUT_CAPSENSE;    /* PRS source */

  /* Set up ACMP1 in capsense mode */
  capsenseInit.fullBias = true;
  capsenseInit.halfBias = true;
  capsenseInit.vddLevel = 0x30;
  ACMP_CapsenseInit(ACMP_CAPSENSE, &capsenseInit);

  /* Use the default STK capacative sensing setup and enable it */
  ACMP_Enable(ACMP_CAPSENSE);



}
