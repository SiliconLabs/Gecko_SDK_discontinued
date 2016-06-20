/***************************************************************************//**
 * @file radio.h
 * @brief This file is contains the public radio interface functions.
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

#ifndef RADIO_H_
#define RADIO_H_

/* Uncomment this to use your own generated radio configuration header file,
 * instead of the kit default one. */
//#define RADIO_USE_GENERATED_CONFIGURATION

#if (defined RADIO_USE_GENERATED_CONFIGURATION)
/* Include the generated radio configuration */
#include "radio-config-wds-gen.h"
#else
/* Include the default radio configuration for the board */
#if   ( (defined SL_WSTK6200A) || (defined SL_WSTK6220A) )
#include "radio-config-wds-gen_R60-868MHz-13dBm.h"
#elif ( (defined SL_WSTK6201A) || (defined SL_WSTK6221A) )
#include "radio-config-wds-gen_R55-434MHz-10dBm.h"
#elif ( (defined SL_WSTK6202A) || (defined SL_WSTK6222A) )
#include "radio-config-wds-gen_R63-915MHz-20dBm.h"
#elif (defined SL_WSTK6223A)
#include "radio-config-wds-gen_R63-490MHz-20dBm.h"
#elif (defined SL_WSTK6224A)
#include "radio-config-wds-gen_R63-169MHz-20dBm.h"
#else
#error No radio configuration is defined! Create your own radio configuration or define your kit properly!
#endif
#endif

/*****************************************************************************
 *  Global Macros & Definitions
 *****************************************************************************/
/*! Maximal packet length definition (FIFO size) */
#define RADIO_MAX_PACKET_LENGTH     64u

/* Location of the sent data in the packet */
#if ( (defined RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE) && (RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE > 0) )
/* TODO: Update this if ISD gen can generate the proper definitons */
#define APP_PKT_DATA_LOC    1u
#else
#define APP_PKT_DATA_LOC    0u
#endif

/*****************************************************************************
 *  Global Typedefs & Enums
 *****************************************************************************/
typedef struct
{
  const uint8_t  *Radio_ConfigurationArray;

        uint8_t   Radio_ChannelNumber;
        uint8_t   Radio_PacketLength;
        uint8_t   Radio_State_After_Power_Up;

        uint16_t  Radio_Delay_Cnt_After_Reset;

        uint8_t   Radio_Variable_Packet_State;

        uint8_t   Radio_CustomPayload[RADIO_MAX_PACKET_LENGTH];

} tRadioConfiguration;

/*****************************************************************************
 *  Global Variable Declarations
 *****************************************************************************/
extern const tRadioConfiguration * const pRadioConfiguration;
extern uint8_t radioPkt[RADIO_MAX_PACKET_LENGTH];
extern bool    radioIrqReceived;

/*! Si446x configuration array */
extern const uint8_t Radio_Configuration_Data_Array[];


/*****************************************************************************
 *  Global Function Declarations
 *****************************************************************************/
void    ezradioInit(void);
bool    ezradioCheckReceived(void);
bool    ezradioCheckTransmitted(void);
bool    ezradioCheckCRCError(void);
void    ezradioStartRX(uint8_t);
bool    ezradioStartTx(uint8_t, uint8_t *);
void    ezradioResetTRxFifo(void);

#endif /* RADIO_H_ */
