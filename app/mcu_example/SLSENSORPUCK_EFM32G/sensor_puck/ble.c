/**************************************************************************//**
* @file
* @brief Bluetooth Low Energy driver

* @version 5.1.2

******************************************************************************
* @section License
* <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/

#include "em_device.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_leuart.h"
#include "rtcdriver.h"
#include "sensor_puck.h"
#include "ble.h"
#include "ble_uart.h"
#include "trace.h"


/* GPIO pins */
#define BLE_RESET_PORT            gpioPortA
#define BLE_RESET_PIN             1
#define BLE_WAKEUP_PORT           gpioPortA
#define BLE_WAKEUP_PIN            2
#define HOST_WAKEUP_PORT          gpioPortB
#define HOST_WAKEUP_PIN           11

/* Command codes */
#define CMD_READY                 0x00
#define CMD_RESET                 0x01
#define CMD_FACTORY_RESET         0x02
#define CMD_SLEEP                 0x03
#define CMD_SET_BAUDRATE          0x04
#define CMD_GET_BD_ADDR           0x05
#define CMD_SET_ADV_PARAM         0x06
#define CMD_SET_ADV_DATA          0x08
#define CMD_SET_SCAN_RSP_DATA     0x09
#define CMD_SET_TIMED_ADV         0x0A
#define CMD_ADV_ENABLE            0x0B
#define CMD_ADV_CHANGED           0x0C
#define CMD_GET_ADV_STATE         0x0D
#define CMD_AUTH_ENABLE           0x13
#define CMD_RUNTIME_SLEEP_MODE    0x21
#define CMD_RESPONSE              0x80
#define CMD_GET_BD_ADDR_RSP       0x81
#define CMD_GET_STATE_RSP         0x82

/* Result codes */
#define RC_SUCCESS                0
#define RC_FAILURE                1
#define RC_TIMEOUT                2
#define RC_UNKNOWN                255

/* Timeouts */
#define TIMEOUT_AWAKE             20
#define TIMEOUT_RESULT            200
#define TIMEOUT_READY             2000

/* Advertising state */
#define ADV_OFF                   0
#define ADV_ON                    1
#define ADV_HI                    2
#define ADV_LO                    3
#define ADV_UNKNOWN               255

/* BLE state */
#define BLE_ACTIVE                0
#define BLE_SLEEPING              1
#define BLE_WAKING                2
#define BLE_UPDATING              3

/* BLE steps */
#define STEP_DISABLE              0
#define STEP_UPDATE               1
#define STEP_ENABLE               2

#define BLE_ADDR_SIZE             6
#define RSP_DATA_SIZE             13
#define ENV_DATA_SIZE             19
#define BIO_DATA_SIZE             23

#define INVALID                   0xFFFFFFFF
#define NULL                      ((void *) 0)

RTCDRV_TimerID_t        BLE_Timer = INVALID;

volatile bool           BLE_Ready       = false;
volatile bool           BLE_Initialized = false;
static volatile bool    BLE_Awake       = false;
volatile bool           BLE_Update_Mode = false;
volatile bool           BLE_Chg_Pending = false;
volatile bool           BLE_Settled     = false;
static volatile uint8_t BLE_Result      = RC_UNKNOWN;
static uint8_t          BLE_MsgID       = 0;
static int              BLE_State       = BLE_SLEEPING;
static int              BLE_Step        = STEP_DISABLE;
static uint8_t          BLE_Sequence    = 0;
static uint16_t         BLE_Sample[BLE_SAMPLE_COUNT];

static uint8_t          BLE_Address[BLE_ADDR_SIZE];

/* Scan Response Data */
const uint8_t ScanRspData[RSP_DATA_SIZE] =
{
  0x0C, /* AD Length */
  0x09, /* AD Type = Complete Local Name */
  'S', 'i', 'l', 'a', 'b', 's', ' ', 'P', 'u', 'c', 'k'
};

/* Environmental Advertising Data */
static uint8_t EnvData[ENV_DATA_SIZE] =
{
  /*  0 */ 0x02,               /* AD Length */
  /*  1 */ 0x01,               /* AD Type = Flags */
  /*  2 */ 0x06,               /* Flags  = LE General Discoverable Mode, BR/EDR Not Supported */

  /*  3 */ ENV_DATA_SIZE - 4,  /* AD Length */
  /*  4 */ 0xFF,               /* AD Type = Manufacturer Specific Data */
  /*  5 */ 0x35,               /* Company ID LSB */
  /*  6 */ 0x12,               /* Company ID MSB */

  /*  7 */ ENVIRONMENTAL_MODE, /* Measurement Mode */
  /*  8 */ 0x00,               /* Sequence */
  /*  9 */ 0x00,               /* Address LSB */
  /* 10 */ 0x00,               /* Address MSB */
  /* 11 */ 0x00,               /* Humidity LSB */
  /* 12 */ 0x00,               /* Humidity MSB */
  /* 13 */ 0x00,               /* Temperature LSB */
  /* 14 */ 0x00,               /* Temperature MSB */
  /* 15 */ 0x00,               /* Ambient Light LSB */
  /* 16 */ 0x00,               /* Ambient Light MSB */
  /* 17 */ 0x00,               /* UV Light Index */
  /* 18 */ 0x00,               /* Battery Voltage */
};

/* Biometric Advertising Data */
static uint8_t BioData[BIO_DATA_SIZE] =
{
  /*  0 */ 0x02,              /* AD Length */
  /*  1 */ 0x01,              /* AD Type = Flags */
  /*  2 */ 0x06,              /* Flags  = LE General Discoverable Mode, BR/EDR Not Supported */

  /*  3 */ BIO_DATA_SIZE - 4, /* AD Length */
  /*  4 */ 0xFF,              /* AD Type = Manufacturer Specific Data */
  /*  5 */ 0x35,              /* Company ID LSB */
  /*  6 */ 0x12,              /* Company ID MSB */

  /*  7 */ BIOMETRIC_MODE,    /* Measurement Mode */
  /*  8 */ 0x00,              /* Sequence */
  /*  9 */ 0x00,              /* Address LSB */
  /* 10 */ 0x00,              /* Address MSB */
  /* 11 */ 0x00,              /* HRM State */
  /* 12 */ 0x00,              /* Heart Rate */
  /* 13 */ 0x00,              /* HRM Sample 1 LSB */
  /* 14 */ 0x00,              /* HRM Sample 1 MSB */
  /* 15 */ 0x00,              /* HRM Sample 2 LSB */
  /* 16 */ 0x00,              /* HRM Sample 2 MSB */
  /* 17 */ 0x00,              /* HRM Sample 3 LSB */
  /* 18 */ 0x00,              /* HRM Sample 3 MSB */
  /* 19 */ 0x00,              /* HRM Sample 4 LSB */
  /* 20 */ 0x00,              /* HRM Sample 4 MSB */
  /* 21 */ 0x00,              /* HRM Sample 5 LSB */
  /* 22 */ 0x00,              /* HRM Sample 5 MSB */
};

static void BLE_SetAdvertisingData(uint8_t Data[], int Length);

/* I do want to do aliasing and I don't want to see any warnings about it */
//#pragma GCC diagnostic ignored "-Wstrict-aliasing"


/**************************************************************************//**
 * @brief  BLE_UpdateAdvertisingData function
 *****************************************************************************/
static void BLE_UpdateAdvertisingData()
{
  if (measurementMode == ENVIRONMENTAL_MODE)
  {
    EnvData[8]  = BLE_Sequence++;
    EnvData[9]  = BLE_Address[0];
    EnvData[10] = BLE_Address[1];
    EnvData[11] = (uint8_t)(rhData / 100);
    EnvData[12] = (uint8_t)((rhData / 100)>>8);
    EnvData[13] = (int8_t)(tempData / 100);
    EnvData[14] = (int8_t)((tempData / 100)>>8);
    EnvData[15] = (uint8_t)alsData;
    EnvData[16] = (uint8_t)(alsData>>8);
    EnvData[17] = (uint8_t)uvData;
    EnvData[18] = (uint8_t)(vBat / 100);

    BLE_SetAdvertisingData(EnvData, ENV_DATA_SIZE);
  }
  else  /* Biometric mode */
  {
    BioData[8]  = BLE_Sequence++;
    BioData[9]  = BLE_Address[0];
    BioData[10] = BLE_Address[1];
    BioData[11] = (uint8_t) HRMState;
    BioData[12] = (uint8_t) HRMRate;
    BioData[13] = (uint8_t)BLE_Sample[0];
    BioData[14] = (uint8_t)(BLE_Sample[0]>>8);
    BioData[15] = (uint8_t)BLE_Sample[1];
    BioData[16] = (uint8_t)(BLE_Sample[1]>>8);
    BioData[17] = (uint8_t)BLE_Sample[2];
    BioData[18] = (uint8_t)(BLE_Sample[2]>>8);
    BioData[19] = (uint8_t)BLE_Sample[3];
    BioData[20] = (uint8_t)(BLE_Sample[3]>>8);
    BioData[21] = (uint8_t)BLE_Sample[4];
    BioData[22] = (uint8_t)(BLE_Sample[4]>>8);

    BLE_SetAdvertisingData(BioData, BIO_DATA_SIZE);
  }
}


/**************************************************************************//**
 * @brief  BLE_OnMsgReceived function
 *****************************************************************************/
void BLE_OnMsgReceived(uint8_t Msg[])
{
  int x;

  switch (Msg[1])
  {
  case CMD_READY:
    BLE_Ready = true;
    break;
  case CMD_RESPONSE:
    if (Msg[0] == BLE_MsgID)
    {
      BLE_MsgID++;
      BLE_Result = Msg[2];
    }
    break;
  case CMD_GET_BD_ADDR_RSP:
    for (x = 0; x < BLE_ADDR_SIZE; x++)
      BLE_Address[x] = Msg[x + 2];
    if (Msg[0] == BLE_MsgID)
    {
      BLE_MsgID++;
      BLE_Result = RC_SUCCESS;
    }
    break;
  case CMD_ADV_CHANGED:
    break;
  case CMD_GET_STATE_RSP:
    if (Msg[0] == BLE_MsgID)
    {
      BLE_MsgID++;
      BLE_Result = RC_SUCCESS;
    }
    break;
  }
}


/**************************************************************************//**
 * @brief  OnTimeout function
 *****************************************************************************/
static void OnTimeout(RTCDRV_TimerID_t id, void *user)
{
#ifdef TRACE
 int x;   /* Trace */

 TraceDWord( 0xCCCCCCCC );
 for ( x=0; x<20; x++ )
  TraceByte( 0x00 );
#endif
  (void) id;
  (void) user;
  BLE_MsgID++;
  BLE_Result = RC_TIMEOUT;
}


/**************************************************************************//**
 * @brief BLE_OnHostWakeup function
 *****************************************************************************/
void BLE_OnHostWakeup()
{
  BLE_Awake = true;
}


/**************************************************************************//**
 * @brief  BLE_Wakeup function
 *****************************************************************************/
static void BLE_Wakeup()
{
  BLE_Awake = false;

  /* Wake up the BLE */
  GPIO_PinOutSet(BLE_WAKEUP_PORT, BLE_WAKEUP_PIN);

  /* Wait for the BLE to wake up */
  RTCDRV_StartTimer(BLE_Timer, rtcdrvTimerTypeOneshot, TIMEOUT_AWAKE,
                    OnTimeout, NULL);
  BLE_Result = RC_UNKNOWN;
  while (!BLE_Awake && (BLE_Result != RC_TIMEOUT))
    ;
  RTCDRV_StopTimer(BLE_Timer);

  /* Release the wakeup line */
  GPIO_PinOutClear(BLE_WAKEUP_PORT, BLE_WAKEUP_PIN);
}


/**************************************************************************//**
 * @brief  WaitForResult function
 *****************************************************************************/
static void WaitForResult()
{
  RTCDRV_StartTimer(BLE_Timer, rtcdrvTimerTypeOneshot, TIMEOUT_RESULT,
                    OnTimeout, NULL);

  BLE_Result = RC_UNKNOWN;
  while (BLE_Result == RC_UNKNOWN)
    ;

  RTCDRV_StopTimer(BLE_Timer);
}


/**************************************************************************//**
 * @brief  BLE_Reset function
 *****************************************************************************/
static void BLE_Reset()
{
  uint8_t Msg[2];

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_RESET;

  BLE_UART_Send(Msg, 2);

  BLE_Ready       = false;
  BLE_Initialized = false;
}

/**************************************************************************//**
 * @brief  BLE_GetAddress function
 *****************************************************************************/
static void BLE_GetAddress()
{
  uint8_t Msg[2];

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_GET_BD_ADDR;

  BLE_UART_Send(Msg, 2);
  WaitForResult();
}


/**************************************************************************//**
 * @brief  BLE_SetBaudrate function
 *****************************************************************************/
static void BLE_SetBaudrate(uint32_t Baudrate)
{
  uint8_t Msg[6];

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_SET_BAUDRATE;
  Msg[2] = (uint8_t)Baudrate;
  Msg[3] = (uint8_t)(Baudrate>>8);

  BLE_UART_Send(Msg, 6);
  WaitForResult();
}


/**************************************************************************//**
 * @brief  BLE_SetAdvertisingData function
 *****************************************************************************/
static void BLE_SetAdvertisingData(uint8_t Data[], int Length)
{
  uint8_t Msg[34];
  int     x;

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_SET_ADV_DATA;
  Msg[2] = 0;  /* No encryption */

  for (x = 0; x < Length; x++)
    Msg[x + 3] = Data[x];

  BLE_UART_Send(Msg, Length + 3);
}


/**************************************************************************//**
 * @brief  BLE_SetScanResponseData function
 *****************************************************************************/
static void BLE_SetScanResponseData(const uint8_t Data[], int Length)
{
  uint8_t Msg[34];
  int     x;

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_SET_SCAN_RSP_DATA;
  Msg[2] = 0;  /* No encryption */

  for (x = 0; x < Length; x++)
    Msg[x + 3] = Data[x];

  BLE_UART_Send(Msg, Length + 3);
  WaitForResult();
}


/**************************************************************************//**
 * @brief  BLE_EnableAdvertising function
 *****************************************************************************/
static void BLE_EnableAdvertising(uint8_t Enable, uint8_t Connectable)
{
  uint8_t Msg[4];

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_ADV_ENABLE;
  Msg[2] = Enable;
  Msg[3] = Connectable;

  BLE_UART_Send(Msg, 4);
}


/**************************************************************************//**
 * @brief  BLE_SetTimedAdvertising function
 *****************************************************************************/
static void BLE_SetTimedAdvertising(uint8_t Enable)
{
  uint8_t Msg[3];

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_SET_TIMED_ADV;
  Msg[2] = Enable;

  BLE_UART_Send(Msg, 3);
  WaitForResult();
}

/**************************************************************************//**
 * @brief  BLE_SetAdvertisingParameters function
 *****************************************************************************/
static void BLE_SetAdvertisingParameters(uint16_t HiInterval, uint16_t HiDuration,
                                         uint16_t LoInterval, uint16_t LoDuration)
{
  uint8_t Msg[10];

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_SET_ADV_PARAM;
  Msg[2] = (uint8_t)HiInterval;       /* 32 - 16384 slots (1 slot = 625 us) */
  Msg[3] = (uint8_t)(HiInterval>>8);
  Msg[4] = (uint8_t)HiDuration;       /* 0  - 3600  sec   (1 hour max)      */
  Msg[5] = (uint8_t)(HiDuration>>8);
  Msg[6] = (uint8_t)LoInterval;       /* 32 - 16384 slots (1 slot = 625 us) */
  Msg[7] = (uint8_t)(LoInterval>>8);
  Msg[8] = (uint8_t)LoDuration;       /* 0  - 3600  sec   (1 hour max)      */
  Msg[9] = (uint8_t)(LoDuration>>8);

  BLE_UART_Send(Msg, 10);
  WaitForResult();
}


/**************************************************************************//**
 * @brief  BLE_SetRuntimeSleepMode function
 *****************************************************************************/
static void BLE_SetRuntimeSleepMode(uint8_t Enable)
{
  uint8_t Msg[3];

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_RUNTIME_SLEEP_MODE;
  Msg[2] = Enable;

  BLE_UART_Send(Msg, 3);
  WaitForResult();
}

/**************************************************************************//**
 * @brief  BLE_SetAuthentication function
 *****************************************************************************/
static void BLE_SetAuthentication(uint8_t Enable)
{
  uint8_t Msg[3];

  Msg[0] = BLE_MsgID;
  Msg[1] = CMD_AUTH_ENABLE;
  Msg[2] = Enable;

  BLE_UART_Send(Msg, 3);
  WaitForResult();
}


/**************************************************************************//**
 * @brief  BLE_GetReady function
 *****************************************************************************/
static void BLE_GetReady()
{
  BLE_Wakeup();

  /* Try to disable runtime sleep mode at 9600 baud */
  BLE_SetRuntimeSleepMode(0);
  if (BLE_Result == RC_SUCCESS)
  {
    BLE_Update_Mode = false;
    BLE_Ready       = true;
    return;    /* Success! */
  }

  /* Set the LEUART to 115200 baud */
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);
  LEUART_BaudrateSet(LEUART0, 0, 115200);

  BLE_Wakeup();

  /* Try to disable runtime sleep mode at 115200 baud */
  BLE_SetRuntimeSleepMode(0);
  if (BLE_Result == RC_TIMEOUT)
  {
    /* Re-initialize the LEUART */
    BLE_UART_Init();

    return;    /* Failure */
  }

  /* Set the BLE to 9600 baud */
  BLE_SetBaudrate(9600);
  if (BLE_Result != RC_SUCCESS)
    return;

  /* BLE uses the new baudrate after reset */
  BLE_Reset();

  /* Set the LEUART to 9600 baud */
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
  LEUART_BaudrateSet(LEUART0, 0, 9600);

  /* Wait for the ready message or timeout */
  RTCDRV_StartTimer(BLE_Timer, rtcdrvTimerTypeOneshot, TIMEOUT_READY,
                    OnTimeout, NULL);
  BLE_Result = RC_UNKNOWN;
  while (!BLE_Ready && (BLE_Result != RC_TIMEOUT))
    ;
  RTCDRV_StopTimer(BLE_Timer);

  /* If BLE did not switch to 9600 baud then prepare for OTA FW update */
  if (!BLE_Ready)
  {
    /* Set the LEUART to 115200 baud */
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);
    LEUART_BaudrateSet(LEUART0, 0, 115200);

    /* Set the local name */
    BLE_SetScanResponseData(ScanRspData, RSP_DATA_SIZE);
    if (BLE_Result != RC_SUCCESS)
      return;

    /* Enable authentication */
    BLE_SetAuthentication(1);
    if (BLE_Result != RC_SUCCESS)
      return;

    /* Start connectable advertisements */
    BLE_EnableAdvertising(1, 1);
    WaitForResult();
    if (BLE_Result != RC_SUCCESS)
      return;

    /* Set the LEUART to 9600 baud */
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
    LEUART_BaudrateSet(LEUART0, 0, 9600);

    /* Waiting for the FW in the BLE to be updated OTA */
    BLE_Update_Mode = true;
  }
}


/**************************************************************************//**
 * @brief  BLE_Init function
 *****************************************************************************/
void BLE_Init()
{
  if (BLE_Timer == INVALID)
  {
    RTCDRV_AllocateTimer(&BLE_Timer);

    BLE_UART_Init();

    /* Enable GPIO for BLE_RESET on A1 */
    GPIO_PinModeSet(BLE_RESET_PORT,            /* GPIO port */
                    BLE_RESET_PIN,             /* GPIO port number */
                    gpioModePushPull,          /* Pin mode is set to push pull */
                    1);                        /* High idle state */    /* Normal  reset */

    /* Enable GPIO for BLE_WAKEUP on A2 */
    GPIO_PinModeSet(BLE_WAKEUP_PORT,           /* GPIO port */
                    BLE_WAKEUP_PIN,            /* GPIO port number */
                    gpioModePushPull,          /* Pin mode is set to push pull */
                    0);                        /* Low idle state */

    /* Enable GPIO for HOST_WAKEUP on B11 */
    GPIO_PinModeSet(HOST_WAKEUP_PORT,          /* Port */
                    HOST_WAKEUP_PIN,           /* Port number */
                    gpioModeInputPull,         /* Pin mode is set to input only, with pull direction given bellow */
                    0);                        /* Pull direction is set to pull-down */

    /* Configure HOST_WAKEUP to generate an interrupt on the rising edge */
    GPIO_IntConfig(HOST_WAKEUP_PORT, HOST_WAKEUP_PIN, true, false, true);
  }

  BLE_GetReady();

  if (BLE_Ready && !BLE_Initialized)
  {
    BLE_GetAddress();
    if (BLE_Result != RC_SUCCESS)
      return;

    BLE_SetScanResponseData(ScanRspData, RSP_DATA_SIZE);
    if (BLE_Result != RC_SUCCESS)
      return;

    BLE_SetTimedAdvertising(0);
    if (BLE_Result != RC_SUCCESS)
      return;

    BLE_Initialized = true;
    BLE_Initialized = BLE_OnMeasurementModeChange();
  }
}


/**************************************************************************//**
 * @brief  BLE_OnMeasurementModeChange function
 *****************************************************************************/
bool BLE_OnMeasurementModeChange()
{
  if (BLE_Initialized)
  {
    BLE_Chg_Pending = true;

    /* If the BLE state machine is not in the middle of something */
    if (BLE_State == BLE_ACTIVE || BLE_State == BLE_SLEEPING)
    {
      BLE_Settled = false;

      /* If switching to enviromental measurement mode */
      if (measurementMode == ENVIRONMENTAL_MODE)
      {
        BLE_SetAdvertisingParameters(800, 0, 800, 0);        /* 800 = 500 ms */
        if (BLE_Result != RC_SUCCESS)
          return false;

        BLE_EnableAdvertising(1, 0);
        WaitForResult();
        if (BLE_Result != RC_SUCCESS)
          return false;

        BLE_SetRuntimeSleepMode(1);
        if (BLE_Result != RC_SUCCESS)
          return false;

        BLE_State = BLE_SLEEPING;
      }
      else    /* Switching to biometric measurement mode */
      {
        BLE_Wakeup();
        BLE_SetRuntimeSleepMode(0);
        if (BLE_Result != RC_SUCCESS)
          return false;

        BLE_SetAdvertisingParameters(1, 0, 1, 0);        /* 1 = 625 us */
        if (BLE_Result != RC_SUCCESS)
          return false;

        BLE_State = BLE_ACTIVE;
        BLE_Step  = STEP_DISABLE;
      }

      BLE_Chg_Pending = false;
    }
  }

  return true;   /* Success */
}


/**************************************************************************//**
 * @brief  BLE_Update function
 *****************************************************************************/
void BLE_Update()
{
  int x;

  if (!BLE_Initialized)
  {
    BLE_Init();
    if (!BLE_Initialized)
      return;
  }

  switch (BLE_State)
  {
  case BLE_ACTIVE:
    if (bluetoothDataPending)
    {
      switch (BLE_Step)
      {
      case STEP_DISABLE:
        for (x = 0; x < BLE_SAMPLE_COUNT; x++)
          BLE_Sample[x] = HRMSample[x];
        BLE_EnableAdvertising(0, 0);
        BLE_Step = STEP_UPDATE;
        break;
      case STEP_UPDATE:
        BLE_UpdateAdvertisingData();
        BLE_Step = STEP_ENABLE;
        break;
      case STEP_ENABLE:
        BLE_EnableAdvertising(1, 0);
        BLE_Step             = STEP_DISABLE;
        bluetoothDataPending = false;
        break;
      }

      /* Wait for the result */
      RTCDRV_StartTimer(BLE_Timer, rtcdrvTimerTypeOneshot,
                        TIMEOUT_RESULT, OnTimeout, NULL);
      BLE_Result = RC_UNKNOWN;
      BLE_State  = BLE_UPDATING;
    }
    break;
  case BLE_SLEEPING:
    if (bluetoothDataPending)
    {
      BLE_Awake = false;

      /* Wake up the BLE */
      GPIO_PinOutSet(BLE_WAKEUP_PORT, BLE_WAKEUP_PIN);

      /* Wait for the BLE to wake up */
      RTCDRV_StartTimer(BLE_Timer, rtcdrvTimerTypeOneshot,
                        TIMEOUT_AWAKE, OnTimeout, NULL);
      BLE_Result = RC_UNKNOWN;
      BLE_State  = BLE_WAKING;
    }
    break;
  case BLE_WAKING:
    if (BLE_Awake)
    {
      /* Release the wakeup line */
      GPIO_PinOutClear(BLE_WAKEUP_PORT, BLE_WAKEUP_PIN);

      /* Update the advertising data */
      BLE_UpdateAdvertisingData();
      bluetoothDataPending = false;

      /* Wait for the result */
      RTCDRV_StartTimer(BLE_Timer, rtcdrvTimerTypeOneshot,
                        TIMEOUT_RESULT, OnTimeout, NULL);
      BLE_Result = RC_UNKNOWN;
      BLE_State  = BLE_UPDATING;
    }
    else if (BLE_Result == RC_TIMEOUT)
    {
      /* Release the wakeup line */
      GPIO_PinOutClear(BLE_WAKEUP_PORT, BLE_WAKEUP_PIN);

      /* Retry */
      BLE_State = BLE_SLEEPING;
    }
    break;
  case BLE_UPDATING:
    if (BLE_Result != RC_UNKNOWN)
    {
      if (BLE_Result == RC_TIMEOUT)
      {
        if (BLE_Settled)
          CalibrateLFRCO();
      }
      else       /* Success */
      {
        RTCDRV_StopTimer(BLE_Timer);
        BLE_Settled = true;
      }

      if (measurementMode == ENVIRONMENTAL_MODE)
        BLE_State = BLE_SLEEPING;
      else
        BLE_State = BLE_ACTIVE;

      if (BLE_Chg_Pending)
        BLE_OnMeasurementModeChange();
    }
    break;
  }
}



