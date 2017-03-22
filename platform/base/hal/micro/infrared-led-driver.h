// Copyright 2014 Silicon Laboratories, Inc.
/**
 * @addtogroup infrared-led-driver
 *
 * Implements a infrared led driver for remote controls.
 * It accepts data on the following formats:
 * 1) Standard IR Database format, SIRD.
 * 2) Universal IR Database format, UIRD.
 * 3) Encrypted UIRD.
 *
 * The SIRD format is specified in the "ZRC Profile Specification Version 2.0".
 * The UIRD format specification can be obtained from Remotec, it is not public.
 *
 * The application interface, decoding of formats and emit functions are
 * implemented in separate files, making it easier to implement decoding
 * of new formats.
 *
 * By calling the halInfraredLedStart and halInfraredLedStop functions with
 * actual parameters, the driver will emit frames acording to the given format.
 *
 * The driver supports formats both with and without ir-carrier and formats
 * that emit a limited number of frames or continues to emit until the stop
 * function is called.
 *
 * This code uses an ::EmberEventControl called
 * ::halInfraredLedEventControl to perform periodic tasks in the handler
 * ::halInfraredLedEventHandler. The application framework will generally manage
 * the event automatically. Customers who do not use the framework must ensure
 * the event is run, by calling either ::emberRunEvents or ::emberRunTask.
 * @{
 */

#ifndef __INFRARED_LED_DRIVER_H__
#define __INFRARED_LED_DRIVER_H__

#include PLATFORM_HEADER

#define HAL_INFRARED_LED_STATUS_OK                  (0)
#define HAL_INFRARED_LED_STATUS_UNKNOWN_FORMAT      (1)
#define HAL_INFRARED_LED_STATUS_DECODE_ERROR        (2)
#define HAL_INFRARED_LED_STATUS_INVALID_LENGTH      (3)
#define HAL_INFRARED_LED_STATUS_DECRYPT_ERROR       (4)
#define HAL_INFRARED_LED_STATUS_INDEX_OUT_OF_RANGE  (5)
#define HAL_INFRARED_LED_STATUS_BUSY                (6)


/** @brief HalInfraredLedDbFormat defines the supported ir format,
 *  i.e. for this application the 16-bit Vendor ID defined in the ir database
 * is used. */
typedef uint16_t HalInfraredLedDbFormat;
enum
{
  HAL_INFRARED_LED_DB_FORMAT_SIRD           = 0x00, // Standard IR Database format
  HAL_INFRARED_LED_DB_FORMAT_UIRD           = 0x01, // Universal IR Descriptor (UIRD)
  HAL_INFRARED_LED_DB_FORMAT_UIRD_ENCRYPTED = 0x02, // Encrypted UIRD format
  HAL_INFRARED_LED_DB_FORMAT_UDEFINED       = 0xff  // Undefined
};

/**
 * @brief Initialize the GPIO and the internal timer for IR waveform generation.
 * The application framework will generally initialize this plugin
 * automatically. Customers who do not use the framework must ensure the plugin
 * is initialized by calling this function.
 */
void halInfraredLedInitialize(void);

/**
 * @brief Set the interval to control how often frames are emitted.
 * The value specifies the minimum periode between start of frames.
 *
 * @param intervalInMs  The 32-bit interval in milliseconds.
 *
 */
void halInfraredLedSetFrameIntervalInMs(uint32_t intervalInMs);

/**
 * @brief Start the transmissions of a sequence of IR frames. The format must
 * be specified in the IR database entry.
 *
 * @param irDbFormat    The 16-bit IR vendor ID of the vendor specific IRDB
 *
 * @param ird           A pointer to the IR database entry or NULL.
 *
 * @param irdLen        The length of the IR database entry, or if the ird
 *                      pointer is NULL the irdLen is the index to the local
 *                      database.
 *
 * @return 0 if no error, otherwise the error code
 *
 */
uint8_t halInfraredLedStart(HalInfraredLedDbFormat irDbFormat,
                            const uint8_t *ird,
                            uint8_t irdLen);

/**
 * @brief Stop the ongoing sequence of IR transmissions and transmit the final
 * IR frame. The format must be specified in the IR database entry.
 *
 * @param irDbFormat    The 16-bit IR vendor ID of the vendor specific IRDB
 *
 * @param ird           A pointer to the IR database entry or NULL.
 *
 * @param irdLen        The length of the IR database entry, or if the ird
 *                      pointer is NULL the irdLen is the index to the local
 *                      database.
 *
 * @return 0 if no error, otherwise the error code
 *
 */
uint8_t halInfraredLedStop(HalInfraredLedDbFormat irDbFormat,
                           const uint8_t *ird,
                           uint8_t irdLen);


#endif // __INFRARED_LED_DRIVER_H__
