//  Copyright 2015 Silicon Laboratories, Inc.                               *80*

///*****************************************************************************
// ************************** Si114x I2C Registers *****************************
// *****************************************************************************
/// @cond DOXYGEN_SHOULD_SKIP_THIS
#define HAL_ILLUMINANCE_SI1141_REG_PART_ID               0x00
#define HAL_ILLUMINANCE_SI1141_REG_REV_ID                0x01
#define HAL_ILLUMINANCE_SI1141_REG_SEQ_ID                0x02
#define HAL_ILLUMINANCE_SI1141_REG_INT_CFG               0x03
#define HAL_ILLUMINANCE_SI1141_REG_IRQ_ENABLE            0x04
#define HAL_ILLUMINANCE_SI1141_REG_IRQ_MODE1             0x05
#define HAL_ILLUMINANCE_SI1141_REG_IRQ_MODE2             0x06
#define HAL_ILLUMINANCE_SI1141_REG_HW_KEY                0x07
#define HAL_ILLUMINANCE_SI1141_REG_MEAS_RATE             0x08
#define HAL_ILLUMINANCE_SI1141_REG_ALS_RATE              0x09
#define HAL_ILLUMINANCE_SI1141_REG_PS_RATE               0x0A
#define HAL_ILLUMINANCE_SI1141_REG_ALS_LO_TH_LSB         0x0B
#define HAL_ILLUMINANCE_SI1141_REG_ALS_LO_TH_MSB         0x0C
#define HAL_ILLUMINANCE_SI1141_REG_ALS_HI_TH_LSB         0x0D
#define HAL_ILLUMINANCE_SI1141_REG_ALS_HI_TH_MSB         0x0E
#define HAL_ILLUMINANCE_SI1141_REG_PS_LED21              0x0F
#define HAL_ILLUMINANCE_SI1141_REG_PS_LED3               0x10
#define HAL_ILLUMINANCE_SI1141_REG_PS1_TH_LSB            0x11
#define HAL_ILLUMINANCE_SI1141_REG_PS1_TH_MSB            0x12
#define HAL_ILLUMINANCE_SI1141_REG_PS2_TH_LSB            0x13
#define HAL_ILLUMINANCE_SI1141_REG_PS2_TH_MSB            0x14
#define HAL_ILLUMINANCE_SI1141_REG_PS3_TH_LSB            0x15
#define HAL_ILLUMINANCE_SI1141_REG_PS3_TH_MSB            0x16
#define HAL_ILLUMINANCE_SI1141_REG_PARAM_WR              0x17
#define HAL_ILLUMINANCE_SI1141_REG_COMMAND               0x18
#define HAL_ILLUMINANCE_SI1141_REG_RESPONSE              0x20
#define HAL_ILLUMINANCE_SI1141_REG_IRQ_STATUS            0x21
#define HAL_ILLUMINANCE_SI1141_REG_ALS_VIS_DATA0         0x22
#define HAL_ILLUMINANCE_SI1141_REG_ALS_VIS_DATA1         0x23
#define HAL_ILLUMINANCE_SI1141_REG_ALS_IR_DATA0          0x24
#define HAL_ILLUMINANCE_SI1141_REG_ALS_IR_DATA1          0x25
#define HAL_ILLUMINANCE_SI1141_REG_PS1_DATA0             0x26
#define HAL_ILLUMINANCE_SI1141_REG_PS1_DATA1             0x27
#define HAL_ILLUMINANCE_SI1141_REG_PS2_DATA0             0x28
#define HAL_ILLUMINANCE_SI1141_REG_PS2_DATA1             0x29
#define HAL_ILLUMINANCE_SI1141_REG_PS3_DATA0             0x2A
#define HAL_ILLUMINANCE_SI1141_REG_PS3_DATA1             0x2B
#define HAL_ILLUMINANCE_SI1141_REG_AUX_DATA0             0x2C
#define HAL_ILLUMINANCE_SI1141_REG_AUX_DATA1             0x2D
#define HAL_ILLUMINANCE_SI1141_REG_PARAM_OUT             0x2E
#define HAL_ILLUMINANCE_SI1141_REG_PARAM_RD              0x2E
#define HAL_ILLUMINANCE_SI1141_REG_CHIP_STAT             0x30
#define HAL_ILLUMINANCE_SI1141_REG_UCOEF0                0x13
#define HAL_ILLUMINANCE_SI1141_REG_UCOEF1                0x14
#define HAL_ILLUMINANCE_SI1141_REG_UCOEF2                0x15
#define HAL_ILLUMINANCE_SI1141_REG_UCOEF3                0x16
#define HAL_ILLUMINANCE_SI1141_REG_MEAS_RATE_LSB         0x08
#define HAL_ILLUMINANCE_SI1141_REG_MEAS_RATE_MSB         0x09
/// @endcond

/*******************************************************************************
 ************************** Si114x I2C Parameter Offsets ***********************
 ******************************************************************************/
/// @cond DOXYGEN_SHOULD_SKIP_THIS
#define HAL_ILLUMINANCE_SI1141_PARAM_I2C_ADDR            0x00
#define HAL_ILLUMINANCE_SI1141_PARAM_CH_LIST             0x01
#define HAL_ILLUMINANCE_SI1141_PARAM_PSLED12_SELECT      0x02
#define HAL_ILLUMINANCE_SI1141_PARAM_PSLED3_SELECT       0x03
#define HAL_ILLUMINANCE_SI1141_PARAM_FILTER_EN           0x04
#define HAL_ILLUMINANCE_SI1141_PARAM_PS_ENCODING         0x05
#define HAL_ILLUMINANCE_SI1141_PARAM_ALS_ENCODING        0x06
#define HAL_ILLUMINANCE_SI1141_PARAM_PS1_ADC_MUX         0x07
#define HAL_ILLUMINANCE_SI1141_PARAM_PS2_ADC_MUX         0x08
#define HAL_ILLUMINANCE_SI1141_PARAM_PS3_ADC_MUX         0x09
#define HAL_ILLUMINANCE_SI1141_PARAM_PS_ADC_COUNTER      0x0A
#define HAL_ILLUMINANCE_SI1141_PARAM_PS_ADC_CLKDIV       0x0B
#define HAL_ILLUMINANCE_SI1141_PARAM_PS_ADC_GAIN         0x0B
#define HAL_ILLUMINANCE_SI1141_PARAM_PS_ADC_MISC         0x0C
#define HAL_ILLUMINANCE_SI1141_PARAM_IR_ADC_MUX          0x0E
#define HAL_ILLUMINANCE_SI1141_PARAM_AUX_ADC_MUX         0x0F
#define HAL_ILLUMINANCE_SI1141_PARAM_ALSVIS_ADC_COUNTER  0x10
#define HAL_ILLUMINANCE_SI1141_PARAM_ALSVIS_ADC_CLKDIV   0x11
#define HAL_ILLUMINANCE_SI1141_PARAM_ALSVIS_ADC_GAIN     0x11
#define HAL_ILLUMINANCE_SI1141_PARAM_ALSVIS_ADC_MISC     0x12
#define HAL_ILLUMINANCE_SI1141_PARAM_ALS_HYST            0x16
#define HAL_ILLUMINANCE_SI1141_PARAM_PS_HYST             0x17
#define HAL_ILLUMINANCE_SI1141_PARAM_PS_HISTORY          0x18
#define HAL_ILLUMINANCE_SI1141_PARAM_ALS_HISTORY         0x19
#define HAL_ILLUMINANCE_SI1141_PARAM_ADC_OFFSET          0x1A

#define HAL_ILLUMINANCE_SI1141_PARAM_SLEEP_CTRL          0x1B
#define HAL_ILLUMINANCE_SI1141_PARAM_LED_RECOVERY        0x1C
#define HAL_ILLUMINANCE_SI1141_PARAM_ALSIR_ADC_COUNTER   0x1D
#define HAL_ILLUMINANCE_SI1141_PARAM_ALSIR_ADC_CLKDIV    0x1E
#define HAL_ILLUMINANCE_SI1141_PARAM_ALSIR_ADC_GAIN      0x1E
#define HAL_ILLUMINANCE_SI1141_PARAM_ALSIR_ADC_MISC      0x1F
/// @endcond

/*******************************************************************************
 *******    Si114x Register and Parameter Bit Definitions  *********************
 ******************************************************************************/
/// @cond DOXYGEN_SHOULD_SKIP_THIS

// REG_IRQ_CFG
#define HAL_ILLUMINANCE_SI1141_ICG_INTOE                 0x01
#define HAL_ILLUMINANCE_SI1141_ICG_INTMODE               0x02

// REG_IRQ_STATUS
#define HAL_ILLUMINANCE_SI1141_IE_NONE                   0x00

#define HAL_ILLUMINANCE_SI1141_IE_ALS_NONE               0x00
#define HAL_ILLUMINANCE_SI1141_IE_ALS_EVRYSAMPLE         0x01
#define HAL_ILLUMINANCE_SI1141_IE_ALS_EXIT_WIN           0x01
#define HAL_ILLUMINANCE_SI1141_IE_ALS_ENTER_WIN          0x02

#define HAL_ILLUMINANCE_SI1141_IE_PS1_NONE               0x00
#define HAL_ILLUMINANCE_SI1141_IE_PS1_EVRYSAMPLE         0x04
#define HAL_ILLUMINANCE_SI1141_IE_PS1_CROSS_TH           0x04
#define HAL_ILLUMINANCE_SI1141_IE_PS1_EXCEED_TH          0x04
#define HAL_ILLUMINANCE_SI1141_IE_PS1                    0x04

#define HAL_ILLUMINANCE_SI1141_IE_PS2_NONE               0x00
#define HAL_ILLUMINANCE_SI1141_IE_PS2_EVRYSAMPLE         0x08
#define HAL_ILLUMINANCE_SI1141_IE_PS2_CROSS_TH           0x08
#define HAL_ILLUMINANCE_SI1141_IE_PS2_EXCEEED_TH         0x08
#define HAL_ILLUMINANCE_SI1141_IE_PS2                    0x08

#define HAL_ILLUMINANCE_SI1141_IE_PS3_NONE               0x00
#define HAL_ILLUMINANCE_SI1141_IE_PS3_EVRYSAMPLE         0x10
#define HAL_ILLUMINANCE_SI1141_IE_PS3_CROSS_TH           0x10
#define HAL_ILLUMINANCE_SI1141_IE_PS3_EXCEED_TH          0x10
#define HAL_ILLUMINANCE_SI1141_IE_PS3                    0x10

#define HAL_ILLUMINANCE_SI1141_IE_CMD                    0x20
#define HAL_ILLUMINANCE_SI1141_IE_ALL                    0x3F

// REG_IRQ_MODE1
#define HAL_ILLUMINANCE_SI1141_IM1_NONE                  0x00
#define HAL_ILLUMINANCE_SI1141_IM1_ALS_NONE              0x00
#define HAL_ILLUMINANCE_SI1141_IM1_ALS_EVRYSAMPLE        0x00
#define HAL_ILLUMINANCE_SI1141_IM1_ALS_VIS_EXIT          0x01
#define HAL_ILLUMINANCE_SI1141_IM1_ALS_VIS_ENTER         0x05
#define HAL_ILLUMINANCE_SI1141_IM1_ALS_IR_EXIT           0x03
#define HAL_ILLUMINANCE_SI1141_IM1_ALS_IR_ENTER          0x06

#define HAL_ILLUMINANCE_SI1141_IM1_PS1_NONE              0x00
#define HAL_ILLUMINANCE_SI1141_IM1_PS1_EVRYSAMPLE        (0x0<<4)
#define HAL_ILLUMINANCE_SI1141_IM1_PS1_CROSS_TH          (0x1<<4)
#define HAL_ILLUMINANCE_SI1141_IM1_PS1_EXCEED_TH         (0x3<<4)

#define HAL_ILLUMINANCE_SI1141_IM1_PS2_NONE              0x00
#define HAL_ILLUMINANCE_SI1141_IM1_PS2_EVRYSAMPLE        (0x0<<6)
#define HAL_ILLUMINANCE_SI1141_IM1_PS2_CROSS_TH          (0x1<<6)
#define HAL_ILLUMINANCE_SI1141_IM1_PS2_EXCEED_TH         (0x3<<6)

// REG_IHAL_ILLUMINANCE_SI1141_RQ_MODE1
#define HAL_ILLUMINANCE_SI1141_IM2_PS3_NONE              0x00
#define HAL_ILLUMINANCE_SI1141_IM2_PS3_EVRYSAMPLE        (0x0)
#define HAL_ILLUMINANCE_SI1141_IM2_PS3_CROSS_TH          (0x1)
#define HAL_ILLUMINANCE_SI1141_IM2_PS3_EXCEED_TH         (0x3)

//
// REG_PS_LED21   LED2 Current is upper nibble
//                LED1 Current is lower nibble
//
// REG_PS_LED3    LED3 Current is lower nibble
#define HAL_ILLUMINANCE_SI1141_LEDI_000                  0x00
#define HAL_ILLUMINANCE_SI1141_LEDI_006                  0x01
#define HAL_ILLUMINANCE_SI1141_LEDI_011                  0x02
#define HAL_ILLUMINANCE_SI1141_LEDI_022                  0x03
#define HAL_ILLUMINANCE_SI1141_LEDI_045                  0x04
#define HAL_ILLUMINANCE_SI1141_LEDI_067                  0x05
#define HAL_ILLUMINANCE_SI1141_LEDI_090                  0x06
#define HAL_ILLUMINANCE_SI1141_LEDI_112                  0x07
#define HAL_ILLUMINANCE_SI1141_LEDI_135                  0x08
#define HAL_ILLUMINANCE_SI1141_LEDI_157                  0x09
#define HAL_ILLUMINANCE_SI1141_LEDI_180                  0x0A
#define HAL_ILLUMINANCE_SI1141_LEDI_202                  0x0B
#define HAL_ILLUMINANCE_SI1141_LEDI_224                  0x0C
#define HAL_ILLUMINANCE_SI1141_LEDI_269                  0x0D
#define HAL_ILLUMINANCE_SI1141_LEDI_314                  0x0E
#define HAL_ILLUMINANCE_SI1141_LEDI_359                  0x0F

// PARAM_CH_LIST
#define HAL_ILLUMINANCE_SI1141_PS1_TASK                  0x01
#define HAL_ILLUMINANCE_SI1141_PS2_TASK                  0x02
#define HAL_ILLUMINANCE_SI1141_PS3_TASK                  0x04
#define HAL_ILLUMINANCE_SI1141_ALS_VIS_TASK              0x10
#define HAL_ILLUMINANCE_SI1141_ALS_IR_TASK               0x20
#define HAL_ILLUMINANCE_SI1141_AUX_TASK                  0x40
//
// ADC Counters
// PARAM_PS_ADC_COUNTER
// PARAM_ALSVIS_ADC_COUNTER
// PARAM_ALSIR_ADC_COUNTER
//
#define HAL_ILLUMINANCE_SI1141_RECCNT_001                0x00
#define HAL_ILLUMINANCE_SI1141_RECCNT_007                0x10
#define HAL_ILLUMINANCE_SI1141_RECCNT_015                0x20
#define HAL_ILLUMINANCE_SI1141_RECCNT_031                0x30
#define HAL_ILLUMINANCE_SI1141_RECCNT_063                0x40
#define HAL_ILLUMINANCE_SI1141_RECCNT_127                0x50
#define HAL_ILLUMINANCE_SI1141_RECCNT_255                0x60
#define HAL_ILLUMINANCE_SI1141_RECCNT_511                0x70

//
// Proximity LED Selection
// PARAM_PSLED12_SELECT  PS2 LED Choice is Upper Nibble
//                       PS1 LED Choice is Lower Nibble
//
// PARAM_PSLED3_SELECT   PS3 LED Choice is Lower Nibble
//
// Each of the three PS measurements can choose whichever
// irLED to light up during the measurement, with whichever
// combination desired.
#define HAL_ILLUMINANCE_SI1141_NO_LED_EN                 0x00
#define HAL_ILLUMINANCE_SI1141_LED1_EN                   0x01
#define HAL_ILLUMINANCE_SI1141_LED2_EN                   0x02
#define HAL_ILLUMINANCE_SI1141_LED3_EN                   0x04
#define HAL_ILLUMINANCE_SI1141_SEL_LED1_PS1              (LED1_EN)
#define HAL_ILLUMINANCE_SI1141_SEL_LED2_PS1              (LED2_EN)
#define HAL_ILLUMINANCE_SI1141_SEL_LED3_PS1              (LED3_EN)
#define HAL_ILLUMINANCE_SI1141_SEL_LED1_PS2              (LED1_EN<<4)
#define HAL_ILLUMINANCE_SI1141_SEL_LED2_PS2              (LED2_EN<<4)
#define HAL_ILLUMINANCE_SI1141_SEL_LED3_PS2              (LED3_EN<<4)
#define HAL_ILLUMINANCE_SI1141_SEL_LED1_PS3              (LED1_EN)
#define HAL_ILLUMINANCE_SI1141_SEL_LED2_PS3              (LED2_EN)
#define HAL_ILLUMINANCE_SI1141_SEL_LED3_PS3              (LED3_EN)

// PARAM_PS_ENCODING
// When these bits are set the corresponding measurement
// will report the least significant bits of the
// ADC is used instead of the most significant bits
#define HAL_ILLUMINANCE_SI1141_PS1_LSB                   0x10
#define HAL_ILLUMINANCE_SI1141_PS2_LSB                   0x20
#define HAL_ILLUMINANCE_SI1141_PS3_LSB                   0x40
#define HAL_ILLUMINANCE_SI1141_PS_ENCODING_MASK          0x70

// PARAM_ALS_ENCODING
// When these bits are set the corresponding measurement
// will report the least significant bits of the
// ADC is used instead of the most significant bits
#define HAL_ILLUMINANCE_SI1141_ALS_VIS_LSB               0x10
#define HAL_ILLUMINANCE_SI1141_ALS_IR_LSB                0x20
#define HAL_ILLUMINANCE_SI1141_AUX_LSB                   0x40
#define HAL_ILLUMINANCE_SI1141_ALS_ENCODING_MASK         0xCF

#define HAL_ILLUMINANCE_SI1141_ALS_IR_ADC_MISC_MASK      0x20
#define HAL_ILLUMINANCE_SI1141_ALS_VIS_ADC_MISC_MASK     0x20

//the offset f parameter ram is only 5 bits, so we use this as a mask
//when we do the addressing.
#define HAL_ILLUMINANCE_SI1141_PARAM_RAM_OFFSET_MASK     0x1F

#define HAL_ILLUMINANCE_SI1141_MUX_SMALL_IR              0x00
#define HAL_ILLUMINANCE_SI1141_MUX_VIS                   0x02
#define HAL_ILLUMINANCE_SI1141_MUX_LARGE_IR              0x03
#define HAL_ILLUMINANCE_SI1141_MUX_NO_PHOTO_DIODE        0x06
#define HAL_ILLUMINANCE_SI1141_MUX_VTEMP                 0x65
#define HAL_ILLUMINANCE_SI1141_MUX_INT                   0x05
#define HAL_ILLUMINANCE_SI1141_MUX_LED1                  0x15
#define HAL_ILLUMINANCE_SI1141_MUX_VSS                   0x25
#define HAL_ILLUMINANCE_SI1141_MUX_LED2                  0x35
#define HAL_ILLUMINANCE_SI1141_MUX_VDD                   0x75

// ADC Dividers
#define HAL_ILLUMINANCE_SI1141_ADC_NORM                  0x00
#define HAL_ILLUMINANCE_SI1141_ADC_DIV2                  0x01
#define HAL_ILLUMINANCE_SI1141_ADC_DIV4                  0x02
#define HAL_ILLUMINANCE_SI1141_ADC_DIV8                  0x03
#define HAL_ILLUMINANCE_SI1141_ADC_DIV16                 0x04
#define HAL_ILLUMINANCE_SI1141_ADC_DIV32                 0x05
#define HAL_ILLUMINANCE_SI1141_ADC_DIV64                 0x06
#define HAL_ILLUMINANCE_SI1141_ADC_DIV128                0x07
#define HAL_ILLUMINANCE_SI1141_ADC_DIV256                0x08
#define HAL_ILLUMINANCE_SI1141_ADC_DIV512                0x09
#define HAL_ILLUMINANCE_SI1141_ADC_DIV1024               0x0A
#define HAL_ILLUMINANCE_SI1141_ADC_DIV2048               0x0B

// Hardware Key value
// REG_HW_KEY
#define HAL_ILLUMINANCE_SI1141_HW_KEY_VAL0               0x17

// Sleep Control
// PARAM_SLEEP_CTRL
#define HAL_ILLUMINANCE_SI1141_SLEEP_DISABLED            0x01

// ANA_IN_KEY value
#define HAL_ILLUMINANCE_SI1141_ANA_KEY_38                0x10
#define HAL_ILLUMINANCE_SI1141_ANA_KEY_39                0x40
#define HAL_ILLUMINANCE_SI1141_AMA_KEY_3A                0x62
#define HAL_ILLUMINANCE_SI1141_ANA_KEY_3B                0x3b
/// @endcond

//chip commands
#define HAL_ILLUMINANCE_SI1141_COMMAND_NOP               0x00
#define HAL_ILLUMINANCE_SI1141_COMMAND_RESET             0x01
#define HAL_ILLUMINANCE_SI1141_COMMAND_BUSADDR           0x02
#define HAL_ILLUMINANCE_SI1141_COMMAND_PS_FORCE          0x05
#define HAL_ILLUMINANCE_SI1141_COMMAND_ALS_FORCE         0x06
#define HAL_ILLUMINANCE_SI1141_COMMAND_PSALS_FORCE       0x07
#define HAL_ILLUMINANCE_SI1141_COMMAND_PS_PAUSE          0x09
#define HAL_ILLUMINANCE_SI1141_COMMAND_ALS_PAUSE         0x0A
#define HAL_ILLUMINANCE_SI1141_COMMAND_PSALS_PAUSE       0x0B
#define HAL_ILLUMINANCE_SI1141_COMMAND_PS_AUTO           0x0D
#define HAL_ILLUMINANCE_SI1141_COMMAND_ALS_AUTO          0x0E
#define HAL_ILLUMINANCE_SI1141_COMMAND_PSALS_AUTO        0x0F

#define HAL_ILLUMINANCE_SI1141_COMMAND_PARAM_QUERY       0x80
#define HAL_ILLUMINANCE_SI1141_COMMAND_PARAM_SET         0xA0
#define HAL_ILLUMINANCE_SI1141_COMMAND_PARAM_AND         0xC0
#define HAL_ILLUMINANCE_SI1141_COMMAND_PARAM_OR          0xE0

#define HAL_ILLUMINANCE_SI1141_IRCORRECTION_FACTOR_PS1 17

#define HAL_ILLUMINANCE_SI1141_RETRY_COUNT 2
#define HAL_ILLUMINANCE_SI1141_I2C_ADDR (0x5A<<1)

#define HAL_ILLUMINANCE_SI1141_LEVEL_1 1
#define HAL_ILLUMINANCE_SI1141_LEVEL_2 3
#define HAL_ILLUMINANCE_SI1141_LEVEL_3 7
#define HAL_ILLUMINANCE_SI1141_LEVEL1_LOWER_BOUND 2500  //around 30000Lux
#define HAL_ILLUMINANCE_SI1141_LEVEL2_UPPER_BOUND 23000 //around 30000Lux
#define HAL_ILLUMINANCE_SI1141_LEVEL2_LOWER_BOUND 700   //around 1000Lux
#define HAL_ILLUMINANCE_SI1141_LEVEL3_UPPER_BOUND 13000 //around 1000Lux

#define HAL_ILLUMINANCE_SI1141_DEFAULT_MULTIPLIER        68
#define HAL_ILLUMINANCE_SI1141_MAX_CONVERSION_TIME_MS    5

 //anything below this value in ADC is actully 0 lux
#define HAL_ILLUMINANCE_SI1141_ADC_OFFSET         255
#define HAL_ILLUMINANCE_SI1141_ERR_NONE           0
#define HAL_ILLUMINANCE_SI1141_ERR_DEVICE_BUSY    1
#define HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION  2
#define HAL_ILLUMINANCE_SI1141_CHIP_SLEEP         1
#define HAL_ILLUMINANCE_SI1141_CHIP_SUSPEND       2
#define HAL_ILLUMINANCE_SI1141_CHIP_RUNNING       3
#define HAL_ILLUMINANCE_SI1141_CHIP_IDLE \
          (HAL_ILLUMINANCE_SI1141_CHIP_SLEEP | \
           HAL_ILLUMINANCE_SI1141_CHIP_SUSPEND)
#define HAL_ILLUMINANCE_SI1141_CHIP_STAT_MASK     7
#define HAL_ILLUMINANCE_SI1141_INIT_DELAY_MS      20

#define HAL_ILLUMINANCE_SI1141_RESET_RESPOND      1

// this is used to set up how often the device wakes up 
// to make measurements, for example can be the following
// values:
//    0x21 = Device Wakes up every ~158 us
//    0xa0 = Device Wakes up every ~30 ms
//    0x94 = Device Wakes up every ~20 ms
//    0x84 = Device Wakes up every ~10 ms
//    0xB9 = Device Wakes up every ~100 ms
//    0xFF = Device Wakes up every ~2 sec
// we set to a very fast value as we want to finish the 
// number of samples as fast as possible 
#define HAL_ILLUMINANCE_SI1141_DEFAULT_MEASUREMENT_RATE 0x21
