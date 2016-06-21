/** @file si114xhrm.h
 *  @brief Main HRM header file for si114xhrm library.
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *  This software is confidential and proprietary information owned by Silicon
 *  Labs which is subject to worldwide copyright protection.  No use or other
 *  rights associated with this software may exist other than as expressly
 *  stated by written agreement with Silicon Labs.  No licenses to the
 *  intellectual property rights included in the software, including patent and
 *  trade secret rights, are otherwise given by implication, estoppel or otherwise.
 *  This contains the prototypes and datatypes needed
 *  to use the si114xhrm library.
 */
#ifndef SI114XHRM_H__

#define	SI114XHRM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "si114x_types.h"

/**
 * Success return value
 */
#define	SI114xHRM_SUCCESS							0
#define	SI114xHRM_ERROR_EVB_NOT_FOUND				-1
#define	SI114xHRM_ERROR_INVALID_MEASUREMENT_RATE	-2
#define	SI114xHRM_ERROR_SAMPLE_QUEUE_EMPTY			-3
#define	SI114xHRM_ERROR_INVALID_PART_ID				-4
#define	SI114xHRM_ERROR_FUNCTION_NOT_SUPPORTED      -5
#define SI114xHRM_ERROR_BAD_POINTER                 -6



// The HRM status values are a bit field. More than one status can be 'on' at any given time.
#define SI114xHRM_STATUS_SUCCESS					0
#define SI114xHRM_STATUS_FINGER_OFF					(1<<0)
#define SI114xHRM_STATUS_FINGER_ON					(1<<1)
#define SI114xHRM_STATUS_ZERO_CROSSING_INVALID		(1<<2)
#define SI114xHRM_STATUS_BPF_PS_VPP_OFF_RANGE		(1<<3)
#define SI114xHRM_STATUS_AUTO_CORR_TOO_LOW			(1<<4)
#define SI114xHRM_STATUS_CREST_FACTOR_TOO_HIGH		(1<<5)
#define SI114xHRM_STATUS_FRAME_PROCESSED			(1<<6)
#define SI114xHRM_STATUS_AUTO_CORR_MAX_INVALID		(1<<7)
#define SI114xHRM_STATUS_HRM_MASK					0x00ff	//Include all HRM bits
#define SPO2_STATUS_PROCESS_SPO2_FRAME				1		//Used to inform to process the SpO2 frame.
#define SI114xHRM_STATUS_SPO2_FINGER_OFF			(1<<8)
#define SI114xHRM_STATUS_SPO2_FINGER_ON			(1<<9)
#define SI114xHRM_STATUS_SPO2_CREST_FACTOR_OFF		(1<<10)
#define SI114xHRM_STATUS_SPO2_TOO_LOW_AC			(1<<11)
#define SI114xHRM_STATUS_SPO2_TOO_HIGH_AC			(1<<12)
#define SI114xHRM_STATUS_SPO2_EXCEPTION			(1<<13)
#define SI114xHRM_STATUS_SPO2_MASK					0xff00	//Include all SpO2 bits

#define	SI114xHRM_MAX_INTERPOLATION_FACTOR		9

typedef struct Si114xhrmData
{
	int16_t Fs;
	int16_t hrIframe;
	int16_t hrUpdateInterval;
	uint16_t hrmRawPs;
	uint16_t hrmPs;
	uint16_t hrmInterpolatedPs[SI114xHRM_MAX_INTERPOLATION_FACTOR];		//this is usefule for displaying the waverform
	int16_t hrmPsVppLow;
	int16_t hrmPsVppHigh;
	int16_t hrmCrestFactor;
	uint16_t hrmAdcgainCurrent;	//HRM AGC: ADCgain<<12+current[2]<<8+current[1]<<4+current[0]
	uint8_t	hrmNumSamples;		//each Process() may result in more than one sample due to interpolation
	int16_t hrmSamples[SI114xHRM_MAX_INTERPOLATION_FACTOR];		//samples after interpolation, filtering and normalization.  These are the sample used for HRM calculation
	uint16_t hrmDcSensingResult[15];  //For HRM DC sensing
	uint16_t hrmPerfusionIndex;
	uint16_t spo2RedDcSample;
	uint16_t spo2IrDcSample;
	uint16_t spo2DcToAcRatio;		//Peak To Peak Ratio
	int16_t spo2CrestFactor;
	uint16_t spo2IrPerfusionIndex;
	uint16_t spo2DcSensingResult[15*2];
} Si114xhrmData_t;

/**
 * Struct for hardware configuration parameters.
 */
typedef struct Si114xhrmConfiguration
{
	int16_t hrmSpo2PsSelect;  /**< bits 15:12 = unused, bits 11:8=SpO2 Red PS, bits 7:4=SpO2 Red PS, bits 3:0=HRM PS.  0=PS1, 1=PS2, 2=PS3. */
	int16_t ledCurrent;	      /**< bits 15:12 = unused, bits 11:8=led3 current, bits 7:4=led2 current, bits 3:0=led1 current */
    int16_t tasklist;
	int16_t psLedSelect;
    int16_t measurementRate;   /**< Always compressed. The algorithm will read uv_part from the chip and then program the chip with correct measurementRate accordingly.*/
	int16_t adcGain;
    int16_t adcMisc;
	int16_t adcMux;					//bits 15:12 = unused, bits 11:8=ps3, bits 7:4=ps2, bits 3:0=ps1
    int16_t psAlign;
} Si114xhrmConfiguration_t;

/**
 * Bytes needed in RAM for SPO2
 */
#define SPO2_DATA_SIZE 5744

/**
 * Number of bytes needed in RAM for HRM
 */
#define HRM_DATA_SIZE 1700

/**
 * Struct for SpO2 RAM storage
 */
typedef struct Si114xSpO2DataStorage
{
  uint8_t data[SPO2_DATA_SIZE]; /**< SPO2_DATA_SIZE bytes allocated */
} Si114xSpO2DataStorage_t;

/**
 * Struct for HRM only RAM storage
 */
typedef struct Si114xhrmDataStorage
{
  uint8_t data[HRM_DATA_SIZE];  /**< HRM_DATA_SIZE bytes allocated */

} Si114xhrmDataStorage_t;

/**
 * Struct for passing allocated RAM locations to HRM library
 */
typedef struct Si114xDataStorage
{
	Si114xSpO2DataStorage_t *spo2;  /**< Pointer to SpO2 RAM */
	Si114xhrmDataStorage_t *hrm;    /**< Pointer to HRM RAM */
} Si114xDataStorage_t;

/**
 * HRM handle type
 */
typedef void * Si114xhrmHandle_t;

/**************************************************************************//**
 * @brief
 *	Initialize the Si114x device.
 *
 * @param[in] portName
 *	Platform specific data to specify the i2c port information.
 *
 * @param[in] options
 *	Initialization options flags.
 *
 * @param[in] handle
 *	Pointer to si114xhrm handle
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
int32_t si114xhrm_Initialize(void *portName, int32_t options, Si114xhrmHandle_t *handle);

/**************************************************************************//**
 * @brief
 *	Close the Si114x device
 *
 * @param[in] handle
 *	Pointer to si114xhrm handle
 *
 * @return
 *	Returns error status
 *****************************************************************************/
int32_t si114xhrm_Close(Si114xhrmHandle_t handle);

/**************************************************************************//**
 * @brief
 *	Configure si114xhrm debugging mode.
 *
 * @param[in] handle
 *	Pointer to si114xhrm handle
 *
 * @param[in] debug
 *	Pointer to debug status
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
int32_t si114xhrm_SetupDebug(Si114xhrmHandle_t handle, void *debug);

/**************************************************************************//**
 * @brief
 *	Find EVB (windows demo only)
 *****************************************************************************/
int32_t si114xhrm_FindEvb(int8_t *portDescription, int8_t *lastPort, int32_t *numPortsFound);

/**************************************************************************//**
 * @brief
 *	Output debug message
 *
 * @param[in] handle
 *	Pointer to si114xhrm handle
 *
 * @param[in] message
 *	Message data
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
int32_t si114xhrm_OutputDebugMessage(Si114xhrmHandle_t handle, int8_t * message);

/**************************************************************************//**
 * @brief
 *	Configure si114x
 *****************************************************************************/
int32_t si114xhrm_ConfigureDiscretes(Si114xhrmHandle_t _handle, int16_t hrmSpo2PsSelect, int16_t *current, int16_t taskList, int16_t psLedSelect, int16_t measurementRate, int16_t adcGain, int16_t adcMisc, int16_t *adcMux, int16_t psAlign);

int32_t si114xhrm_Configure(Si114xhrmHandle_t handle, Si114xhrmConfiguration_t *configuration);


/**************************************************************************//**
 * @brief
 *	HRM process engine.  This function should be called at least once per sample
 *
 * @param[in] _handle
 *	SiHRM handle
 *
 * @param[out] heartRate
 *	Pointer to a location where this function will return the heart rate result
 *
 * @param[out] spo2
 *	Pointer to a location where this function will return the spo2 result.  
 *
 * @param[out] hrmStatus
 *	Pointer to a integer where this function will report status flags
 *
 * @param[out] hrmData
 *	Optional pointer to a Si114xhrmData_t structure where this function will 
 *  return auxiliary data useful for the application.  If the application is not 
 *  interested in this data it may pass NULL to this parameter.
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
 int32_t si114xhrm_Process(Si114xhrmHandle_t handle, int16_t *heartRate, int16_t *spo2, int32_t *hrmStatus, Si114xhrmData_t *hrmData);

/**************************************************************************//**
 * @brief
 *	Start the Si114x autonomous measurement operation.
 *  The device must be configured before calling this function.
 *
 * @param[in] _handle
 *	Si114x HRM handle
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
int32_t si114xhrm_Run(Si114xhrmHandle_t handle);

/**************************************************************************//**
 * @brief
 *	Pause the Si114x autonomous measurement operation.
 *  HRM must be running before calling this function.
 *
 * @param[in] _handle
 *	Si114x HRM handle
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
int32_t si114xhrm_Pause(Si114xhrmHandle_t handle);

/**************************************************************************//**
 * @brief
 *	Check for skin contact
 *
 * @param[in] _handle
 *	Si114x HRM handle
 *
 * @param[out] isContact
 *	Set to non-zero value if skin contact is found
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
int32_t si114xhrm_DetectSkinContact(Si114xhrmHandle_t handle, int32_t *isContact);

/**************************************************************************//**
 * @brief
 *	Check si114x device part number for UV register interface
 *
 * @param[in] _handle
 *	Si114x HRM handle
 *
 * @param[out] isUvDevice
 *	Set to non-zero value if UV device detected
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
int32_t si114xhrm_IsUvDevice(Si114xhrmHandle_t handle, int32_t *isUvDevice);

/**************************************************************************//**
 * @brief
 *	Returns algorithm version
 *
 * @param[out] revision
 *	String representing algorithm version number
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
int32_t si114xhrm_QuerySoftwareRevision(int8_t *revision);

/**************************************************************************//**
 * @brief
 *	Get low level i2c handle
 *
 * @param[in] _handle
 *	Si114x HRM handle
 *
 * @param[out] si114xHandle
 *	Low level i2c handle
 *
 * @return
 *	Returns error status.
 *****************************************************************************/
int32_t si114xhrm_GetLowLevelHandle(Si114xhrmHandle_t _handle, HANDLE *si114xHandle);


#ifdef __cplusplus
}
#endif

#endif		//SI114XHRM_H__
