/* 
 * File:   alarm.h
 * Author: carlospardo
 *
 * Created on April 4, 2020, 5:08 PM
 */

#ifndef ALARM_H
#define	ALARM_H

#ifdef	__cplusplus
extern "C" {
#endif



// --------------------- BUZZER ALARM STATE
#define ALARM_NONE 0
#define ALARM_LOW  1 // E 200ms LOW 200ms C 200ms LOW 16s
#define ALARM_MED  2 // C 200ms LOW 200ms A 200ms LOW 200ms F 200ms LOW 8s
#define ALARM_HIGH 4 // C 180ms LOW 100ms A 180ms LOW 100ms F 180ms LOW 400ms A 180ms LOW 100ms F 180ms LOW 1s
                     // ... C 180ms LOW 100ms A 180ms LOW 100ms F 180ms LOW 400ms A 180ms LOW 100ms F 180ms LOW 4s

void AlarmInit(void);

void MuteAlarm(void);
void HistAlarm(void);

void TestAlarm(int);

#ifdef	__cplusplus
}
#endif

#endif	/* ALARM_H */

