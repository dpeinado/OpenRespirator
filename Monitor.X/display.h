/* 
 * File:   display.h
 * Author: carlospardo
 *
 * Created on April 4, 2020, 2:21 PM
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

void InitDisplay(void);
void DisplayTask(void);
void ValueDisplay();
void AlarmDisplay(int idx, char *alarm);




#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */

