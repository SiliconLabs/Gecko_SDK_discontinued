// Copyright 2014 Silicon Laboratories, Inc.
//
// Some implementations of this function may make use of ::EmberEventControl
// events.  If so, the application framework will generally manage the events
// automatically.  Customers who do not use the framework must ensure the events
// are run, by calling either ::emberRunEvents or ::emberRunTask.  See the
// specific implementation for more information.
#ifndef __MICROPHONE_CODEC_MSADPCM_H__
#define __MICROPHONE_CODEC_MSADPCM_H__

/** @brief Start microphone
 *
 * This function starts sampling the microphone output.
 *
 * Note: halMicroponeCodecMsadpcmDataReadyCallback will be called with a pointer
 * to the microphone data once it is available.
 *
 * @param none
 */
void halMicrophoneCodecMsadpcmStart(void);

/** @brief Stop microphone
 *
 * This function stops sampling of the microphone output.
 *
 * @param none
 */
void halMicrophoneCodecMsadpcmStop(void);

/** @brief Initializes the microphone plugin. The application framework will
 * generally initialize this plugin automatically. Customers who do not use the
 * framework must ensure the plugin is initialized by calling this function.
 */
void halMicrophoneCodecMsadpcmInitialize(void);

/** @brief A callback called when new microphone data is ready
 *
 * @appusage This function is called by the plugin when new data has been
 * processed and is ready to be processed by other parts of the system.
 * The callback should be implemented by the application.
 *
 * @param data     Pointer to the data that is ready
 *
 * @param length   Length of the data
 */
void halMicrophoneCodecMsadpcmDataReadyCallback(uint8_t * data, uint8_t length);

#endif // __MICROPHONE_CODEC_MSADPCM_H__
