/* 
 * File:   tick.h
 * Author: carlospardo
 *
 * Created on April 6, 2020, 8:47 PM
 */

#ifndef TICK_H
#define	TICK_H

#ifdef	__cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------*/
/*		Includes and dependencies			*/
/*-------------------------------------------------------------*/
#define TICKS_PER_SECOND	((CONFIG_TIMING_MAIN_CLOCK + 128ull)/256ull)	// Timer with 1:256 prescaler

/*-------------------------------------------------------------*/
/*		Macros and definitions				*/
/*-------------------------------------------------------------*/
#define TICK_SECOND	((unsigned long long)TICKS_PER_SECOND)
#define TICK_MINUTE	((unsigned long long)TICKS_PER_SECOND*60ull)
#define TICK_HOUR	((unsigned long long)TICKS_PER_SECOND*3600ull)

/*-------------------------------------------------------------*/
/*		Typedefs enums & structs			*/
/*-------------------------------------------------------------*/

/*-------------------------------------------------------------*/
/*		Function prototypes				*/
/*-------------------------------------------------------------*/
/**
 * @brief Initializes the system tick service.
 *
 * This function prepares the time keeping facilities for non-blocking delays,
 * event scheduling, time calculation and even real time clock functions. The
 * underlying time keeping variable is 48 bit long so there is enough space to
 * count from miliseconds to days and weeks.
 */
void tick_init();

/**
 * @brief Gets the current system tick value.
 *
 * Reads the lower 32 bits of the system tick counter. You can compare this
 * value with constants to generate delays or use it to measure time intervals.
 * The resolution and maximun time span to measure depend on operating frequency
 * and timer architecture.
 *
 * @return The current system tick value (a 32 bit integer value)
 */
uint32_t tick_get();
uint32_t tick_get_slow();

/**
 * @brief Updates the tick counter variable.
 *
 * For platforms which tick updates need to be done outside the Tick library
 * assembly, such as the PIC16 architecture wich only has a single interrupt
 * vector.
 */
void tick_update();


#ifdef	__cplusplus
}
#endif

#endif	/* TICK_H */

