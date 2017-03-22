/** @file hal/micro/random.h
 * See @ref random for detailed documentation.
 * 
 * <!-- Copyright 2005 by Ember Corporation. All rights reserved.-->   
 */

/** @addtogroup random
 * @brief Functions that provide access to random numbers.
 *
 * These functions may be hardware accelerated, though often are not.
 *
 * See random.h for source code.
 *@{
 */

#ifndef __RANDOM_H__
#define __RANDOM_H__

/** @brief Seeds the ::halCommonGetRandom() pseudorandom number
 * generator.
 *
 * Called by the stack during initialization with a seed from the radio.
 *
 * @param seed  A seed for the pseudorandom number generator.
 */
void halStackSeedRandom(uint32_t seed);

/** @brief Runs a standard LFSR to generate pseudorandom numbers.
 *
 * Called by the MAC in the stack to choose random backoff slots.
 *
 * Complicated implementations may improve the MAC's
 * ability to avoid collisions in large networks, but it is \b critical to
 * implement this function to return quickly.
 */
#if defined( EMBER_TEST )
#define halCommonGetRandom() halCommonGetRandomTraced(__FILE__, __LINE__)
uint16_t halCommonGetRandomTraced(char *file, int line);
#else
uint16_t halCommonGetRandom(void);
#endif

/**@}  // end of Random Functions
 */
 
#endif //__RANDOM_H__


