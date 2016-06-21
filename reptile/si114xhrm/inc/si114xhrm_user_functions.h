/** @file si114xhrm_user_functions.h
 *  @brief si114x functions to be provided by developer specific to
 *  the system in which the HRM algorithm is used.
 *
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef SI114XHRM_USER_FUNC_H
#define SI114xHRM_USER_FUNC_H

#include <stdint.h>
#include "si114x_types.h"


int si114xFindEvb(char *port_description, char *last_port, int num_ports_found);
int si114xSetupDebug(HANDLE si114x_handle, void *si114x_debug);
int si114xOutputDebugMessage(HANDLE si114x_handle, char *message);
s16 Si114xWriteToRegister(HANDLE si114x_handle, u8 address, u8 data);
s16 Si114xReadFromRegister(HANDLE si114x_handle, u8 address);
int16_t Si114xBlockWrite(HANDLE si114x_handle,
                     uint8_t address, uint8_t length, uint8_t *values);
s16 Si114xBlockRead(HANDLE si114x_handle,
                        u8 address, u8 length, u8 *values);
s16 Si114xProcessIrq(HANDLE si114x_handle, u16 timestamp);
s16 Si114xIrqQueueNumentries(HANDLE si114x_handle);
s16 Si114xIrqQueue_Get(HANDLE si114x_handle, SI114X_IRQ_SAMPLE *samples);
s16 Si114xIrqQueue_Clear(HANDLE si114x_handle);
s16 Si114xInit(void *port, int options, HANDLE *si114x_handle);
s16 Si114xClose(HANDLE si114x_handle);

#endif
