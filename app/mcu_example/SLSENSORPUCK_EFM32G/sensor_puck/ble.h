/**************************************************************************//**
* @file
* @brief Bluetooth Low Energy driver
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

#ifndef BLE_H_
#define BLE_H_

#define BLE_SAMPLE_COUNT    5

extern volatile bool BLE_Update_Mode;
extern volatile bool BLE_Ready;
extern volatile bool BLE_Initialized;
extern volatile bool BLE_Chg_Pending;

void BLE_Init(void);
void BLE_Update(void);
bool BLE_OnMeasurementModeChange(void);
void BLE_OnMsgReceived(uint8_t Msg[]);
void BLE_OnHostWakeup(void);
#endif /* BLE_H_ */
