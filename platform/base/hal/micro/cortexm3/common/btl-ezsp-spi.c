/***************************************************************************//**
 * @file hal/micro/cortexm3/common/btl-ezsp-spi.c
 * @brief EFM32 bootloader serial interface functions for ezsp spi
 * @version
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
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

#include PLATFORM_HEADER  // should be iar.h
#include "bootloader-common.h"
#include "bootloader-gpio.h"
#include "bootloader-serial.h"
#include "btl-ezsp-spi-protocol.h"
#include "stack/include/ember-types.h"
#include "hal.h"

bool spipFramePending;       // halHostCommandBuffer[] contains a frame
bool spipResponsePending;    // have data to send in next spi session
uint8_t preSpiComBufIndex;        // index for reading from halHostCommandBuffer[]

void serInit(void)
{
  halHostSerialInit();
}

void serPutFlush(void)
{
  //stub
}
void serPutChar(uint8_t ch)
{
  //stub
  (void)ch;
}
void serPutStr(const char *str)
{
  //stub
  (void)*str;
}
void serPutBuf(const uint8_t buf[], uint8_t size)
{
  //stub
  (void)*buf; (void)size;
}
void serPutHex(uint8_t byte)
{
  //stub
  (void)byte;
}
void serPutHexInt(uint16_t word)
{
  //stub
  (void)word;
}

// get char if available, else return error
BL_Status serGetChar(uint8_t* ch)
{
  if (serCharAvailable()) {
    *ch = halHostCommandBuffer[XMODEM_FRAME_START_INDEX + preSpiComBufIndex];
    preSpiComBufIndex++;
    return BL_SUCCESS;
  } else {
    return BL_ERR;
  }
}

bool serCharAvailable(void)
{
  if (spipFramePending) {           // already have frame?
    return true;
  }

  if (!halHostSerialTick(false)) {  // see if a frame was just received
    return false;
  }
  if (spipResponsePending) {        // need to send a response?
    halHostCallback(false);         // send it at once
    halHostSerialTick(true);
    spipResponsePending = false;
    return false;
  }
  spipFramePending = true;          // flag frame available
  preSpiComBufIndex = 0;            // and set read index to the frame start
  return true;
}

void serGetFlush(void)
{
  //stub
}
