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
void LCDMessage1(char *str);
void LCDMessage2(char *str);
void I2CSend(bool RS, bool RW, bool E, uint8_t D74);


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

