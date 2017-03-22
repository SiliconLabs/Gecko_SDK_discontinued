/*
 * File: bootloader-usb.c
 * Description: em35x bootloader serial interface functions for a USB
 *
 *
 * Copyright 2012 by Ember Corporation. All rights reserved.                *80*
 */

#include PLATFORM_HEADER  // should be iar.h
#include "bootloader-common.h"
#include "bootloader-serial.h"
#include "stack/include/ember-types.h"
#include "hal.h"

#include "stack/include/error.h"
#include "serial/serial.h"
#include "hal/micro/cortexm3/usb/em_usb.h"


void serInit(void) 
{
  //Note that this function does what it needs to in terms of GPIO config
  //and USB enumeration.
  emberSerialInit(3,0,0,0);
  //Note that bootloaders do not use interrupts so USB requires a call
  //to halUsbIsr() here and in the three low level ser* functions that
  //interface to the emberSerial driver (which interfaces to the USB driver).
  halUsbIsr();
}


void serPutFlush(void)
{
  //USB doesn't support or need emberSerialWaitSend(3);  It will block
  //normally so flushing is not necessary.
}

//sending a char over USB goes through the serial driver which implements
//the TX Q
void serPutChar(uint8_t ch)
{
  emberSerialWriteByte(3, ch);
  halUsbIsr();
}

void serPutStr(const char *str)
{
  while (*str) {
    serPutChar(*str);
    str++;
  }
}

void serPutBuf(const uint8_t buf[], uint8_t size)
{
  uint16_t i;

  for (i=0; i<size; i++) {
    serPutChar(buf[i]);
  }
}

void serPutDecimal(uint16_t val) 
{
  char outStr[] = {'0','0','0','0','0','\0'};
  uint8_t i = sizeof(outStr)/sizeof(char) - 1;
  uint8_t remainder;

  // Convert the integer into a string.
  while(--i) {
    remainder = val%10;
    val /= 10;
    outStr[i] = remainder + '0';
  }

  // Find the first non-zero character
  for(i = 0; i < (sizeof(outStr)/sizeof(char)-2); i++) {
    if(outStr[i] != '0') {
      break;
    }
  }

  // Print the final string
  serPutStr(outStr+i);
}

void serPutHex(uint8_t byte)
{
  uint8_t val;
  val = ((byte & 0xF0) >> 4);
  serPutChar((val>9)?(val-10+'A'):(val+'0') );
  val = (byte & 0x0F);
  serPutChar((val>9)?(val-10+'A'):(val+'0') );
}

void serPutHexInt(uint16_t word)
{
  serPutHex(HIGH_BYTE(word));
  serPutHex(LOW_BYTE(word));
}



//Using emberSerialReadByte for USB serGetChar doesn't require the use of
//serCharAvailable since emberSerialReadByte returns an EMBER_SUCCESS
//status.  But, other functions higher up do want to use serCharAvailable.
bool serCharAvailable(void)
{
  halUsbIsr();
  if(emberSerialReadAvailable(3)) {
    return true;
  } else {
    return false;
  }
}

//get char if available, else return error.  receiving a char over USB
//goes through the serial driver which implements the RX Q.
BL_Status serGetChar(uint8_t* ch)
{
  halUsbIsr();
  if(emberSerialReadByte(3, ch) == EMBER_SUCCESS) {
    return BL_SUCCESS;
  } else {
    return BL_ERR;
  }
}

// get chars until rx buffer is empty
void serGetFlush(void)
{
  uint8_t status = BL_SUCCESS;
  uint8_t tmp;
  do {
    status = serGetChar(&tmp);
  } while(status == BL_SUCCESS);
}
