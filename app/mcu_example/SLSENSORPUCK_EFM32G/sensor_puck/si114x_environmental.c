/**************************************************************************//**
* @file
* @brief Si114x UV & ALS measurement code
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

#include "si114x_functions.h"
#include "em_msc.h"
#include "em_core.h"

#define UV_TASKLIST                  0x80
#define UV_IRQ_EN                    IE_ALS_EVRYSAMPLE
#define SI1146_DEVICE_ID             0x46
#define SI1147_DEVICE_ID             0x47

#define AMBIENT_BLOCKED_THRESHOLD    500
#define DARK_CAL_ADDR                0x1f800
#define ALS_HISTORY_LENGTH           5

static uint32_t alsDark1  = 0, alsDark2 = 0, alsDark3 = 0, alsDark4 = 0;
static int      disableUV = 0;
static uint16_t alsDataHistory[ALS_HISTORY_LENGTH];
static int      alsHistoryIndex = 0;

/**************************************************************************//**
 * @brief  Sets flag to use non-UV capable sensor
 *****************************************************************************/
void Si114x_NoUV()
{
  disableUV = 1;
}

/**************************************************************************//**
 * @brief  Save ALS calibration to flash
 *****************************************************************************/
static void DarkCal_SaveToFlash(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4)
{
  CORE_DECLARE_IRQ_STATE;
  uint32_t *addr = (uint32_t *) DARK_CAL_ADDR;
  uint32_t data[4];
  data[0] = d1; data[1] = d2; data[2] = d3; data[3] = d4;
  CORE_ENTER_ATOMIC();
  MSC_Init();
  MSC_ErasePage(addr);
  MSC_WriteWord(addr, data, sizeof(data));
  MSC_Deinit();
  CORE_EXIT_ATOMIC();
}

/**************************************************************************//**
 * @brief  Read ALS calibration from flash
 *****************************************************************************/
static void DarkCal_ReadFromFlash(uint32_t* d1, uint32_t* d2, uint32_t* d3, uint32_t* d4)
{
  uint32_t *addr = (uint32_t *) DARK_CAL_ADDR;
  if (addr[0] == 0xffffffff)
  {
    *d1 = 0;
    *d2 = 0;
    *d3 = 0;
    *d4 = 0;
  }
  else
  {
    *d1 = addr[0];
    *d2 = addr[1];
    *d3 = addr[2];
    *d4 = addr[3];
  }
}

/**************************************************************************//**
 * @brief  Read ALS measurement results
 *****************************************************************************/
static void readALSData(HANDLE si114x_handle, int32_t *alsData, int32_t *alsIRData)
{
  uint8_t regval;
  uint8_t response;
  Si114xAlsForce(si114x_handle);    //This function will not return until the measurement is complete
  //check for saturation after the forced measurement and clear it if found
  //otherwise the next si114x cmd will not be performed
  response = Si114xReadFromRegister(si114x_handle, REG_RESPONSE);
  if ((response & 0x80) != 0)
  {
    // Send the NOP Command to clear the error...we cannot use Si114xNop()
    // because it first checks if REG_RESPONSE < 0 and if so it does not
    // perform the cmd. Since we have a saturation REG_RESPONSE will be <0
    Si114xWriteToRegister(si114x_handle, REG_COMMAND, 0x00);
    response   = Si114xReadFromRegister(si114x_handle, REG_RESPONSE);
    *alsData   = 0xffff;
    *alsIRData = 0xffff;
  }
  else
  {
    regval      = Si114xReadFromRegister(si114x_handle, REG_ALS_VIS_DATA0);    /*read sample data from si114x */
    *alsData    = regval;
    regval      = Si114xReadFromRegister(si114x_handle, REG_ALS_VIS_DATA1);
    *alsData   |= regval << 8;
    regval      = Si114xReadFromRegister(si114x_handle, REG_ALS_IR_DATA0);    /*read sample data from si114x */
    *alsIRData  = regval;
    regval      = Si114xReadFromRegister(si114x_handle, REG_ALS_IR_DATA1);
    *alsIRData |= regval << 8;
  }
}

/**************************************************************************//**
 * @brief Measure ALS calibration values
 *****************************************************************************/
void Si114x_MeasureDarkOffset(HANDLE si114x_handle)
{
  int32_t alsData;
  int32_t alsIRData;

  uint8_t savedChList;
  savedChList = Si114xParamRead(si114x_handle, PARAM_CH_LIST);
  Si114xParamSet(si114x_handle, PARAM_CH_LIST, ALS_VIS_TASK | ALS_IR_TASK);

  Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_MISC, 0);
  Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_MISC, 0);
  readALSData(si114x_handle, &alsData, &alsIRData);
  if (((uint32_t) alsData < alsDark1) || (alsDark1 == 0))
  {
    alsDark1 = alsData;
  }
  //increase gain
  Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_GAIN, 4);
  Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_GAIN, 0);
  readALSData(si114x_handle, &alsData, &alsIRData);
  if (((uint32_t) alsData < alsDark2) || (alsDark2 == 0))
  {
    alsDark2 = alsData;
  }
  //increase gain
  Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_GAIN, 7);
  Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_GAIN, 0);
  readALSData(si114x_handle, &alsData, &alsIRData);
  if (((uint32_t) alsData < alsDark3) || (alsDark3 == 0))
  {
    alsDark3 = alsData;
  }
  //decrease gain
  Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_GAIN, 0);
  Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_GAIN, 0);
  Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_MISC, RANGE_EN);
  Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_MISC, RANGE_EN);
  readALSData(si114x_handle, &alsData, &alsIRData);
  if (((uint32_t) alsData < alsDark4) || (alsDark4 == 0))
  {
    alsDark4 = alsData;
  }
  Si114xParamSet(si114x_handle, PARAM_CH_LIST, savedChList);
  DarkCal_SaveToFlash(alsDark1, alsDark2, alsDark3, alsDark4);
}

/**************************************************************************//**
 * @brief  Measure ALS by trying various gain structures
 *****************************************************************************/
static uint16_t measureAmbientLight(HANDLE si114x_handle, uint16_t *alsForFingerDetect)
{
  int32_t alsData;
  int32_t alsIRData;
  int32_t lux = 0;

  int32_t normalizedAls;

  Si114xParamSet(si114x_handle, PARAM_CH_LIST, ALS_VIS_TASK | ALS_IR_TASK);

  Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_MISC, 0);
  Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_MISC, 0);
  readALSData(si114x_handle, &alsData, &alsIRData);
  *alsForFingerDetect = alsData;
  normalizedAls       = (alsData - alsDark1) * 16 * 8;

  if (alsData <= 3000)
  {
    //increase gain
    Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_GAIN, 4);
    Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_GAIN, 0);
    readALSData(si114x_handle, &alsData, &alsIRData);
    if (alsData != 0xffff)
    {
      normalizedAls = (alsData - alsDark2) * 8;
    }
    if (alsData <= 2000)
    {
      //increase gain
      Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_GAIN, 7);
      Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_GAIN, 0);
      readALSData(si114x_handle, &alsData, &alsIRData);
      if (alsData != 0xffff)
      {
        normalizedAls = (alsData - alsDark3);
      }
    }
  }
  else if (alsData == 0xffff)
  {
    //decrease gain
    Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_GAIN, 0);
    Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_GAIN, 0);
    Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_MISC, RANGE_EN);
    Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_MISC, RANGE_EN);
    readALSData(si114x_handle, &alsData, &alsIRData);
    normalizedAls = (alsData - alsDark4) * 232 * 8;
  }
  lux = (396 * (normalizedAls) / 1000 / 8);

  lux >>= 1;
  if (lux > 0xffff)
    lux = 0xffff;
  if (lux < 0)
    lux = 0;
  return (uint16_t) lux;
}


/**************************************************************************//**
 * @brief
 *  Reads the UV index measurement data from the
 *  Si1147 and check for skin contact.
 * @param[in] i2C
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] uvIndex
 *   The UV index read from the sensor
 * @return
 *   Returns 1 if sensor covered.
 *****************************************************************************/
int Si114x_MeasureEnvironmental(HANDLE si114x_handle, uint16_t *uvIndex, uint16_t *ps1, uint16_t *als)
{
  u16     data;
  u16     ps1Data;
  u16     alsData;
  u16     alsFingerDetect;
  u8      regval;
  int     retval = 0;
  uint8_t savedChList;
  u8      response;
  int     i;
  u32     alsAvg;

  savedChList = Si114xParamRead(si114x_handle, PARAM_CH_LIST);
  if (disableUV)
    Si114xParamSet(si114x_handle, PARAM_CH_LIST, PS1_TASK);
  else
    Si114xParamSet(si114x_handle, PARAM_CH_LIST, PS1_TASK | 0x80);

  Si114xPsAlsForce(si114x_handle);
  /*wait for measurement data */
  //check for saturation after the forced measurement and clear it if found
  //otherwise the next si114x cmd will not be performed
  response = Si114xReadFromRegister(si114x_handle, REG_RESPONSE);
  while ((response & 0x80) != 0)
  { // Send the NOP Command to clear the error...we cannot use Si114xNop()
    // because it first checks if REG_RESPONSE < 0 and if so it does not
    // perform the cmd. Since we have a saturation REG_RESPONSE will be <0
    Si114xWriteToRegister(si114x_handle, REG_COMMAND, 0x00);
    response = Si114xReadFromRegister(si114x_handle, REG_RESPONSE);
  }
  regval   = Si114xReadFromRegister(si114x_handle, REG_AUX_DATA0); /*read sample data from si114x */
  data     = regval;
  regval   = Si114xReadFromRegister(si114x_handle, REG_AUX_DATA1);
  data    |= regval << 8;
  regval   = Si114xReadFromRegister(si114x_handle, REG_PS1_DATA0); /*read sample data from si114x */
  ps1Data  = regval;
  regval   = Si114xReadFromRegister(si114x_handle, REG_PS1_DATA1);
  ps1Data |= regval << 8;

  /*we average ALS readings over time*/
  alsData                           = measureAmbientLight(si114x_handle, &alsFingerDetect);
  alsDataHistory[alsHistoryIndex++] = alsData;
  if (alsHistoryIndex == ALS_HISTORY_LENGTH)
    alsHistoryIndex = 0;


  Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_MISC, RANGE_EN);
  Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_MISC, RANGE_EN);
  Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_GAIN, 0);
  Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_GAIN, 0);
  Si114xParamSet(si114x_handle, PARAM_CH_LIST, savedChList);

  /*round to nearest*/
  *uvIndex  = data + 50;
  *uvIndex /= 100;
  if (*uvIndex > 10)
    *uvIndex = 11;
  if (disableUV)
    *uvIndex = 20;
  *ps1 = ps1Data;

  alsAvg = 0;
  for (i = 0; i < ALS_HISTORY_LENGTH; i++)
    alsAvg += alsDataHistory[i];
  alsAvg = alsAvg / ALS_HISTORY_LENGTH;
  *als   = alsAvg;

  // if ambient light detected finger is not present
  if (alsFingerDetect > AMBIENT_BLOCKED_THRESHOLD)
    *ps1 = 0;

  /*clear irq*/
  Si114xWriteToRegister(si114x_handle, REG_IRQ_STATUS, 0xff);
  return retval;
}

/**************************************************************************//**
 * @brief
 *  Configures the Si114x sensor for UV index measurements.
 * @param[in] i2C
 *   The I2C peripheral to use (not used).
 * @return
 *   Returns 0 on success.
 *****************************************************************************/
int Si114x_ConfigureEnvironmental(HANDLE si114x_handle)
{
  int          retval = 0;
  SI114X_CAL_S si114x_cal;



  /* UV Coefficients */
  //si114x_handle not used!
  if (disableUV == 0)
  {
    si114x_get_calibration(si114x_handle, &si114x_cal, 0);
    si114x_set_ucoef(si114x_handle, 0, &si114x_cal);
  }
  /* Configure the ALS IR channel for the same settings as PS */

  retval  = Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_MISC, RANGE_EN);
  retval += Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_MISC, RANGE_EN);


  /* If nothing went wrong after all of this time, the value */
  /* returned will be 0. Otherwise, it will be some negative */
  /* number */
  DarkCal_ReadFromFlash(&alsDark1, &alsDark2, &alsDark3, &alsDark4);

  return retval;
}

/**************************************************************************//**
 * @brief  Sets Si114x for HRM mode
 *****************************************************************************/
int Si114x_ConfigureHRM(HANDLE si114x_handle)
{
  int retval;
  //HRM requires vis_range bit = 0
  retval  = Si114xParamSet(si114x_handle, PARAM_ALSVIS_ADC_MISC, 0);
  retval += Si114xParamSet(si114x_handle, PARAM_ALSIR_ADC_MISC, 0);
  return retval;
}
