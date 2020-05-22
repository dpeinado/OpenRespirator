/* 
 * File:   buttons.h
 * Author: carlospardo
 *
 * Created on April 4, 2020, 4:56 PM
 */

#ifndef BUTTONS_H
#define	BUTTONS_H

#ifdef	__cplusplus
extern "C" {
#endif

void ClearAlarmLED(void);
void SetAlarmLED(void);
void ButtonTask(void);
void ToggleAlarmLED(void);
void ButtonInit(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BUTTONS_H */

