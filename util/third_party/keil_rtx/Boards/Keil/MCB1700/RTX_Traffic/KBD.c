/*----------------------------------------------------------------------------
 * Name:    KBD.c
 * Purpose: low level Joystick, INT0 functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "LPC17xx.H"                         /* LPC17xx definitions           */
#include "KBD.h"

uint32_t KBD_val  = 0;             
uint32_t INT0_val = 0;             

/*----------------------------------------------------------------------------
  initialize LED Pins
 *----------------------------------------------------------------------------*/
void KBD_Init (void) {

  LPC_PINCON->PINSEL4 &= ~(3<<20);           /* P2.10 is GPIO (INT0)          */
  LPC_GPIO2->FIODIR   &= ~(1<<10);           /* P2.10 is input                */

  LPC_PINCON->PINSEL3 &= ~((3<< 8)|(3<<14)|(3<<16)|(3<<18)|(3<<20)); /* P1.20, P1.23..26 is GPIO (Joystick) */
  LPC_GPIO1->FIODIR   &= ~((1<<20)|(1<<23)|(1<<24)|(1<<25)|(1<<26)); /* P1.20, P1.23..26 is input           */
}


/*----------------------------------------------------------------------------
  Get Joystick value
 *----------------------------------------------------------------------------*/
uint32_t KBD_Get  (void) {
  uint32_t kbd_val;

  kbd_val = (LPC_GPIO1->FIOPIN >> 20) & KBD_MASK;
  return (kbd_val);
}

/*----------------------------------------------------------------------------
  Get INT0 value
 *----------------------------------------------------------------------------*/
uint32_t INT0_Get  (void) {
  uint32_t int0_val;

  int0_val = (LPC_GPIO2->FIOPIN >> 10) & 0x01;
  return (int0_val);
}
