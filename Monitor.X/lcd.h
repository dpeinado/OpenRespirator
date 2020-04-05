/* 
 * File:   lcd.h
 * Author: carlospardo
 *
 * Created on April 4, 2020, 2:05 PM
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

void LCDInit (void);
void LCDMessage(char *str);


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

