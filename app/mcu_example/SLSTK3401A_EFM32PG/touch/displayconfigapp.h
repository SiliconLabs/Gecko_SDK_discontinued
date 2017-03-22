/***************************************************************************//**
 * @file displayconfigapp.h
 * @brief Display application specific configuration file.
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

#ifndef __DISPLAYCONFIGAPP_H__
#define __DISPLAYCONFIGAPP_H__

/* Include TEXTDISPLAY support because this example prints text to the
 * display. */
#define INCLUDE_TEXTDISPLAY_SUPPORT

/* Use number font of size 16x20 pixels */
#define TEXTDISPLAY_NUMBER_FONT_16x20

/* Disable scroll mode on the text display. */
#define RETARGETTEXTDISPLAY_SCROLL_MODE  (false)

/* Add Carriage Return (CR) to Line Feed (LF) characters
 * on the text display. */
#define RETARGETTEXTDISPLAY_LINE_FEED_MODE  (true)

#endif /* __DISPLAYCONFIGAPP_H__ */
