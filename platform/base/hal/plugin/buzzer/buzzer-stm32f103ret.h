/** @file hal/plugin/buzzer/buzzer-stm32f103ret.h
 * See @ref stm32f103ret_buzzer for documentation.
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

/** @addtogroup stm32f103ret_buzzer
 * @brief Example API funtions for operating a piezo buzzer.
 *  
 * @note On the STM32F103RET example host, the buzzer is tied to
 * GPIO PC6 using TIM3 Channel 1.
 *
 * See buzzer.h for source code.
 *@{
 */

#ifndef __BUZZER_H__
#define __BUZZER_H__


/** @name Note Definitions
 * Flats are used instead of sharps because # is a special character.
 *@{
 */


/**
 * @brief A note which can be used in tune structure definitions.  These
 * definitions are simply the actual note frequencies.  The division
 * by 4 is necessary to get the frequencies to fit in a byte.
 */
#define NOTE_C3  (130/4)
#define NOTE_Db3 (138/4)
#define NOTE_D3  (146/4)
#define NOTE_Eb3 (155/4)
#define NOTE_E3  (164/4)
#define NOTE_F3  (174/4)
#define NOTE_Gb3 (185/4)
#define NOTE_G3  (196/4)
#define NOTE_Ab3 (207/4)
#define NOTE_A3  (220/4)
#define NOTE_Bb3 (233/4)
#define NOTE_B3  (246/4)
#define NOTE_C4  (261/4)
#define NOTE_Db4 (277/4)
#define NOTE_D4  (293/4)
#define NOTE_Eb4 (311/4)
#define NOTE_E4  (329/4)
#define NOTE_F4  (349/4)
#define NOTE_Gb4 (369/4)
#define NOTE_G4  (392/4)
#define NOTE_Ab4 (415/4)
#define NOTE_A4  (440/4)
#define NOTE_Bb4 (466/4)
#define NOTE_B4  (493/4)
#define NOTE_C5  (523/4)
#define NOTE_Db5 (554/4)
#define NOTE_D5  (587/4)
#define NOTE_Eb5 (622/4)
#define NOTE_E5  (659/4)
#define NOTE_F5  (698/4)
#define NOTE_Gb5 (739/4)
#define NOTE_G5  (783/4)
#define NOTE_Ab5 (830/4)
#define NOTE_A5  (880/4)
#define NOTE_Bb5 (932/4)
#define NOTE_B5  (987/4)
/** @}  Note Definitions */


/** @brief Plays a tune on the piezo buzzer. 
 *
 * The tune is played in the background if ::bkg is true. 
 * Otherwise, the API blocks until the playback of the tune is complete.
 * 
 * @param tune  A pointer to tune to play. 
 *  
 * @param bkg   Determines whether the tune plays in the background.
 * If true, tune plays in background; if false, tune plays in foreground.
 *
 * A tune is implemented as follows:
 * @code 
 * uint8_t PGM hereIamTune[] = {    //All tunes are stored in flash.
 *    NOTE_B4,  1,                //Plays the note B4 for 100 milliseconds.
 *    0,        1,                //Pause for 100 milliseconds.
 *    NOTE_B4,  1,                //Plays the note B4 for 100 milliseconds.
 *    0,        1,                //Pause for 100 milliseconds.
 *    NOTE_B4,  1,                //Plays the note B4 for 100 milliseconds.
 *    0,        1,                //Pause for 100 milliseconds.
 *    NOTE_B5,  5,                //Plays the note B5 for 500 milliseconds.
 *    0,        0                 //NULL terminates the tune.
 *  }; 
 * @endcode
 * 
 */
void halPlayTune_P(uint8_t PGM *tune, bool bkg);


/** @brief Plays a tone on the piezo buzzer.  The tone will play continuously
 * until halStopBuzzerTone() is called.
 *
 * @param frequency  The frequency of the tone to play.
 */
void halStartBuzzerTone(uint16_t frequency);


/** @brief Stops playing a tone that was started by halStartBuzzerTone().
 */
void halStopBuzzerTone(void);


/** @brief Extern definition of Ember's traditional little "here I am" 
 * announcement tune, which lives in the buzzer module.
 */
extern uint8_t PGM hereIamTune[];

#endif //__BUZZER_H__

/**@} //END addtogroup 
 */

