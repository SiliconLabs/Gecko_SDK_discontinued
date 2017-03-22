/**************************************************************************//**
* @file
* @brief Sensor Puck demo for EFM32G
* @version 5.1.2

******************************************************************************
* @section License
* <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/

#ifndef SENSOR_PUCK_H_
#define SENSOR_PUCK_H_

#include "heart_rate_monitor.h"

#define GREEN_LED_PORT gpioPortC
#define GREEN_LED_PIN  1
#define RED_LED_PORT   gpioPortC
#define RED_LED_PIN    0

#define HRM_TIMEOUT_MS                90000
#define PERIODIC_UPDATE_MS            1000
#define SI114X_IRQ_PORT               gpioPortC
#define SI114X_IRQ_PIN                14
#define BLE_HOST_WAKEUP_PIN           11
#define PS1_SKIN_CONTACT_THRESHOLD    1500

#define SI114X_I2C_ADDR               0x60

typedef enum
{
  ENVIRONMENTAL_MODE,
  BIOMETRIC_MODE
} Meas_Mode_TypeDef;

extern Meas_Mode_TypeDef measurementMode;

extern bool              bluetoothDataPending;

extern uint32_t          rhData;   /* milliperecent */
extern int32_t           tempData; /* millidegree C */
extern uint16_t          uvData;   /* index */
extern uint16_t          alsData;  /* lux */
extern uint32_t          vBat;     /* millivolt */

extern HRMSpO2State_t    HRMState;
extern int32_t           HRMRate;
extern uint16_t          HRMSample[];
extern HRMSpO2State_t    SpO2State;
extern int32_t           HRMSpO2Value;

void hrmSampleCallback(uint16_t sample);
void CalibrateLFRCO(void);
#endif /* SENSOR_PUCK_H_ */
