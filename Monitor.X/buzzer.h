/* 
 * File:   buzzer.h
 * Author: carlospardo
 *
 * Created on April 5, 2020, 3:07 PM
 */

#ifndef BUZZER_H
#define	BUZZER_H

#ifdef	__cplusplus
extern "C" {
#endif

void BuzzerTask(void);
void BuzzerInit(void);
void BuzzerTest(char);
void BuzzerOn(uint8_t period);
void BuzzerOff(void);
void BuzzerSet(uint8_t state);
void BuzzerClear(void);


#ifdef	__cplusplus
}
#endif

#endif	/* BUZZER_H */

