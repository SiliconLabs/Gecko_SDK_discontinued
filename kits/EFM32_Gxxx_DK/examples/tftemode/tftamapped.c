/**************************************************************************//**
 * @file
 * @brief EFM32_G2xx_DK, TFT Initialization and setup for Address Mapped mode
 * @version 4.1.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdbool.h>
#include "em_device.h"
#include "em_gpio.h"
#include "em_ebi.h"
#include "bsp.h"
#include "tftamapped.h"
/* DMD init from dmd_ssd2119_direct.c */
#include "glib/glib.h"
#include "dmd/ssd2119/dmd_ssd2119.h"

/* Flag to indicate that we need to initialize once if starting up in */
bool runOnce = true;

/**************************************************************************//**
 * @brief  TFT initialize or reinitialize to Address Mapped Mode
 *         Assumes EBI has been configured correctly in BSP_Init()
 *
 * @return true if we should redraw into buffer, false if BC has control
 *         over display
 *****************************************************************************/
bool TFT_AddressMappedInit(void)
{
  bool     ret;
  EMSTATUS status;

  /* If we are in BC_UIF_AEM_EFM state, we can redraw graphics */
  if (BSP_RegisterRead(BC_AEMSTATE) == 1)
  {
    /* If we're not BC_ARB_CTRL_EBI state, we need to reconfigure display controller */
    if ((BSP_RegisterRead(BC_BUS_CFG) == BC_BUS_CFG_FSMC) || runOnce)
    {
      /* Initialize graphics - abort on failure */
      status = DMDIF_init(BC_SSD2119_BASE, BC_SSD2119_BASE + 2);
      if (status == DMD_OK) status = DMD_init(0);
      if ((status != DMD_OK) && (status != DMD_ERROR_DRIVER_ALREADY_INITIALIZED)) while (1) ;
      /* Make sure display is configured with correct rotation */
      if ((status == DMD_OK)) DMD_flipDisplay(1, 1);

      runOnce = false;
    }
    ret = true;
  }
  else
  {
    ret = false;
  }
  return ret;
}
