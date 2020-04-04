/* 
 * File:   time.h
 * Author: David
 *
 * Created on April 4, 2020, 4:27 PM
 */

#ifndef TIME_H
#define	TIME_H

#ifdef	__cplusplus
extern "C" {
#endif
    
typedef uint16_t time_t;

// ENSURE USE WITH TIME BELOW 1/2.
#define TIME_MS(time) ((time_t) time*1)
#define TIME_S(time) ((time_t) (time*1000))
    

void timeInit(void);
time_t timeGet(void);
bool timeElapsedR(time_t *prevTime, time_t duration);
bool timeElapsed(time_t prevTime, time_t duration);


#ifdef	__cplusplus
}
#endif

#endif	/* TIME_H */

