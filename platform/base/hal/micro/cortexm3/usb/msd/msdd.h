/***************************************************************************//**
 * @file  msdd.h
 * @brief Mass Storage class Device (MSD) driver.
 * @author Nathaniel Ting
 * @version 3.20.1
 *******************************************************************************
 * @section License
 * Copyright 2013 by Silicon Labs. All rights reserved.                     *80*
 *****************************************************************************/
#ifndef __MSDD_H
#define __MSDD_H

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Msd
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define MEDIA_BUFSIZ    4096 *4     /**< Intermediate media storage buffer size */
#define SECTOR_SIZE     512
#define BULK_OUT        0x05
#define BULK_IN         0x85
/**************************************************************************//**
 * @brief Status info for one BOT CBW -> Data I/O -> CSW cycle.
 *****************************************************************************/
typedef struct
{
  bool                                                       valid;     /**< True if the CBW is valid.    */
  uint8_t                                                    direction; /**< Set if BOT direction is IN.  */
  uint8_t                                                    *pData;    /**< Media data pointer.          */
  uint32_t                                                   lba;       /**< SCSI Read/Write lba address. */
  uint32_t                                                   xferLen;   /**< BOT transfer length.         */
  uint32_t                                                   maxBurst;  /**< Max length of one transfer.  */
  enum              { XFER_MEMORYMAPPED = 0, XFER_INDIRECT } xferType;
  /**< MSD media access type.       */
} MSDD_CmdStatus_TypeDef;

extern bool usbDisconnected;

/*** MSDD Device Driver Function prototypes ***/

bool MSDD_Handler(void);
void MSDD_Init(int activityLedPort, uint32_t activityLedPin);
bool MSDD_Busy(void);

#ifdef __cplusplus
}
#endif

/** @} (end group Msd) */
/** @} (end group Drivers) */

#endif /* __MSDD_H */
