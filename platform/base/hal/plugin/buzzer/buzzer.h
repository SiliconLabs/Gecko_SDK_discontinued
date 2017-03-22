/** @file hal/plugin/buzzer/buzzer.h
 * See @ref buzzer for documentation.
 *
 * <!-- Author(s): Lee Taylor, lee@ember.com -->
 * <!-- Copyright 2005 by Ember Corporation. All rights reserved.       *80*-->   
 */
 
/** @addtogroup buzzer 
 *  @brief Sample API functions for playing tunes on a piezo buzzer
 *
 * See buzzer.h for source code.
 *@{
 */

/** @name Note Definitions
 * Flats are used instead of sharps because # is a special character.
 *@{
 */

/**
 * @brief A note which can be used in tune structure definitions.  
 */
#define	NOTE_C3	119
#define	NOTE_Db3	112
#define	NOTE_D3	106
#define	NOTE_Eb3	100
#define	NOTE_E3	94
#define	NOTE_F3	89
#define	NOTE_Gb3	84
#define	NOTE_G3	79
#define	NOTE_Ab3	74
#define	NOTE_A3	70
#define	NOTE_Bb3	66
#define	NOTE_B3	63
#define	NOTE_C4	59
#define	NOTE_Db4	55
#define	NOTE_D4	52
#define	NOTE_Eb4	49
#define	NOTE_E4	46
#define	NOTE_F4	44
#define	NOTE_Gb4	41
#define	NOTE_G4	39
#define	NOTE_Ab4	37
#define	NOTE_A4	35
#define	NOTE_Bb4	33
#define	NOTE_B4	31
#define	NOTE_C5	29
#define	NOTE_Db5	27
#define	NOTE_D5	26
#define	NOTE_Eb5	24
#define	NOTE_E5	23
#define	NOTE_F5	21
#define	NOTE_Gb5	20
#define	NOTE_G5	19
#define	NOTE_Ab5	18
#define	NOTE_A5	17
#define	NOTE_Bb5	16
#define	NOTE_B5	15
/** @}  Note Definitions */

/** @brief Plays a tune on the piezo buzzer. 
 *
 * The tune is played in the background if ::bkg is true. 
 * Otherwise, the API blocks until the playback of the tune is complete.
 * halPlayTune_P() is not meant to be called back-to-back.
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
 *    NOTE_B5,  5,                //Plays the note B5 for 500 milliseconds.
 *    0,        0                 //NULL terminates the tune.
 *  }; 
 * @endcode
 * 
 */
void halPlayTune_P(uint8_t PGM *tune, bool bkg);


/** @brief Causes something to happen on a node (such as playing a tune
 *    on the buzzer) that can be used to indicate where it physically is.
 *
 */
void halStackIndicatePresence(void);

/** @} // END addtogroup 
 */




