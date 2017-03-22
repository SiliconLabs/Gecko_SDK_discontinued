/* File: bootloader-uart.c
 * Description: em35x bootloader serial interface functions for a uart
 *
 * Copyright 2013 Silicon Laboratories, Inc.                                *80*
 */

#include PLATFORM_HEADER  // should be iar.h
#include "bootloader-common.h"
#include "bootloader-serial.h"
#include "stack/include/ember-types.h"
#include "hal.h"


// Function Name: serInit
// Description:   This function configures the UART. These are default
//                settings and can be modified for custom applications.
// Parameters:    none
// Returns:       none
//
void serInit(void)
{
  // set uart to 115.2k baud
#if     BTL_SYSCLK_KNOWN
  // We know peripheral clock must be 12 MHz
  // set uart to 115.2k baud
  SC1_UARTPER  = 104;
//SC1_UARTFRAC = 0; // 0 is default for this register so save code space
#else//!BTL_SYSCLK_KNOWN
  halInternalUartSetBaudRate(1, 115200);
#endif//BTL_SYSCLK_KNOWN

  // set to 8 bits, no parity, 1 stop bit
  SC1_UARTCFG = BIT(SC_UART8BIT_BIT);
  SC1_MODE = SC1_MODE_UART;   // SC1 set to uart mode

  // configure IO pins to use UART
  halGpioSetConfig(PORTB_PIN(1),GPIOCFG_OUT_ALT);
  halGpioSetConfig(PORTB_PIN(2),GPIOCFG_IN);
}


void serPutFlush(void)
{
  while ( !(SC1_UARTSTAT & SC_UARTTXIDLE) )
  { /*wait for txidle*/ }
}

// wait for transmit free then send char
void serPutChar(uint8_t ch)
{
  while ( !(SC1_UARTSTAT & SC_UARTTXFREE))
  { /*wait for txfree*/ }
  SC1_DATA = ch;
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
  int8_t i = sizeof(outStr)/sizeof(char) - 1;
  uint8_t remainder, lastDigit = i-1;

  // Convert the integer into a string.
  while(--i >= 0) {
    remainder = val%10;
    val /= 10;
    outStr[i] = remainder + '0';
    if(remainder != 0) {
      lastDigit = i;
    }
  }

  // Print the final string
  serPutStr(outStr+lastDigit);
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

bool serCharAvailable(void)
{
  if(SC1_UARTSTAT & SC_UARTRXVAL) {
    return true;
  } else {
    return false;
  }
}

// get char if available, else return error
BL_Status serGetChar(uint8_t* ch)
{
  if(serCharAvailable()) {
    *ch = SC1_DATA;
#if     (CORTEXM3_EM35X_GEN3 && defined(ENABLE_OSC24M) && (ENABLE_OSC24M != 24) \
         && (ENABLE_OSC24M != 24000) && (ENABLE_OSC24M != 24000000))
    // Hack workaround for EMLIPARI-101 UART HW bug on non-24 MHz SysClk
    static bool uartFixTried = false;
    if ( (SC1_UARTSTAT & SC_UARTFRMERR) && (*ch != 0 /* a BRK */) ) {
      if (!uartFixTried) {
        // Try to correct for this HW bug by inverting fraction bit
        SC1_UARTFRAC = !SC1_UARTFRAC;
        uartFixTried = true;
      }
      *ch >>= 1;
    }
#endif//(CORTEXM3_EM35X_GEN3 && defined(ENABLE_OSC24M) && ...)
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
