/*********************************************************************************
	This software is confidential and proprietary information owned by Silicon
	Labs which is subject to worldwide copyright protection.  No use or other
	rights associated with this software may exist other than as expressly
	stated by written agreement with Silicon Labs.  No licenses to the
	intellectual property rights included in the software, including patent and
	trade secret rights, are otherwise given by implication, estoppel or otherwise.

	This file is the main HRM header file and is used with the library.
***********************************************************************************/

#ifndef SI114XHRM_H__

#define	SI114XHRM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "si114x_types.h"


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

typedef struct Si114xhrmConfiguration
{
	int16_t hrmSpo2PsSelect;		//bits 15:12 = unused, bits 11:8=SpO2 Red PS, bits 7:4=SpO2 Red PS, bits 3:0=HRM PS.  0=PS1, 1=PS2, 2=PS3.
	int16_t ledCurrent;			//bits 15:12 = unused, bits 11:8=led3 current, bits 7:4=led2 current, bits 3:0=led1 current
    int16_t tasklist;
	int16_t psLedSelect;
    int16_t measurementRate;				//Always compressed. The algorithm will read uv_part from the chip and then program the chip with correct measurementRate accordingly.
	int16_t adcGain;
    int16_t adcMisc;
	int16_t adcMux;					//bits 15:12 = unused, bits 11:8=ps3, bits 7:4=ps2, bits 3:0=ps1
    int16_t psAlign;
} Si114xhrmConfiguration_t;

#define SPO2_DATA_SIZE 5744
#define HRM_DATA_SIZE 1700

typedef struct Si114xSpO2DataStorage
{
  uint8_t data[SPO2_DATA_SIZE];
} Si114xSpO2DataStorage_t;

typedef struct Si114xhrmDataStorage
{
  uint8_t data[HRM_DATA_SIZE];
} Si114xhrmDataStorage_t;

typedef struct Si114xDataStorage
{
	Si114xSpO2DataStorage_t *spo2;
	Si114xhrmDataStorage_t *hrm;
} Si114xDataStorage_t;

typedef void * Si114xhrmHandle_t;

/* Exported Function Prototypes  */
int32_t si114xhrm_Initialize(void *portName, int32_t options, Si114xhrmHandle_t *handle);
int32_t si114xhrm_Close(Si114xhrmHandle_t handle);
int32_t si114xhrm_SetupDebug(Si114xhrmHandle_t handle, void *debug);
int32_t si114xhrm_FindEvb(int8_t *portDescription, int8_t *lastPort, int32_t *numPortsFound);
int32_t si114xhrm_OutputDebugMessage(Si114xhrmHandle_t handle, int8_t * message);
int32_t si114xhrm_ConfigureDiscretes(Si114xhrmHandle_t _handle, int16_t hrmSpo2PsSelect, int16_t *current, int16_t taskList, int16_t psLedSelect, int16_t measurementRate, int16_t adcGain, int16_t adcMisc, int16_t *adcMux, int16_t psAlign);

int32_t si114xhrm_Configure(Si114xhrmHandle_t handle, Si114xhrmConfiguration_t *configuration);


int32_t si114xhrm_Process(Si114xhrmHandle_t handle, int16_t *heartRate, int16_t *spo2, int32_t *hrmStatus, Si114xhrmData_t *hrmData);
int32_t si114xhrm_ProcessExternalSample(Si114xhrmHandle_t handle, int16_t *heartRate, int16_t *spo2, int32_t *hrmStatus, Si114xhrmData_t *hrmData, SI114X_IRQ_SAMPLE *samples);

int32_t si114xhrm_Run(Si114xhrmHandle_t handle);
int32_t si114xhrm_Pause(Si114xhrmHandle_t handle);
int32_t si114xhrm_DetectSkinContact(Si114xhrmHandle_t handle, int32_t *isContact);

int32_t si114xhrm_IsUvDevice(Si114xhrmHandle_t handle, int32_t *isUvDevice);
int32_t si114xhrm_QuerySoftwareRevision(int8_t *revision);
int32_t si114xhrm_GetLowLevelHandle(Si114xhrmHandle_t _handle, HANDLE *si114xHandle);


#ifdef __cplusplus
}
#endif

#endif		//SI114XHRM_H__
