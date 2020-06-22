// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA.
//
//  Copyright © 2020 Carlos Pardo
//  This file is part of project: OpenRespirator
//
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
void LCDMessage12(char *str1, char *str2);
void I2CSend(bool RS, bool RW, bool E, uint8_t D74);
void LCDOff(void);
void LCDOn(void);


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

