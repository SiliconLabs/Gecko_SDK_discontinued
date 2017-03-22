// *****************************************************************************
// * i2c-driver-efr32.c
// *
// * Implements an I2C Driver plugin that can be used by multiple plugins
// * requiring common I2C support.
// *
// * NOTE: Functions are NOT RE-ENTRANT and CANNOT be called from ISR level. All
// *       functions MUST be called at main() (aka. event) level only!!!
// *
// * Copyright 2016 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "hal/plugin/i2c-driver/i2c-driver.h"

#include "i2cspmconfig.h"
#include "em_gpio.h"
#include "em_i2c.h"
#include "../../hardware/kit/common/drivers/i2cspm.h"

I2CSPM_Init_TypeDef i2cConfig = I2CSPM_INIT_DEFAULT;

void halI2cInitialize(void)
{
  I2CSPM_Init(&i2cConfig);
}

// This initialization callback is for compatibility with AFV2.
void emberAfPluginI2cDriverInitCallback(void)
{
  halI2cInitialize();
}

uint8_t halI2cWriteBytes(uint8_t address,
                         const uint8_t *buffer,
                         uint8_t count)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t halRetCode;
  
  seq.addr = address;
  seq.flags = I2C_FLAG_WRITE;
  seq.buf[0].data = (uint8_t*)buffer;
  seq.buf[0].len = (uint16_t)count;
  
  ret = I2CSPM_Transfer(i2cConfig.port, &seq);
    
  switch (ret) {
  case i2cTransferDone:
    halRetCode = I2C_DRIVER_ERR_NONE;
    break;
  case i2cTransferBusErr:
    halRetCode = I2C_DRIVER_ERR_ADDR_NAK;
    break;
  case i2cTransferArbLost:
    halRetCode = I2C_DRIVER_ERR_ARB_LOST;
    break;
  case i2cTransferUsageFault:
    halRetCode = I2C_DRIVER_ERR_USAGE_FAULT;
    break;
  case i2cTransferSwFault:
    halRetCode = I2C_DRIVER_ERR_SW_FAULT;
    break;
  default:
    halRetCode = I2C_DRIVER_ERR_UNKOWN;
    break;
  }

  return halRetCode;
}

uint8_t halI2cWriteBytesDelay(uint8_t address,
                              const uint8_t *buffer,
                              uint8_t count,
                              uint8_t delay)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t halRetCode;
  
  seq.addr = address;
  seq.flags = I2C_FLAG_WRITE;
  seq.buf[0].data = (uint8_t*)buffer;
  seq.buf[0].len = (uint16_t)count;
  
  ret = I2CSPM_Transfer(i2cConfig.port, &seq);
    
  switch (ret) {
  case i2cTransferDone:
    halRetCode = I2C_DRIVER_ERR_NONE;
    break;
  case i2cTransferBusErr:
    halRetCode = I2C_DRIVER_ERR_ADDR_NAK;
    break;
  case i2cTransferArbLost:
    halRetCode = I2C_DRIVER_ERR_ARB_LOST;
    break;
  case i2cTransferUsageFault:
    halRetCode = I2C_DRIVER_ERR_USAGE_FAULT;
    break;
  case i2cTransferSwFault:
    halRetCode = I2C_DRIVER_ERR_SW_FAULT;
    break;
  default:
    halRetCode = I2C_DRIVER_ERR_UNKOWN;
    break;
  }

  return halRetCode;
}

uint8_t halI2cReadBytes(uint8_t address, uint8_t *buffer, uint8_t count)
{
  I2C_TransferSeq_TypeDef    seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t halRetCode;
  
  seq.addr = address;
  seq.flags = I2C_FLAG_READ;
  seq.buf[0].data = (uint8_t*)buffer;
  seq.buf[0].len = (uint16_t)count;
  
  ret = I2CSPM_Transfer(i2cConfig.port, &seq);
    
  switch (ret) {
  case i2cTransferDone:
    halRetCode = I2C_DRIVER_ERR_NONE;
    break;
  case i2cTransferBusErr:
    halRetCode = I2C_DRIVER_ERR_ADDR_NAK;
    break;
  case i2cTransferArbLost:
    halRetCode = I2C_DRIVER_ERR_ARB_LOST;
    break;
  case i2cTransferUsageFault:
    halRetCode = I2C_DRIVER_ERR_USAGE_FAULT;
    break;
  case i2cTransferSwFault:
    halRetCode = I2C_DRIVER_ERR_SW_FAULT;
    break;
  default:
    halRetCode = I2C_DRIVER_ERR_UNKOWN;
    break;
  }

  return halRetCode;
}
