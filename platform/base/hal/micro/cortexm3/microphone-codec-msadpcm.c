// Copyright 2014 Silicon Laboratories, Inc.
//-----------------------------------------------------------------------------
// Implements an InvenSense INMP421 PDM digital microphone to EM3xx Interface
//
// Microphone is connected to a TI TLV320DAC3203 right digital mic input.  The
// codec is connected to EM3xx:
//   1. timer port creating the codec MCLK microphone
//   2. SC1 port for I2C control
//   3. SC2 port for I2S data input (operate SC2 is SPI slave mode)
//   4. PA6 for power on/off
//
// Codec is programmed to produce a 16-bit 16kSps I2S output, which is captured
// using the SPI/DMA slave mode.
//
// Two words of PCM data are compressed to a single byte for inclusion in
// audio output buffer
//  => 4:1 compression is achieved via low MIPS MSADCPM compression
//  => MSADCPCM compressor state variables use 7-bytes (12-bytes unpacked)
//  => MSADCPCM compressor state at start of each buffer fill is copied to
//     beginning of buffer
//  => At start of each audio output buffer, MSADPCM bPredictor is re-evaluated
//     over all 7 possibilities using first eight samples.  bPredictor with
//     lowest is selected for that buffer.
//
//  Audio buffer size is set to 103-bytes (102-bytes after SEQ header)
//  => After MSADCPCM compressor state, 95 bytes are used for compressed data.
//     However, two uncompressed samples are saved in MSADPCM state.
//  => 95 compression bytes/audio buffer + 2 uncompressed
//                                               => 192 PCM samples/audio buffer
//  => At 16kSps, an audio buffer must be emptied every 12 ms to avoid overflow
//
// Due to mathematics of 95 compressed bytes per buffer and 16kSps Codec:
//  1. SPI/DMA buffers (using header defaults) are 8 samples every 500us
//  6. 8 16-bit signed PCM at 16kSps => 4 compressed MSADPCM bytes
//  7. 4 compressed MS ADPCM bytes => 24 DMA cycles to fill 96 byte buffer
//  8. 24 cycles of 500us each => 12 ms between audio xfers
//
//-----------------------------------------------------------------------------
// Possible future enhancements
// 1. Add DC removal or calibration <= use codec IIR

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"
#include "hal/plugin/i2c-driver/i2c-driver.h"
#include "hal/micro/microphone-codec-msadpcm.h"
#include "hal/micro/msadpcm.h"

#define CODEC_DELAY_AFTER_POWERON_MS 2

// Select SPI SC2 used for codec I2S connection
#ifndef MICROPHONE_SPI_PORT
#define MICROPHONE_SPI_PORT 2
#endif
// Select timer 2, channel 3 for codec MCLK
#ifndef MICROPHONE_TIMER
#define MICROPHONE_TIMER 2
#endif

//#define TICS_PER_PERIOD 2   // based on 12MHz, div 2 for 6MHz codec MCLK
//#define TICS_PER_PERIOD 12  // based on 12MHz, div 12 for 1MHz codec MCLK
#define TICS_PER_PERIOD 6     // based on 6MHz, div 6 for 1MHz codec MCLK
#define ENABLE_1KHZ_DEBUG 0
#define ENABLE_OVERFLOW_DETECT 1
#define ENABLE_DEBUG_COMPRESSOR 0

#define AUDIO_BUFFER_FIFO_SIZE 4

// Space needed to hold sequence header
#define SEQ_HEADER_SIZE (1*sizeof(uint8_t))

// Define offset indexes into packed uint8_t audio buffer array
#define AUDIO_BUFFER_START_OFFSET 0
// Sequence header
#define AUDIO_BUFFER_SEQ_OFFSET (AUDIO_BUFFER_START_OFFSET)
// MSADPCM state (packed)
#define AUDIO_BUFFER_MSADCPM_STATE_OFFSET (AUDIO_BUFFER_START_OFFSET \
                                           + SEQ_HEADER_SIZE)
// Compressed audio payload
#define AUDIO_BUFFER_PAYLOAD_OFFSET (AUDIO_BUFFER_START_OFFSET \
                                    + SEQ_HEADER_SIZE \
                                    + MSADCPM_STATE_SIZE)

// For MSADPCM, 16-bits samples compress to 4-bit encoded output
#define MSADPCM_COMPRESSION_RATIO 4

#define AUDIO_BUFFER_SIZE (103)

#define AUDIO_PAYLOAD_SIZE (AUDIO_BUFFER_SIZE \
                            - SEQ_HEADER_SIZE \
                            - MSADCPM_STATE_SIZE)

#define AUDIO_PAYLOAD_SIZE_EFFECTIVE (AUDIO_PAYLOAD_SIZE \
                            + 2*MSADCPM_SAMPLES_IN_HEADER \
                            /MSADPCM_COMPRESSION_RATIO)

#define DMA_CYCLES_PER_AUDIO_BUFFER 24

#define POST_COMPRESSION_BYTES_PER_DMA_CYCLE \
                           (AUDIO_PAYLOAD_SIZE_EFFECTIVE \
                          / DMA_CYCLES_PER_AUDIO_BUFFER)
#define PRE_COMPRESSION_BYTES_PER_DMA_CYCLE \
                           (POST_COMPRESSION_BYTES_PER_DMA_CYCLE \
                          * MSADPCM_COMPRESSION_RATIO)

// Note: Two DMA buffers are required for DMA A/B buffer ping/pong
#define SPI_RX_BUFFER_SIZE PRE_COMPRESSION_BYTES_PER_DMA_CYCLE
#define PCM_SAMPLES_PER_DMA (SPI_RX_BUFFER_SIZE / sizeof(int16_t))

//-----------------------------------------------------------------------------
// Some handy-dandy preprocessor magic
#define PASTE(a,b,c)       a##b##c
#define EVAL3(a,b,c)       PASTE(a,b,c)

//-----------------------------------------------------------------------------
// Simplify the code by supporting SPI only on SCx
#define SCx_REG(port, reg)            (EVAL3(SC,port,_##reg))
#define SCx_NSEL(port)                (SCx_REG(port, NSEL))
#define SCx_OPFLAGS(port)             (scxOpFlags)
#define SCx_CB(port)                  (scxCB)
#define INT_SCx(port)                 (EVAL3(INT_SC,port,))
#define INT_SCxCFG(port)              (EVAL3(INT_SC,port,CFG))
#define INT_SCxFLAG(port)             (EVAL3(INT_SC,port,FLAG))

//-----------------------------------------------------------------------------
// Local Macros
//-----------------------------------------------------------------------------
/// Macros for timer, microphone power, and debug

#if (MICROPHONE_TIMER == 1)
#define TIMER_OR    TIM1_OR
#define TIMER_PSC   TIM1_PSC
#define TIMER_EGR   TIM1_EGR
#define TIMER_CCMR1 TIM1_CCMR1
#define TIMER_CCMR2 TIM1_CCMR2
#define TIMER_ARR   TIM1_ARR
#define TIMER_CNT   TIM1_CNT
#define TIMER_CMR2  TIM1_CMR2
#define TIMER_CCR3  TIM1_CCR3
#define TIMER_CCR4  TIM1_CCR4
#define TIMER_CCER  TIM1_CCER
#define TIMER_CR1   TIM1_CR1
#else
#if (MICROPHONE_TIMER == 2)
#define TIMER_OR    TIM2_OR
#define TIMER_PSC   TIM2_PSC
#define TIMER_EGR   TIM2_EGR
#define TIMER_CCMR1 TIM2_CCMR1
#define TIMER_CCMR2 TIM2_CCMR2
#define TIMER_ARR   TIM2_ARR
#define TIMER_CNT   TIM2_CNT
#define TIMER_CMR2  TIM2_CMR2
#define TIMER_CCR3  TIM2_CCR3
#define TIMER_CCR4  TIM2_CCR4
#define TIMER_CCER  TIM2_CCER
#define TIMER_CR1   TIM2_CR1
#else
#error "MICROPHONE_TIMER must be 1 or 2!"
#endif
#endif

#define MICROPHONE_ON do {AUDIO_ENABLE_CLRREG = AUDIO_ENABLE_PINMASK;} while(0)
#define MICROPHONE_OFF do {AUDIO_ENABLE_SETREG = AUDIO_ENABLE_PINMASK;} while(0)

#define CODEC_I2C_ADDRESS     0x30
#define CODEC_I2C_RESET_DELAY 2
#define CODEC_I2C_PLL_DELAY   15

#define GPIO_PxCFGL_BASE      (GPIO_PACFGL_ADDR)
#define GPIO_PxCFGH_BASE      (GPIO_PACFGH_ADDR)
#define GPIO_PxIN_BASE        (GPIO_PAIN_ADDR)
#define GPIO_PxOUT_BASE       (GPIO_PAOUT_ADDR)
#define GPIO_PxCLR_BASE       (GPIO_PACLR_ADDR)
#define GPIO_PxSET_BASE       (GPIO_PASET_ADDR)
#define GPIO_Px_OFFSET        (GPIO_PBCFGL_ADDR - GPIO_PACFGL_ADDR)

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef uint8_t AudioBuffer[AUDIO_BUFFER_SIZE];

//-----------------------------------------------------------------------------

// Prototypes
//-----------------------------------------------------------------------------
static void microphoneEventHandler(const uint8_t *pcmBuffer);
static AudioBuffer *microphoneCodecMsadpcmGetBufferPtr(void);
static void microphoneCodecMsadpcmReleaseBuffer(void);
//-----------------------------------------------------------------------------
// Local Variables
//-----------------------------------------------------------------------------
// Raw PDM SPI/DMA RX data buffers
static uint8_t RxBufferA[SPI_RX_BUFFER_SIZE];
static uint8_t RxBufferB[SPI_RX_BUFFER_SIZE];

static HalMsadpcmState compressorState;
static uint8_t compressedNibble;
static uint8_t compressedNibbleCount;

static uint8_t audioBufferIndex;

static AudioBuffer audioBufferFifo[AUDIO_BUFFER_FIFO_SIZE];
static uint8_t audioBufferFifoCount;
static uint8_t audioBufferFifoHead;
static uint8_t audioBufferFifoTail;

static uint8_t audioTransitOVRCountMod128; // MSB=1 => FIFO overrun, 7 LSB=count

static uint16_t codecPinCfg;

#if (ENABLE_1KHZ_DEBUG == 1)
static uint8_t sine_index = 0;
static const int16_t sine1khzM26dbfs[16] = {
  0,
  627,
  1159,
  1514,
  1638,
  1514,
  1159,
  627,
  0,
  -627,
  -1159,
  -1514,
  -1638,
  -1514,
  -1159,
  -627
};
#define SINE_1KHZ_M26DBFS_LENGTH (sizeof(sine1khzM26dbfs)/sizeof(int16_t))
#endif

#if (ENABLE_DEBUG_COMPRESSOR == 1)
#define COMPRESSOR_PREDICTOR_HITS (7)
uint32_t compressorPredictorHits[COMPRESSOR_PREDICTOR_HITS][COMPRESSOR_PREDICTOR_HITS];
#endif

//-----------------------------------------------------------------------------
// Local Constants
//-----------------------------------------------------------------------------
static const uint8_t codecSelectPage0[]           = {0x00,0x00};
static const uint8_t codecSwReset[]               = {0x01,0x01};
//static const uint8_t codecPllConfig[]           = {0x04,0x03,0x91,0x05,
//                                                 0x04,0xB0}; // 6MHz MCLK
static const uint8_t codecPllConfig[]             = {0x04,0x03,0x91,0x1E,
                                                   0x1C,0x20}; // 1MHz MCLK
static const uint8_t codecNdacMdac[]              = {0x0B,0x85,0x83};
static const uint8_t codecDacOsr[]                = {0x0D,0x80};
static const uint8_t codecNadcMadc[]              = {0x12,0x85,0x83};
static const uint8_t codecAdcOsr[]                = {0x14,0x80};
static const uint8_t codecAudioInterface[]        = {0x1B,0xCC,0x00,0x01,
                                                   0x84,0x00,0x00,0x00};
static const uint8_t codecProcessingBlocks[]      = {0x3C,0x04,0x04};
static const uint8_t codecMisoDigMicClock[]       = {0x37,0x0E};
static const uint8_t codecSclkRadcDigMicData[]    = {0x51,0x54};
//static const uint8_t codecUnmuteRadc[]          = {0x52,0x80};           // 0dB
//static const uint8_t codecUnmuteRadc[]          = {0x52,0x80,0x00,0x0C}; // 6dB
//static const uint8_t codecUnmuteRadc[]          = {0x52,0x80,0x00,0x18}; //12dB
static const uint8_t codecUnmuteRadc[]            = {0x52,0x80,0x00,0x24}; //18dB
static const uint8_t codecSelectPage1[]           = {0x00,0x01};
static const uint8_t codecDisableAvddToDvdd[]     = {0x01,0x08};
static const uint8_t codecEnableMasterAnalogPwr[] = {0x02,0x01};
static const uint8_t codecInputPwrUpTime[]        = {0x47,0x32};
static const uint8_t codecRefChargeTime[]         = {0x7B,0x01};

// enable 1st order IIR HPF with fs=2.048MHz and f-3dB = 160Hz
static const uint8_t codecSelectPage9[]           = {0x00,0x09};
static const uint8_t codecIirN0[]                 = {0x20,0x7F,0xF7,0xF6};
static const uint8_t codecIirN1[]                 = {0x24,0x80,0x08,0x0A};
static const uint8_t codecIirD1[]                 = {0x28,0x7F,0xEF,0xEB};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
EmberEventControl halMicrophoneCodecMsadpcmDataReadyEventControl;

//-----------------------------------------------------------------------------
// Local Helper Functions
//-----------------------------------------------------------------------------
static void codecInitialize(void)
{
  // Reset
  // Select Page 0
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecSelectPage0,sizeof(codecSelectPage0));
  // Initialize the device through software reset
  halI2cWriteBytesDelay(CODEC_I2C_ADDRESS,codecSwReset,
                        sizeof(codecSwReset),
                        CODEC_I2C_RESET_DELAY);
  // Clock and Interface Settings
  // The codec receives: MCLK = 6MHz,
  // BCLK = 512kHz, WCLK = 16kHz

  // Select Page 0
  // PLL_clkin = MCLK, codec_clkin = PLL_CLK,
  // PLL on, P=1, R=1, J=5, D=0x04B0 (1200 dec)
  halI2cWriteBytesDelay(CODEC_I2C_ADDRESS,codecPllConfig,
                        sizeof(codecPllConfig),
                        CODEC_I2C_PLL_DELAY);
  // NDAC = 5, MDAC = 3, dividers powered on (needed for WCLK generation)
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecNdacMdac,sizeof(codecNdacMdac));
  // DOSR = 128
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecDacOsr,sizeof(codecDacOsr));
  // NADC = 5, MADC = 3, dividers powered on
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecNadcMadc,sizeof(codecNadcMadc));
  // AOSR = 128
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecAdcOsr,sizeof(codecAdcOsr));
  // LJF, 16-bits, BCLK output, WCLK output, DOUT not high-Z
  halI2cWriteBytes(CODEC_I2C_ADDRESS,
                   codecAudioInterface,
                   sizeof(codecAudioInterface));

  // Configure Power Supplies
  // Select Page 1
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecSelectPage1,sizeof(codecSelectPage1));
  // Disable weak AVDD to DVDD connection
  halI2cWriteBytes(CODEC_I2C_ADDRESS,
                   codecDisableAvddToDvdd,
                   sizeof(codecDisableAvddToDvdd));
  // Enable Master Analog Power Control
  halI2cWriteBytes(CODEC_I2C_ADDRESS,
                   codecEnableMasterAnalogPwr,
                   sizeof(codecEnableMasterAnalogPwr));
  // Set the input power-up time to 3.1ms
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecInputPwrUpTime,
                   sizeof(codecInputPwrUpTime));
  // Set the REF charging time to 40ms
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecRefChargeTime,
                   sizeof(codecRefChargeTime));

  // Configure Processing Blocks
  // Select Page 0
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecSelectPage0,sizeof(codecSelectPage0));
  // PRB_P4 and PRB_R4 (mono-right) selected
  halI2cWriteBytes(CODEC_I2C_ADDRESS,
                   codecProcessingBlocks,
                   sizeof(codecProcessingBlocks));

  // Configure 1st order IIR (DC rejection, f-3dB=160Hz for fs=2.048MHz)
  // Select Page 9
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecSelectPage9,sizeof(codecSelectPage9));
  // set IIR coeffs: N0, N1, D1
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecIirN0,sizeof(codecIirN0));
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecIirN1,sizeof(codecIirN1));
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecIirD1,sizeof(codecIirD1));

  // Configure ADC Channel
  // Select Page 0
  halI2cWriteBytes(CODEC_I2C_ADDRESS,codecSelectPage0,sizeof(codecSelectPage0));
  // Configure MISO as clock output for DIGMIC
  halI2cWriteBytes(CODEC_I2C_ADDRESS,
                   codecMisoDigMicClock,
                   sizeof(codecMisoDigMicClock));
  // Power up RADC, Route SCLK as DIGMIC_DATA, RADC enabled for DIGMIC
  halI2cWriteBytes(CODEC_I2C_ADDRESS,
                   codecSclkRadcDigMicData,
                   sizeof(codecSclkRadcDigMicData));
  // Unmute RADC
  halI2cWriteBytes(CODEC_I2C_ADDRESS,
                   codecUnmuteRadc,
                   sizeof(codecUnmuteRadc));
}

//-----------------------------------------------------------------------------
// PCM->LPF->MSADPCM->Audio conversion signaled by ISR
static void microphoneEventHandler(const uint8_t *pcmBuffer)
{
  uint8_t i;                    // Watch i for SPI_RX_BUFFER_SIZE > 255 bytes
  int16_t pcmBufferW[PCM_SAMPLES_PER_DMA];

#if (ENABLE_OVERFLOW_DETECT == 1)
  if (audioBufferFifoCount >= AUDIO_BUFFER_FIFO_SIZE) { // Is FIFO full?
    audioTransitOVRCountMod128 |= 0x80; // Set FIFO overrun bit
    return;                    // Ff so, return, don't waste MCU time
  }
#endif

#if (ENABLE_1KHZ_DEBUG == 1)
  // Save 1kHz sine in buffer in little-endian format
  for (i=0; i < PCM_SAMPLES_PER_DMA; i++) {
    pcmBufferW[i] = sine1khzM26dbfs[sine_index];
    sine_index++;
    if (sine_index >= SINE_1KHZ_M26DBFS_LENGTH)
      sine_index = 0;
  }
#else
  // Convert pcmBuffer from big-endian uint8_t to pcmBufferW little-endian int16_t
  for (i=0; i < PCM_SAMPLES_PER_DMA; i++)
    pcmBufferW[i] = (int16_t)((((uint16_t)pcmBuffer[i*sizeof(int16_t)]) << 8)
                              + ((uint16_t)pcmBuffer[i*sizeof(int16_t)+1]));
#endif

  for (i=0; i < PCM_SAMPLES_PER_DMA; i++) {
    // Time to initialize Audio buffer header(SEQ header & MSADCPM state)?
    // Empty buffer and first nibble compression?
    if (!audioBufferIndex && !compressedNibbleCount) {
      // Initialize sequence header
      audioBufferFifo[audioBufferFifoHead][AUDIO_BUFFER_SEQ_OFFSET]
                     = audioTransitOVRCountMod128;
#if (ENABLE_DEBUG_COMPRESSOR == 1)
      int predictor_in;
      predictor_in = compressorState.bPredictor;
#endif

      // Reset the MSADPCM state with first samples (always start of buffer)
      halInternalMsadpcmResetState(&compressorState,0xff,0,pcmBufferW,
                                                          PCM_SAMPLES_PER_DMA);
      // Copy MSADPCM state (packed)
      halInternalMsadpcmSaveState(&audioBufferFifo[audioBufferFifoHead]
                                       [AUDIO_BUFFER_MSADCPM_STATE_OFFSET],
                       &compressorState);

#if (ENABLE_DEBUG_COMPRESSOR == 1)
      compressorPredictorHits[predictor_in][compressorState.bPredictor]++;
#endif

      audioTransitOVRCountMod128++;      // Inc buffer count mod 128
#if (ENABLE_OVERFLOW_DETECT == 1)
      audioTransitOVRCountMod128 &= 0x7F; // And clear over-run bit
#endif

      // Set index to start of payload
      audioBufferIndex = AUDIO_BUFFER_PAYLOAD_OFFSET;
      i+=2;   // MSADPCM header used 2 samples, start compression at 3rd sample
    }

    // Use MSADPCM compression from 16-bit to 4-bit nibble
    compressedNibble <<= 4;              // Prepare for next nibble
    compressedNibble |= halInternalMsadpcmEncode(pcmBufferW[i], &compressorState);

    compressedNibbleCount++;             // Update nibble counter
    if (compressedNibbleCount < 2) {     // Both nibbles in byte?
      continue;                          // No, get next nibble
    }
    compressedNibbleCount = 0;           // Reset nibble counter

    // Save MSADPCM byte (two compressed samples) to audio buffer
    audioBufferFifo[audioBufferFifoHead][audioBufferIndex]
                                                            = compressedNibble;
    audioBufferIndex++;
    if (audioBufferIndex < AUDIO_BUFFER_SIZE) {// Full?
      continue;                         // No, get next MSADPCM byte
    }
    audioBufferIndex = 0;              // Reset audio buffer counter

    // inc FIFO counter and advance head
    audioBufferFifoCount++;            // Inc count
    audioBufferFifoHead++;             // Advance head and test for wrap
    if (audioBufferFifoHead >= AUDIO_BUFFER_FIFO_SIZE) {
      audioBufferFifoHead = 0;
    }

    // Signal buffer transmit handler
    emberEventControlSetActive(
              halMicrophoneCodecMsadpcmDataReadyEventControl);
  }
}

//-----------------------------------------------------------------------------
// SPI ISR handler
void EVAL3(halSc,MICROPHONE_SPI_PORT,Isr)(void)  // Macro resolves to halScxIsr
{
  // Get SCx interrupt sources
  uint32_t INT_SCxFLAG = INT_SCxFLAG(MICROPHONE_SPI_PORT);
  if ((INT_SCRXULDA | INT_SCRXULDB) & INT_SCxFLAG) { // Either buffer unloaded?
    if (INT_SCRXULDA & INT_SCxFLAG) { // RX buffer A unloaded?
      microphoneEventHandler(RxBufferA); // Process buffer A
	  INT_SCxFLAG(MICROPHONE_SPI_PORT) = INT_SCRXULDA; // Ack RX buffer A int
      SCx_REG(MICROPHONE_SPI_PORT, DMACTRL) = SC_RXLODA; // Reload RX buffer A
	}
	if (INT_SCRXULDB & INT_SCxFLAG) { // RX buffer B unloaded?
      microphoneEventHandler(RxBufferB); // Process buffer B
	  INT_SCxFLAG(MICROPHONE_SPI_PORT) = INT_SCRXULDB; // Ack RX buffer B int
      SCx_REG(MICROPHONE_SPI_PORT, DMACTRL) = SC_RXLODB; // Reload RX buffer B
	}
  } else	{           	         // Clear unexpected interrupts
    INT_SCxFLAG(MICROPHONE_SPI_PORT) = 0xFFFF & ~(INT_SCRXULDA | INT_SCRXULDB);
  }
}

//-----------------------------------------------------------------------------
// Global API Functions
//-----------------------------------------------------------------------------
void halMicrophoneCodecMsadpcmStart(void)
{
  // Store current config for pins connected to codec
  codecPinCfg = CODEC_GPIO_CFG;

  // Reset SCx port and both TX/RX DMA channels, then disable/clear interrupts
  halMicrophoneCodecMsadpcmStop();

  // Configure codec pins
  CODEC_GPIO_CFG = CODEC_GPIO_CFGVAL;

  MICROPHONE_ON;        // Power-up microphone

  TIMER_OR = 0;         // Use 12MHz clock, and PA1 from TIMER2 TIM2C3 output
//  TIMER_PSC = 0;      // Prescaler 2^0=1 -> 12MHz internal clock
  TIMER_PSC = 1;        // Prescaler 2^1=2 -> 6MHz internal clock
  TIMER_EGR = 1;        // Trigger update event to load new prescaler value
  TIMER_CCMR1  = 0;     // Start from a zeroed configuration
  TIMER_ARR = TICS_PER_PERIOD-1;  // Set the period
  TIMER_CNT = 0;        // Force the counter back to zero

  //*** Set up PA1, which is TIMER2 channel 3 for 6MHz clock PWM output
  // Output waveform: toggle on CNT reaching TOP
  TIMER_CCMR2 = 0x007E;  // Also enable buffer and fast mode
  // Set the time when we go from high to low
  TIMER_CCR3 = TICS_PER_PERIOD/2;

  ATOMIC(
  TIMER_CCER |= TIM_CC3E;    // Enable output on channel 3
  TIMER_CR1  |= TIM_CEN;     // Enable counting
  )

  // Wait for codec to get ready after applying power to it
  halCommonDelayMicroseconds(CODEC_DELAY_AFTER_POWERON_MS*1000);

  // Initialize codec
  codecInitialize();

  // Setup buffer A and B beginning and ending addresses
  SCx_REG(MICROPHONE_SPI_PORT, RXBEGA) = (uint32_t)RxBufferA;
  SCx_REG(MICROPHONE_SPI_PORT, RXENDA) = (uint32_t)(RxBufferA
                                       + SPI_RX_BUFFER_SIZE - 1);
  SCx_REG(MICROPHONE_SPI_PORT, RXBEGB) = (uint32_t)RxBufferB;
  SCx_REG(MICROPHONE_SPI_PORT, RXENDB) = (uint32_t)(RxBufferB
                                       + SPI_RX_BUFFER_SIZE - 1);

  // Configure and initialize SPI port
  SCx_REG(MICROPHONE_SPI_PORT, SPICFG)  = 0; // Reset all SPI CFG, config SCLK
  SCx_REG(MICROPHONE_SPI_PORT, SPICFG)  = (
          (0 << SC_SPIRXDRV_BIT)    // Don't care in slave mode
        | (0 << SC_SPIMST_BIT)	    // SPI slave
        | (1 << SC_SPIRPT_BIT)      // TX send 0xFF BUSY (don't care, MCLK)
        | (0 << SC_SPIORD_BIT)	    // MSB per LJF timing
        | (0 << SC_SPIPHA_BIT)      // PHA=0 and POL=0
        | (0 << SC_SPIPOL_BIT)      // Capture I2S right channel LJF via SPI
                                          );
  SCx_REG(MICROPHONE_SPI_PORT, MODE)    = 2; // Enable the SCx port in SPI mode

  // Interrupt on either buffer unloading
  INT_SCxCFG(MICROPHONE_SPI_PORT) = INT_SCRXULDA | INT_SCRXULDB;

  // Enable top-level interrupt
  INT_CFGSET = INT_SCx(MICROPHONE_SPI_PORT);

  // Start DMA with both A and B buffers
  SCx_REG(MICROPHONE_SPI_PORT, DMACTRL) = SC_RXLODA | SC_RXLODB;
}

//-----------------------------------------------------------------------------
void halMicrophoneCodecMsadpcmStop(void)
{
  emberEventControlSetInactive(
                halMicrophoneCodecMsadpcmDataReadyEventControl);

  // Reset both TX/RX DMA channels and SCx port, then disable/clear interrupts
  // Reset the DMA
  SCx_REG(MICROPHONE_SPI_PORT, DMACTRL) = SC_TXDMARST | SC_RXDMARST;

#if (ENABLE_DEBUG_COMPRESSOR == 1)
  int i, j;

  if (SCx_REG(MICROPHONE_SPI_PORT, MODE) != 0) { // Currently running?
    SCx_REG(MICROPHONE_SPI_PORT, MODE) = 0; // Disable the port

    emberSerialPrintf(APP_SERIAL,"   0 1 2 3 4 5 6\r\n");
    for (i=0;i<COMPRESSOR_PREDICTOR_HITS;i++) {
      emberSerialPrintf(APP_SERIAL,"%d:",i);
      for (j=0;j<COMPRESSOR_PREDICTOR_HITS;j++) {
        emberSerialPrintf(APP_SERIAL," %d",compressorPredictorHits[i][j]);
      }
      emberSerialPrintf(APP_SERIAL,"\r\n");
    }
  }

  for (j=0;j<COMPRESSOR_PREDICTOR_HITS;j++) {
    compressorPredictorHits[i][j] = 0;
  }
#endif

  SCx_REG(MICROPHONE_SPI_PORT, MODE) = 0; // Disable the port

  ATOMIC (
  TIMER_CR1  &= ~TIM_CEN;         // Disable counting
  TIMER_CNT   = 0;                // Force the counter to 0 to force output low
  TIMER_CCER |= TIM_CC3E;         // Disable output on channel 3
  )

  MICROPHONE_OFF;                 // Power down microphone


  // Disable top-level interrupt
  INT_CFGCLR                       = INT_SCx(MICROPHONE_SPI_PORT);
  // Disable 2nd-level interrupts
  INT_SCxCFG(MICROPHONE_SPI_PORT)  = 0;
  // Clear stale 2nd-level interrupts
  INT_SCxFLAG(MICROPHONE_SPI_PORT) = 0xFFFF;
  // Clear stale top-level interrupt
  INT_PENDCLR                      = INT_SCx(MICROPHONE_SPI_PORT);

  halInternalMsadpcmResetState(&compressorState,0,1,NULL,0); // Reset MSADPCM compression
  compressedNibbleCount = 0;    // Reset nibble counter

  audioBufferIndex = 0;        // Reset audio FIFO
  audioBufferFifoCount = audioBufferFifoHead = audioBufferFifoTail = 0;

  audioTransitOVRCountMod128 = 0; // Reset overrun and buffer mod 128 count

  // Restore pins connected to codec to their original configuration
  CODEC_GPIO_CFG = codecPinCfg;
}

//-----------------------------------------------------------------------------
// Initialization function for the plugin
void halMicrophoneCodecMsadpcmInitialize(void)
{
  // Make sure I2C port initialized
  halI2cInitialize();
}

//-----------------------------------------------------------------------------
static AudioBuffer *microphoneCodecMsadpcmGetBufferPtr(void)
{
  // Note: Only call after microphone is started. Enables SCx interrupts

  // Check if audio FIFO contains a full buffer, ready for transmit
  // If no full buffer, return null pointer
  // If so, return pointer to oldest buffer, but do not release from FIFO

  // Note: This allows user to process buffer without MEMCOPY time waste, but
  // requires calling microphoneCodecMsadpcmReleaseBuffer
  // after done with buffer

  // Disable SCx/SPI top-level interrupt for FIFO count test
  INT_CFGCLR = INT_SCx(MICROPHONE_SPI_PORT);

  if (!audioBufferFifoCount) {    // Empty?
    // Re-enable SCx/SPI top-level interrupt
    INT_CFGSET = INT_SCx(MICROPHONE_SPI_PORT);
    return NULL;
  }

  // Re-enable SCx/SPI top-level interrupt
  INT_CFGSET = INT_SCx(MICROPHONE_SPI_PORT);
  return &audioBufferFifo[audioBufferFifoTail];
}

//-----------------------------------------------------------------------------
static void microphoneCodecMsadpcmReleaseBuffer(void)
{
  // Note: Only call after microphone is started. Enables SCx interrupts

  // Check if audio FIFO contains a full buffer (already transmitted)
  // If no full buffer, just return
  // If a full, advance tail and dec count (protect from SCx/SPI interrrupts!)

  // Disable SCx/SPI top-level interrupt for FIFO count test/update
  INT_CFGCLR = INT_SCx(MICROPHONE_SPI_PORT);

  if (!audioBufferFifoCount) { // Empty? => main loop has gotten out of synch!
    // Re-enable SCx/SPI top-level interrupt
    INT_CFGSET = INT_SCx(MICROPHONE_SPI_PORT);
    // <<======================
    // Flag FIFO under-run?
    // <<======================
    return; // Just return, may eventually correct
  }

  audioBufferFifoCount--;
  // Re-enable SCx/SPI top-level interrupt
  INT_CFGSET = INT_SCx(MICROPHONE_SPI_PORT);

  // Advance tail and test for wrap
  audioBufferFifoTail++;
  if (audioBufferFifoTail >= AUDIO_BUFFER_FIFO_SIZE) {
    audioBufferFifoTail = 0;
  }
}

//-----------------------------------------------------------------------------
// This function is call when an audio buffer has been filled and is ready
// to be read. The halMicrophoneCodecMsadpcmDataReadyCallback() allows the
// application to fetch the audio data from the buffer
void halMicrophoneCodecMsadpcmDataReadyEventHandler(void)
{
  AudioBuffer *buffer;

  // Make sure buffer transmit is inactive
  emberEventControlSetInactive(
                halMicrophoneCodecMsadpcmDataReadyEventControl);

  buffer = microphoneCodecMsadpcmGetBufferPtr();

  while (buffer != NULL) {
    halMicrophoneCodecMsadpcmDataReadyCallback((uint8_t*) buffer,
                                               AUDIO_BUFFER_SIZE);

    // Release audio buffer from FIFO
    microphoneCodecMsadpcmReleaseBuffer();

    // Check for another full audio buffer
    buffer = microphoneCodecMsadpcmGetBufferPtr();
  }
}
