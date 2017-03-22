/***************************************************************************//**
 * @file fem-control.c
 * @brief This file implements a simple API for configuring FEM control signals via PRS.
 * @copyright Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "hal/micro/cortexm3/efm32/prs.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"

#define PRS_CHANNEL_RX  EMBER_AF_PLUGIN_FEM_CONTROL_RX_CHANNEL
#define PRS_LOCATION_RX  EMBER_AF_PLUGIN_FEM_CONTROL_RX_LOCATION
#ifdef EMBER_AF_PLUGIN_FEM_CONTROL_RX_ACTIVE
 #define EMBER_AF_PLUGIN_FEM_CONTROL_RX_ACTIVE 1
#else
 #define EMBER_AF_PLUGIN_FEM_CONTROL_RX_ACTIVE 0
#endif
#define ACTIVE_RX  EMBER_AF_PLUGIN_FEM_CONTROL_RX_ACTIVE
#ifdef  EMBER_AF_PLUGIN_FEM_CONTROL_RX_CHANNEL_INVERSION
  #define EMBER_AF_PLUGIN_FEM_CONTROL_RX_CHANNEL_INVERSION 1
#else
  #define EMBER_AF_PLUGIN_FEM_CONTROL_RX_CHANNEL_INVERSION 0 
#endif

#define PRS_CHANNEL_TX  EMBER_AF_PLUGIN_FEM_CONTROL_TX_CHANNEL
#define PRS_LOCATION_TX  EMBER_AF_PLUGIN_FEM_CONTROL_TX_LOCATION
#ifdef  EMBER_AF_PLUGIN_FEM_CONTROL_TX_ACTIVE
 #define EMBER_AF_PLUGIN_FEM_CONTROL_TX_ACTIVE 1
#else
 #define EMBER_AF_PLUGIN_FEM_CONTROL_TX_ACTIVE 0
#endif
#ifdef  EMBER_AF_PLUGIN_FEM_CONTROL_TX_CHANNEL_INVERSION
  #define EMBER_AF_PLUGIN_FEM_CONTROL_TX_CHANNEL_INVERSION 1
#else
  #define EMBER_AF_PLUGIN_FEM_CONTROL_TX_CHANNEL_INVERSION 0 
#endif

#if EMBER_AF_PLUGIN_FEM_CONTROL_RX_ACTIVE == 1 && EMBER_AF_PLUGIN_FEM_CONTROL_TX_ACTIVE==1
  #if PRS_CHANNEL_RX == PRS_CHANNEL_TX
    #error The RX and TX PRS channels cannot be equal.
  #endif 
#endif

#define _PRS_CH_CTRL_SOURCESEL_RAC 0x20
#define _PRS_CH_CTRL_SIGSEL_RACPAEN 0x04
#define _PRS_CH_CTRL_SIGSEL_RACLNAEN 0x03

#define AF_PRS_CHX_PIN(x,i)        ((x) == 0 ? AF_PRS_CH0_PIN(i) : (x) == 1 ? AF_PRS_CH1_PIN(i) : (x) == 2 ? AF_PRS_CH2_PIN(i) : (x) == 3 ? AF_PRS_CH3_PIN(i) : (x) == 4 ? AF_PRS_CH4_PIN(i) : (x) == 5 ? AF_PRS_CH5_PIN(i) : (x) == 6 ? AF_PRS_CH6_PIN(i) : (x) == 7 ? AF_PRS_CH7_PIN(i) : (x) == 8 ? AF_PRS_CH8_PIN(i) : (x) == 9 ? AF_PRS_CH9_PIN(i) : (x) == 10 ? AF_PRS_CH10_PIN(i) : (x) == 11 ? AF_PRS_CH11_PIN(i) : -1)
#define AF_PRS_CHX_PORT(x,i)        ((x) == 0 ? AF_PRS_CH0_PORT(i) : (x) == 1 ? AF_PRS_CH1_PORT(i) : (x) == 2 ? AF_PRS_CH2_PORT(i) : (x) == 3 ? AF_PRS_CH3_PORT(i) : (x) == 4 ? AF_PRS_CH4_PORT(i) : (x) == 5 ? AF_PRS_CH5_PORT(i) : (x) == 6 ? AF_PRS_CH6_PORT(i) : (x) == 7 ? AF_PRS_CH7_PORT(i) : (x) == 8 ? AF_PRS_CH8_PORT(i) : (x) == 9 ? AF_PRS_CH9_PORT(i) : (x) == 10 ? AF_PRS_CH10_PORT(i) : (x) == 11 ? AF_PRS_CH11_PORT(i) : -1)


void emberAfPluginFemControlInitCallback(void)
{
  // Turn on the GPIO clock so that we can turn on GPIOs
  CMU_ClockEnable(cmuClock_GPIO, true);

  
#if EMBER_AF_PLUGIN_FEM_CONTROL_TX_ACTIVE == 1
    //Enable the output of TX_ACTIVE based on a specific port and pin
    //Configure the GPIO to be an output
    GPIO_PinModeSet(AF_PRS_CHX_PORT(PRS_CHANNEL_TX,PRS_LOCATION_TX), AF_PRS_CHX_PIN(PRS_CHANNEL_TX,PRS_LOCATION_TX), gpioModePushPull, 0);
    //Setup the PRS to output TX_ACTIVE on the channel and location chosen. Defined in prs.c
    halConfigurePrs(PRS_CHANNEL_TX, PRS_LOCATION_TX, _PRS_CH_CTRL_SOURCESEL_RAC, _PRS_CH_CTRL_SIGSEL_RACPAEN, EMBER_AF_PLUGIN_FEM_CONTROL_TX_CHANNEL_INVERSION);
#endif
  
#if EMBER_AF_PLUGIN_FEM_CONTROL_RX_ACTIVE == 1 
    //Enable the output of RX_ACTIVE based on a specific port and pin
    //Configure the GPIO to be an output
    GPIO_PinModeSet(AF_PRS_CHX_PORT(PRS_CHANNEL_RX,PRS_LOCATION_RX), AF_PRS_CHX_PIN(PRS_CHANNEL_RX,PRS_LOCATION_RX), gpioModePushPull, 0);
    //Setup the PRS to output RX_ACTIVE on the channel and location chosen. Defined in prs.c
    halConfigurePrs(PRS_CHANNEL_RX, PRS_LOCATION_RX, _PRS_CH_CTRL_SOURCESEL_RAC, _PRS_CH_CTRL_SIGSEL_RACLNAEN, EMBER_AF_PLUGIN_FEM_CONTROL_RX_CHANNEL_INVERSION);
#endif
}
