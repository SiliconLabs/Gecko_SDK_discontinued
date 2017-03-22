/**************************************************************************//**
 * @file  msdd.c
 * @brief Mass Storage class Device (MSD) driver.
 * @author Nathaniel Ting
 * @version 3.20.1
 *******************************************************************************
 * @section License
 * Copyright 2013 by Silicon Labs. All rights reserved.                     *80*
 *****************************************************************************/
#ifdef USB_MSD

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "hal/micro/cortexm3/usb/em_usb.h"
#include "msdbot.h"
#include "msdscsi.h"
#include "msdd.h"
#include "msddmedia.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/*** Typedef's and defines. ***/

#define DIR_DATA_OUT    0
#define DIR_DATA_IN     1
#define MAX_BURST       32768U          /* 32 * 1024 */

/**************************************************************************//**
 * @brief MSD device state machine states.
 *****************************************************************************/
typedef enum
{
  MSDD_IDLE                 = 0,
  MSDD_WAITFOR_CBW          = 1,
  MSDD_WAITFOR_RECOVERY     = 2,
  MSDD_SEND_CSW             = 3,
  MSDD_WAIT_FOR_INUNSTALLED = 4,
  MSDD_STALL_IN             = 5,
  MSDD_ACCESS_INDIRECT      = 6,
  MSDD_WRITE_INDIRECT       = 7,
  MSDD_DO_CMD_TASK          = 8,
} msdState_TypeDef;

/*** Function prototypes. ***/

static int            CbwCallback(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining);
static inline bool  CswMeaningful(void);
static inline bool  CswValid(void);
static inline void  EnableNextCbw(void);
static void           ProcessScsiCdb(void);
static inline void  SendCsw(void);
static int            UsbSetupCmd(const USB_Setup_TypeDef *setup);
static void           UsbStateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);
static void           UsbXferBotData(uint8_t *data, uint32_t len, USB_XferCompleteCb_TypeDef cb);
static void           XferBotData(uint32_t length);
static int            XferBotDataCallback(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining);
static int            XferBotDataIndirectCallback(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining);
static void         delayedUnStall(void);

/*** Include device descriptor definitions. ***/

#include "descriptors.h"

/*** Variables ***/

/* Storage for one CBW */
STATIC_UBUF(cbw, CBW_LEN);
static MSDBOT_CBW_TypeDef *pCbw = (MSDBOT_CBW_TypeDef*) &cbw;

// EFM32_ALIGN(4)
/* Storage for one CSW */
static MSDBOT_CSW_TypeDef csw;
static MSDBOT_CSW_TypeDef *pCsw = &csw;

STATIC_UBUF(mediaBuffer, MEDIA_BUFSIZ);  /* Intermediate media storage buffer */

static MSDD_CmdStatus_TypeDef CmdStatus;
static MSDD_CmdStatus_TypeDef *pCmdStatus = &CmdStatus;
static msdState_TypeDef       savedState; /* MSD state machine state. */
// static int ledPort;
// static unsigned int ledPin;
bool usbDisconnected;

/**************************************************************************//**
 * @brief Preformated SCSI INQUIRY response data structure.
 *****************************************************************************/
// EFM32_ALIGN(4)
static const MSDSCSI_InquiryData_TypeDef InquiryData =
{
  { .PeripheralDeviceType = 0, .PeripheralQualifier = 0 }, /* Block device  */
  { .Reserved1            = 0, .Removable           = 1 },

  .Version = 5,                                       /* T10 SPC-3 compliant */

  { .ResponseDataFormat = 2,                          /* T10 SPC-3 compliant reponse data */
    .HiSup              = 0, .NormACA = 0, .Obsolete1 = 0 },

  .AdditionalLength = 31,

  { .Protect = 0, .Reserved2           = 0, .ThirdPartyCode = 0,
    .Tpgs    = 0, .Acc = 0, .Sccs = 0 },

  { .Addr16 = 0, .Obsolete2           = 0, .MChngr = 0, .MultiP = 0,
    .Vs1    = 0, .EncServ = 0, .BQue = 0 },

  { .Vs2  = 0, .CmdQue              =0, .Obsolete3 = 0, .Linked = 0,
    .Sync = 0, .Wbus16 = 0, .Obsolete4 = 0 },

  .T10VendorId          = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
  .ProductId            = { 'E', 'M', '3', '5', '8', ' ', 'M', 'S', 'D', ' ', 'D', 'e', 'v', 'i', 'c', 'e' },
  .ProductRevisionLevel ={ '1', '.', '0', '0' }
};

/**************************************************************************//**
 * @brief
 *   Preformated SCSI REQUEST SENSE response data structure.
 *   Used when no error condition exists in the SCSI device server.
 *****************************************************************************/
EFM32_ALIGN(4)
static const MSDSCSI_RequestSenseData_TypeDef NoSenseData __attribute__ ((aligned(4))) =
{
  { .ResponseCode      = 0x70, .Valid    = 0 },
  .Obsolete = 0,
  { .SenseKey          =    0, .Reserved =0, .Ili = 0, .Eom = 0, .FileMark = 0 },
  .Information      = 0,
  .AdditionalLength = 10,
  .CmdSpecificInfo  = 0,
  .Asc              = 0,
  .Ascq             = 0,
  .Fruc             = 0,
  { .SenseKeySpecific1 =    0, .Sksv     = 0 },
  .SenseKeySpecific2 = 0,
  .SenseKeySpecific3 = 0
};

/**************************************************************************//**
 * @brief
 *   Preformated SCSI REQUEST SENSE response data structure.
 *   Used when host has issued an unsupported SCSI command, or when an
 *   invalid field in a SCSI command descriptor block (CDB) is detected.
 *****************************************************************************/
EFM32_ALIGN(4)
static const MSDSCSI_RequestSenseData_TypeDef IllegalSenseData __attribute__ ((aligned(4))) =
{
  { .ResponseCode      = 0x70, .Valid = 0 },
  .Obsolete = 0,
  { .SenseKey =    5,          /* SensKey = 5 => ILLEGAL REQUEST */
    .Reserved = 0, .Ili = 0, .Eom = 0, .FileMark = 0 },
  .Information      = 0,
  .AdditionalLength = 10,
  .CmdSpecificInfo  = 0,
  .Asc              = 0x24,    /* Asc/Ascq = 0x24/0x00 => INVALID FIELD IN CDB*/
  .Ascq             = 0,
  .Fruc             = 0,
  { .SenseKeySpecific1 =    0, .Sksv  = 0 },
  .SenseKeySpecific2 = 0,
  .SenseKeySpecific3 = 0
};

static volatile msdState_TypeDef        msdState;     /**< BOT statemachine state enumaration. */
static MSDSCSI_RequestSenseData_TypeDef *pSenseData;  /**< Points to current sense data.       */

// /** @endcond */

/**************************************************************************//**
 * @brief Initialize MSD device.
 *
 * @param[in] activityLedPort
 *   Specify a GPIO port for a LED activity indicator (i.e. enum gpioPortX)
 *   Pass -1 if no indicator LED is available.
 *
 * @param[in] activityLedPin
 *   Pin number on activityLedPort for the LED activity indicator.
 *****************************************************************************/
void MSDD_Init(int activityLedPort, uint32_t activityLedPin)
{

  // typedef size error
  assert(sizeof(MSDSCSI_Read10_TypeDef) == SCSI_READ10_LEN);
  assert(sizeof(MSDSCSI_Write10_TypeDef) == SCSI_WRITE10_LEN);
  assert(sizeof(MSDSCSI_RequestSense_TypeDef) == SCSI_REQUESTSENSE_LEN);
  assert(sizeof(InquiryData) == SCSI_INQUIRYDATA_LEN);
  assert(sizeof(NoSenseData) == SCSI_REQUESTSENSEDATA_LEN);
  assert(sizeof(IllegalSenseData) == SCSI_REQUESTSENSEDATA_LEN);
  assert(sizeof(MSDSCSI_ReadCapacity_TypeDef) == SCSI_READCAPACITY_LEN);
  assert(sizeof(MSDSCSI_ReadCapacityData_TypeDef) == SCSI_READCAPACITYDATA_LEN);
  msdState   = MSDD_IDLE;
  pSenseData = (MSDSCSI_RequestSenseData_TypeDef*) &NoSenseData;
  USBD_Init(&initstruct);     /* Start USB. */

}

bool MSDD_Busy(void)
{
  if (msdState== MSDD_ACCESS_INDIRECT)
    return true;//(msdState == MSDD_WRITE_INDIRECT);
  
  return false;
}

/**************************************************************************//**
 * @brief
 *   Serve the MSD state machine.
 *   This function should be called on a regular basis from your main loop.
 *   It cannot be called from within an interrupt handler.
 * @return
 *   Returns true if there is no pending tasks to perform. This means that
 *   energymodes (sleep) functionality can be used.
 *****************************************************************************/
bool MSDD_Handler(void)
{
  static uint32_t len;        /* Note: len is static ! */

  switch (msdState)
  {
  case MSDD_ACCESS_INDIRECT:
    #ifdef USB_DEBUG_MSDHANDLER
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "MSDD_ACCESS_INDIRECT\r\n");
    #endif
    if (pCmdStatus->xferLen)
    {
      len = EFM32_MIN(pCmdStatus->xferLen, pCmdStatus->maxBurst);

      msdState = MSDD_IDLE;
      if (pCmdStatus->direction)
      {
        MSDDMEDIA_Read(pCmdStatus, mediaBuffer, len / SECTOR_SIZE);
      }
      UsbXferBotData(mediaBuffer, len, XferBotDataIndirectCallback);
    }
    else
    {
      /* We are done ! */
      msdState = savedState;

      if (msdState == MSDD_SEND_CSW)
      {
        SendCsw();
        EnableNextCbw();
        msdState = MSDD_WAITFOR_CBW;
      }

      else if (msdState == MSDD_STALL_IN)
      {
          delayedUnStall();
      }
    }
    break;

  case MSDD_WRITE_INDIRECT:
    #ifdef USB_DEBUG_MSDHANDLER
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "MSDD_WRITE_INDIRECT\r\n");
    #endif

    MSDDMEDIA_Write(pCmdStatus, mediaBuffer, len / SECTOR_SIZE);
    pCmdStatus->lba += len / SECTOR_SIZE;
    msdState         = MSDD_ACCESS_INDIRECT;
    break;

  case MSDD_DO_CMD_TASK:
    #ifdef USB_DEBUG_MSDHANDLER
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "MSDD_DO_CMD_TASK\r\n");
    #endif
    /* else if ( .... )  Add more when needed. */
    SendCsw();
    EnableNextCbw();
    msdState = MSDD_WAITFOR_CBW;
    break;

  default:
    break;
  }
  return (msdState == MSDD_WAITFOR_CBW) || (msdState == MSDD_IDLE);
}

// /** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/**************************************************************************//**
 * @brief
 *   Called on USB transfer completion callback. Will qualify and parse a
 *   Command Block Wrapper (CBW).
 *
 * @param[in] status
 *   The transfer status.
 *
 * @param[in] xferred
 *   Number of bytes actually transferred.
 *
 * @param[in] remaining
 *   Number of bytes not transferred.
 *
 * @return
 *   USB_STATUS_OK.
 *****************************************************************************/
static int CbwCallback(USB_Status_TypeDef status,
                       uint32_t xferred, uint32_t remaining)
{
  (void) remaining;
  #ifdef USB_DEBUG_MSD
    #ifdef USB_DEBUG_VERBOSE
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "CbwCallback: %d - %d\r\n", xferred, remaining);
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "msdState: %d\r\n", msdState);
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "status: %d\r\n", (status == USB_STATUS_OK)?1:0);
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "xferred: %d\r\n", (xferred == CBW_LEN)?1:0);
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "CswValid: %d\r\n", (CswValid())?1:0);
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "CswMeaningful: %d\r\n", (CswMeaningful())?1:0);
    #else
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "CbwCallback-->");
    #endif
  #endif
    
  if ((msdState == MSDD_WAITFOR_CBW) &&
      (status == USB_STATUS_OK) &&
      (xferred == CBW_LEN) &&
      (CswValid()) &&
      (CswMeaningful()))
  {

    /* Check the SCSI command descriptor block (CDB) */
    ProcessScsiCdb();

    if (pCmdStatus->valid)
      pCsw->bCSWStatus = USB_CLASS_MSD_CSW_CMDPASSED;
    else
      pCsw->bCSWStatus = USB_CLASS_MSD_CSW_CMDFAILED;

    pCsw->dCSWSignature   = CSW_SIGNATURE;
    pCsw->dCSWTag         = pCbw->dCBWTag;
    pCsw->dCSWDataResidue = pCbw->dCBWDataTransferLength;

    /* Check the "thirteen cases" */

    if ((pCbw->dCBWDataTransferLength != 0) &&
        (pCbw->Direction != pCmdStatus->direction))
    {
      /* Handle cases 8 and 10 */
      pCsw->bCSWStatus = USB_CLASS_MSD_CSW_PHASEERROR;

      if (pCbw->Direction)
      {
        /* Host expects to receive data, case 8 */
        #ifdef USB_DEBUG_MSDHANDLER
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 8\r\n");
        #endif
        delayedUnStall();
        msdState = MSDD_WAIT_FOR_INUNSTALLED;
      }
      else
      {
        /* Host expects to send data, case 10 */
        #ifdef USB_DEBUG_MSDHANDLER
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 10\r\n");
        #endif
        USBD_StallEp(BULK_OUT);
        SendCsw();
        msdState = MSDD_IDLE;
      }
    }

    else if (pCbw->Direction || (pCbw->dCBWDataTransferLength == 0))
    {
      /* SCSI IN commands or commands without data phase */
      /* Handle cases 1-7 */

      if (pCbw->dCBWDataTransferLength == 0)
      {
        /* Host expects no data, case 1, 2 or 3 */

        #ifdef USB_DEBUG_MSDHANDLER
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 1/2/3\r\n");
        #endif
        if (pCmdStatus->xferLen)
        {
          /* Device has data to transmit, case 2 & 3 */
          pCsw->bCSWStatus = USB_CLASS_MSD_CSW_PHASEERROR;
        }

        if ((pCmdStatus->xferLen == 0) &&
            (pCmdStatus->xferType == XFER_INDIRECT))
        {
          /* Commands with no data phase which require timeconsuming  */
          /* processing are executed in MSDD_Handler()                */
          msdState = MSDD_DO_CMD_TASK;
        }
        else
        {
          SendCsw();
          EnableNextCbw();
          msdState = MSDD_WAITFOR_CBW;
        }
      }
      else if (pCbw->dCBWDataTransferLength == pCmdStatus->xferLen)
      {
        #ifdef USB_DEBUG_MSDHANDLER
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 6\r\n");
        #endif
        /* Host and device agree on transferlength, case 6 */
        /* Send data to host */
        msdState = MSDD_SEND_CSW;
        XferBotData(pCmdStatus->xferLen);
      }
      else if (pCbw->dCBWDataTransferLength > pCmdStatus->xferLen)
      {
        /* Host expects more data than device can provide, case 4 and 5 */

        if (pCmdStatus->xferLen > 0)
        {
          #ifdef USB_DEBUG_MSDHANDLER
            DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 5\r\n");
          #endif
          /* Device has data, case 5 */
          /* Send data to host */
          msdState = MSDD_STALL_IN;
          XferBotData(pCmdStatus->xferLen);
        }
        else
        {
          #ifdef USB_DEBUG_MSDHANDLER
            DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 4\r\n");
          #endif
          /* Device has no data, case 4 */
          delayedUnStall();
          msdState = MSDD_WAIT_FOR_INUNSTALLED;

        }
      }
      else
      {
        #ifdef USB_DEBUG_MSDHANDLER
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 7\r\n");
        #endif
        /* Host expects less data than device will provide, case 7 */
        pCsw->bCSWStatus = USB_CLASS_MSD_CSW_PHASEERROR;
        /* Send data to host */
        msdState = MSDD_SEND_CSW;
        XferBotData(pCbw->dCBWDataTransferLength);
      }
    }

    else /* Host Direction is OUT and Host transferlength > 0 */
    {
      /* SCSI OUT commands */
      /* Handle cases 9, 11, 12 and 13 */

      if (pCbw->dCBWDataTransferLength == pCmdStatus->xferLen)
      {
        #ifdef USB_DEBUG_MSDHANDLER
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 12\r\n");
        #endif
        /* Host and device agree on transferlength, case 12 */

        /* Read data from host */
        msdState = MSDD_SEND_CSW;
        XferBotData(pCmdStatus->xferLen);
      }
      else if (pCbw->dCBWDataTransferLength > pCmdStatus->xferLen)
      {
        #ifdef USB_DEBUG_MSDHANDLER
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 9/11\r\n");
        #endif
        /* Host intend to send more data than device expects, case 9 & 11 */
        pCsw->bCSWStatus = USB_CLASS_MSD_CSW_CMDFAILED;
        USBD_StallEp(BULK_OUT);
        SendCsw();
      }
      else
      {
        #ifdef USB_DEBUG_MSDHANDLER
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "case 13\r\n");
        #endif
        /* Host has less data than device expects to receive, case 13 */
        pCsw->bCSWStatus = USB_CLASS_MSD_CSW_PHASEERROR;
        USBD_StallEp(BULK_OUT);
        SendCsw();
        msdState = MSDD_IDLE;
      }
    }
    return USB_STATUS_OK;
  }

  if ((USBD_GetUsbState() == USBD_STATE_CONFIGURED))
  {
    /* Stall both Ep's and wait for reset recovery */
    USBD_StallEp(BULK_OUT);
    USBD_StallEp(BULK_IN);
    msdState = MSDD_WAITFOR_RECOVERY;
  }

  return USB_STATUS_OK;
}

/**************************************************************************//**
 * @brief
 *   callback workaround after failed status csw's. Unstalls endpoints and
 *   prepares for next cbw
 *****************************************************************************/
static int cswCallback(USB_Status_TypeDef status,
                       uint32_t xferred, uint32_t remaining)
{
  USBD_UnStallEp(BULK_IN);
  USBD_UnStallEp(BULK_OUT);
  EnableNextCbw();
  msdState = MSDD_WAITFOR_CBW;
  return USB_STATUS_OK;
}

/**************************************************************************//**
 * @brief
 *   Check if a Command Block Wrapper (CBW) is meaningful.
 *   The term "meaningful" is defined in the USB Mass Storage Class BOT spec.
 *****************************************************************************/
static inline bool CswMeaningful(void)
{
  if ((pCbw->Reserved1 == 0) &&
      (pCbw->Obsolete == 0) &&
      (pCbw->Reserved2 == 0) &&
      (pCbw->Lun == 0) &&
      (pCbw->Reserved3 == 0))
  {
    return true;
  }

  return false;
}

/**************************************************************************//**
 * @brief
 *   Check if a Command Block Wrapper (CBW) is valid.
 *   The term "valid" is defined in the USB Mass Storage Class BOT spec.
 *****************************************************************************/
static inline bool CswValid(void)
{
  return pCbw->dCBWSignature == CBW_SIGNATURE ? true : false; /* Ascii USBC */
}


/**************************************************************************//**
 * @brief
 *   Workaround to mimic 'ClearFeature:Halt Endpoint', which is HW handled/ignored
 *****************************************************************************/
static inline void delayedUnStall(void)
{
  USBD_StallEp(BULK_IN);
  if ((pCbw->CBWCB[ 0 ] == 0x2a)|| //write (10)
      (pCbw->CBWCB[ 0 ] == 0xaa)|| //write (12)
      (pCbw->CBWCB[ 0 ] == 0x28)|| //read (10)
      (pCbw->CBWCB[ 0 ] == 0xa8)|| //read (12)
      (pCbw->CBWCB[ 0 ] == 0x1a)|| //mode sense (6)
      (pCbw->CBWCB[ 0 ] == 0x5a))  //mode sense (10)
  {
    #ifdef USB_CERT_TESTING
    halCommonDelayMicroseconds(750);
    #else
    halCommonDelayMicroseconds(500);
    #endif
  }
#ifdef USB_CERT_TESTING
  else if((pCbw->CBWCB[ 0 ] == 0x12)|| // inquiry
          (pCbw->CBWCB[ 0 ] == 0x03)) // request sense
  {
    halCommonDelayMicroseconds(1500);
  }
#endif
  else
  {
    halCommonDelayMicroseconds(300);
  }
  USBD_UnStallEp(BULK_IN);
  SendCsw();
}

/**************************************************************************//**
 * @brief
 *   Start a USB bulk-out transfer to get next CBW.
 *****************************************************************************/
static inline void EnableNextCbw(void)
{
  #ifdef USB_DEBUG_MSD
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "EnableNextCbw\r\n" );
  #endif
  USBD_Read(BULK_OUT, (void*) &cbw, CBW_LEN, CbwCallback);
}

/**************************************************************************//**
 * @brief
 *   Parse a SCSI command.
 *   A minimal, yet sufficient SCSI command subset is supported.
 *****************************************************************************/
static void ProcessScsiCdb(void)
{
  #ifdef USB_DEBUG_MSD
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "ProcessScsiCdb: " );
  #endif
  MSDSCSI_Inquiry_TypeDef      *cbI;
  MSDSCSI_RequestSense_TypeDef *cbRS;
  MSDSCSI_ReadCapacity_TypeDef *cbRC;
  MSDSCSI_Read10_TypeDef       *cbR10;
  MSDSCSI_Write10_TypeDef      *cbW10;

  EFM32_ALIGN(4)
  static MSDSCSI_ReadCapacityData_TypeDef ReadCapData __attribute__ ((aligned(4)));

  pCmdStatus->valid    = false;
  pCmdStatus->xferType = XFER_MEMORYMAPPED;
  pCmdStatus->maxBurst = MAX_BURST;

  switch (pCbw->CBWCB[ 0 ])
  {
  case SCSI_INQUIRY:
    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SCSI_INQUIRY\r\n");
    #endif
    cbI = (MSDSCSI_Inquiry_TypeDef*) &pCbw->CBWCB;

    if ((cbI->Evpd == 0) && (cbI->PageCode == 0))
    {
      /* Standard Inquiry data request */
      pCmdStatus->valid     = true;
      pCmdStatus->direction = DIR_DATA_IN;
      pCmdStatus->pData     = (uint8_t*) &InquiryData;
      #ifdef USB_DEBUG_MSD
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"AllocationLength:%d\r\n",__REV16(cbI->AllocationLength));
      #endif
      pCmdStatus->xferLen   = EFM32_MIN(SCSI_INQUIRYDATA_LEN,
                                        __REV16(cbI->AllocationLength));
    }
    break;

  case SCSI_REQUESTSENSE:
    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SCSI_REQUESTSENSE\r\n");
    #endif
      // USBD_Disconnect();
    cbRS = (MSDSCSI_RequestSense_TypeDef*) &pCbw->CBWCB;

    if ((cbRS->Desc == 0) && (cbRS->Reserved1 == 0) &&
        (cbRS->Reserved2 == 0) && (cbRS->Reserved3 == 0))
    {
      pCmdStatus->valid     = true;
      pCmdStatus->direction = DIR_DATA_IN;
      pCmdStatus->pData     = (uint8_t*) pSenseData;
      pCmdStatus->xferLen   = EFM32_MIN(SCSI_REQUESTSENSEDATA_LEN,
                                        cbRS->AllocationLength);
      pSenseData = (MSDSCSI_RequestSenseData_TypeDef*) &NoSenseData;
    }
    break;

  case SCSI_READCAPACITY:
    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SCSI_READCAPACITY\r\n");
    #endif

    cbRC = (MSDSCSI_ReadCapacity_TypeDef*) &pCbw->CBWCB;

    if ((cbRC->Pmi == 0) && (cbRC->Lba == 0))
    {
      ReadCapData.LogicalBlockAddress = __REV(MSDDMEDIA_GetSectorCount() - 1);

      // ReadCapData.LogicalBlockAddress = __REV(1 - 1);
      ReadCapData.LogicalBlockLength  = __REV(SECTOR_SIZE);

      pCmdStatus->valid     = true;
      pCmdStatus->direction = DIR_DATA_IN;
      pCmdStatus->pData     = (uint8_t*) &ReadCapData;
      pCmdStatus->xferLen   = SCSI_READCAPACITYDATA_LEN;
    }
    break;

  case SCSI_READ10:
    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SCSI_READ10\r\n");
    #endif
      // USBD_Disconnect();
    cbR10 = (MSDSCSI_Read10_TypeDef*) &pCbw->CBWCB;
    pCmdStatus->direction = DIR_DATA_IN;
    pCmdStatus->valid     = MSDDMEDIA_CheckAccess(pCmdStatus,
                                                  __REV(cbR10->Lba),
                                                  __REV16(cbR10->TransferLength));
    break;

  case SCSI_WRITE10:
    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SCSI_WRITE10\r\n");
    #endif
      // USBD_Disconnect();
    cbW10 = (MSDSCSI_Write10_TypeDef*) &pCbw->CBWCB;

    pCmdStatus->direction = DIR_DATA_OUT;
    pCmdStatus->valid     = MSDDMEDIA_CheckAccess(pCmdStatus,
                                                  __REV(cbW10->Lba),
                                                  __REV16(cbW10->TransferLength));
    break;

  case SCSI_TESTUNIT_READY:
    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SCSI_TESTUNIT_READY\r\n");
    #endif
    pCmdStatus->valid     = true;
    pCmdStatus->direction = pCbw->Direction;
    pCmdStatus->xferLen   = 0;
      // USBD_Disconnect();
    break;

  case SCSI_STARTSTOP_UNIT:
    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SCSI_STARTSTOP_UNIT\r\n");
    #endif
    pCmdStatus->valid     = true;
    pCmdStatus->direction = pCbw->Direction;
    pCmdStatus->xferLen   = 0;
    pCmdStatus->xferType  = XFER_INDIRECT;
      // USBD_Disconnect();
    break;
  }

  if (!pCmdStatus->valid)
  {
    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "INVALID\r\n");
    #endif
    pCmdStatus->xferLen   = 0;
    pCmdStatus->direction = pCbw->Direction;
    pSenseData            = (MSDSCSI_RequestSenseData_TypeDef*) &IllegalSenseData;
      // USBD_Disconnect();
  }
}


/**************************************************************************//**
 * @brief
 *   Start a USB bulk-in transfer to send a CSW back to host.
 *****************************************************************************/
static inline void SendCsw(void)
{
  #ifdef USB_DEBUG_MSDHANDLER
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SendCsw\r\n" );
  #endif
  // if ( ledPort != -1 )
  //   GPIO_PinOutToggle((GPIO_Port_TypeDef)ledPort, ledPin);

  if (pCsw->bCSWStatus == USB_CLASS_MSD_CSW_CMDFAILED)
  {
    USBD_Write(BULK_IN, (void*) &csw, CSW_LEN, cswCallback);
  }
  else
  {
    USBD_Write(BULK_IN, (void*) &csw, CSW_LEN, NULL);
  }
}


/**************************************************************************//**
 * @brief
 *   Called whenever a USB setup command is received.
 *   This function overrides standard CLEAR_FEATURE commands, and implements
 *   MSD class commands "Bulk-Only Mass Storage Reset" and "Get Max LUN".
 *
 * @param[in] setup
 *  Pointer to an USB setup packet.
 *
 * @return
 *  An appropriate status/error code. See USB_Status_TypeDef.
 *****************************************************************************/
int UsbSetupCmd(const USB_Setup_TypeDef *setup)
{
  int             retVal;
  static uint32_t tmp;

  retVal = USB_STATUS_REQ_UNHANDLED;

  /* Check if it is MSD class command: "Bulk-Only Mass Storage Reset" */

  if ((setup->Type == USB_SETUP_TYPE_CLASS) &&
      (setup->Direction == USB_SETUP_DIR_OUT) &&
      (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE) &&
      (setup->bRequest == USB_MSD_BOTRESET) &&
      (setup->wValue == 0) &&
      (setup->wIndex == 0) &&
      (setup->wLength == 0))
  {
    USBD_Write( 0, (void*)tmp, 0, NULL );
    USBD_UnStallEp(BULK_IN);
    USBD_UnStallEp(BULK_OUT);
    retVal = USB_STATUS_OK;
    if (msdState == MSDD_WAITFOR_RECOVERY)
    {
      msdState = MSDD_IDLE;
    }

    EnableNextCbw();
    msdState = MSDD_WAITFOR_CBW;      
    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "MSD Reset\r\n" );
    #endif
  }


  /* Check if it is MSD class command: "Get Max LUN" */

  else if ((setup->Type == USB_SETUP_TYPE_CLASS) &&
           (setup->Direction == USB_SETUP_DIR_IN) &&
           (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE) &&
           (setup->bRequest == USB_MSD_GETMAXLUN) &&
           (setup->wValue == 0) &&
           (setup->wIndex == 0) &&
           (setup->wLength == 1))
  {
    /* Only one LUN (i.e. no support for multiple LUN's). Reply "0". */
    tmp    = 0;
    retVal = USBD_Write(0, (void*) &tmp, 1, NULL);

    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "MSD: Get Max LUN\r\n" );
    #endif
    EnableNextCbw();
    msdState = MSDD_WAITFOR_CBW;
  }


  /* Check if it is a standard CLEAR_FEATURE endpoint command */

  else if ((setup->Type == USB_SETUP_TYPE_STANDARD) &&
           (setup->Direction == USB_SETUP_DIR_OUT) &&
           (setup->Recipient == USB_SETUP_RECIPIENT_ENDPOINT) &&
           (setup->bRequest == CLEAR_FEATURE) &&
           (setup->wValue == USB_FEATURE_ENDPOINT_HALT) &&
           (setup->wLength == 0))
  {
    if (((setup->wIndex & 0xFF) == BULK_OUT) ||
        ((setup->wIndex & 0xFF) == BULK_IN))
    {
      retVal = USB_STATUS_OK;

      /* Dont unstall ep's when waiting for reset recovery */
      if (msdState != MSDD_WAITFOR_RECOVERY)
      {
        retVal = USBD_UnStallEp(setup->wIndex & 0xFF);

        if ((setup->wIndex & 0xFF) == BULK_IN)
        {
          if (msdState == MSDD_WAIT_FOR_INUNSTALLED)
          {
            SendCsw();
            EnableNextCbw();
            msdState = MSDD_WAITFOR_CBW;
          }
        }
        else
        {
          EnableNextCbw();
          msdState = MSDD_WAITFOR_CBW;
        }
      }

    #ifdef USB_DEBUG_MSD
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "MSD: CLEAR_FEATURE\r\n" );
    #endif
    }
  }

  return retVal;
}

/**************************************************************************//**
 * @brief
 *   Called whenever the USB device has changed its device state.
 *
 * @param[in] oldState
 *   The device USB state just leaved. See USBD_State_TypeDef.
 *
 * @param[in] newState
 *   New (the current) USB device state. See USBD_State_TypeDef.
 *****************************************************************************/
static void UsbStateChangeEvent(USBD_State_TypeDef oldState,
                                USBD_State_TypeDef newState)
{
  // emberSerialPrintf(SER232,"%s => %s\r\n", USBD_GetUsbStateName(oldState), USBD_GetUsbStateName(newState));
  if (newState == USBD_STATE_CONFIGURED)
  {
    /* We have been configured, start MSD functionality ! */
    EnableNextCbw();
    msdState = MSDD_WAITFOR_CBW;
    usbDisconnected = false;
  }

  else if ((oldState == USBD_STATE_CONFIGURED) &&
           (newState != USBD_STATE_SUSPENDED))
  {
    /* We have been de-configured */
    msdState = MSDD_IDLE;
  }

  else if (newState == USBD_STATE_SUSPENDED)
  {
    /* We have been suspended.                     */
    msdState = MSDD_IDLE;

    /* Reduce current consumption to below 2.5 mA. */
  }

  if ((oldState == USBD_STATE_CONFIGURED) && (newState!= USBD_STATE_CONFIGURED))
  {
    usbDisconnected = true;
  }
}

/**************************************************************************//**
 * @brief
 *   Start a USB bulk-in or bulk-out transfer to transfer a data payload
 *   to/from host.
 *
 * @param[in] data
 *   Pointer to data payload.
 *
 * @param[in] len
 *   Number of bytes to transfer.
 *
 * @param[in] cb
 *   Transfer completion callback.
 *****************************************************************************/
static void UsbXferBotData(uint8_t *data, uint32_t len,
                           USB_XferCompleteCb_TypeDef cb)
{
  #ifdef USB_DEBUG_MSD
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "UsbXferBotData\r\n" );
  #endif
  if (pCmdStatus->direction)
  {
    USBD_Write(BULK_IN, data, len, cb);
  }
  else
  {
    USBD_Read(BULK_OUT, data, len, cb);
  }
}

/**************************************************************************//**
 * @brief
 *   Start a USB bulk-in or bulk-out transfer to transfer a data payload
 *   to/from host according to the transfer mode of the transfer.
 *
 * @param[in] len
 *   Number of bytes to transfer.
 *****************************************************************************/
static void XferBotData(uint32_t length)
{
  #ifdef USB_DEBUG_MSD
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "XferBotData\r\n" );
  #endif
  pCmdStatus->xferLen   = length;
  pCsw->dCSWDataResidue = pCbw->dCBWDataTransferLength;
  if (pCmdStatus->xferType == XFER_INDIRECT)
  {
    /* Access media in "background" polling loop, i.e. in MSDD_Handler() */
    savedState = msdState;
    msdState   = MSDD_ACCESS_INDIRECT;
  }
  else
  {
    UsbXferBotData(pCmdStatus->pData,
                   EFM32_MIN(length, pCmdStatus->maxBurst),
                   XferBotDataCallback);
  }
}

/**************************************************************************//**
 * @brief
 *   Called on USB transfer completion callback for memory-mapped transfers.
 *   Will initiate a new transfer if there is more data available.
 *   If all data has been sent it will either send a CSW or stall the bulk-in
 *   endpoint if needed.
 *
 * @param[in] status
 *   The transfer status.
 *
 * @param[in] xferred
 *   Number of bytes actually transferred.
 *
 * @param[in] remaining
 *   Number of bytes not transferred.
 *
 * @return
 *   USB_STATUS_OK.
 *****************************************************************************/
static int XferBotDataCallback(USB_Status_TypeDef status,
                               uint32_t xferred, uint32_t remaining)
{
  #ifdef USB_DEBUG_MSD
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "XferBotDataCallback\r\n" );
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "pCmdStatus->xferLen:%d, xferred:%d\r\n",pCmdStatus->xferLen, xferred);
  #endif
  (void) status;
  (void) remaining;
  pCmdStatus->xferLen   -= xferred;
  pCsw->dCSWDataResidue -= xferred;


  if (pCmdStatus->xferLen)
  {
    pCmdStatus->pData += xferred;
    
    UsbXferBotData(pCmdStatus->pData,
                   EFM32_MIN(pCmdStatus->xferLen, pCmdStatus->maxBurst),
                   XferBotDataCallback);
  }
  else
  {
    if (msdState == MSDD_SEND_CSW)
    {
      SendCsw();
      EnableNextCbw();
      msdState = MSDD_WAITFOR_CBW;
    }

    else if (msdState == MSDD_STALL_IN)
    {
      delayedUnStall();
      EnableNextCbw();
      msdState = MSDD_WAITFOR_CBW;
    }
  }

  return USB_STATUS_OK;
}

/**************************************************************************//**
 * @brief
 *   Called on USB transfer completion callback for indirect access media.
 *   Signals MSD state change back to MSDD_Handler().
 *
 * @param[in] status
 *   The transfer status.
 *
 * @param[in] xferred
 *   Number of bytes actually transferred.
 *
 * @param[in] remaining
 *   Number of bytes not transferred.
 *
 * @return
 *   USB_STATUS_OK.
 *****************************************************************************/
static int XferBotDataIndirectCallback(USB_Status_TypeDef status,
                                       uint32_t xferred, uint32_t remaining)
{
  (void) status;
  (void) remaining;

  pCmdStatus->xferLen   -= xferred;
  pCsw->dCSWDataResidue -= xferred;

  if (pCmdStatus->direction)
  {
    pCmdStatus->lba += xferred / SECTOR_SIZE;
    msdState         = MSDD_ACCESS_INDIRECT;
  }
  else
  {
    msdState = MSDD_WRITE_INDIRECT;
  }

  return USB_STATUS_OK;
}






/** @endcond */
#endif //USB_MSD

