// File: uart-link.h
//
// Description: NCP side of the Host<->NCP serial link.
//
// Copyright 2014 by Silicon Labs. All rights reserved.                     *80*

bool halHostUartTxIdle(void);
void halHostUartLinkTx(const uint8_t *data, uint16_t length);
void halHostSerialInit(void);
void halHostSerialTick(void);
bool halHostUartTxEmpty(void);
void halHostUartForceRTS(void);
bool halHostCtsWaitOrTimeout(bool reset);
