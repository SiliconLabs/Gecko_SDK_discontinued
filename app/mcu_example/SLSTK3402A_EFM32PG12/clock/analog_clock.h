/***************************************************************************//**
 * @file analog_clock.h
 * @brief Helper functions for drawing an analog clock
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __ANALOG_CLOCK_H__
#define __ANALOG_CLOCK_H__

/* Analog clock prototypes */
void analogClockInitGraphics(void);
void analogClockUpdate(struct tm *t, bool forceRedraw);

#endif /* __ANALOG_CLOCK_H__ */
