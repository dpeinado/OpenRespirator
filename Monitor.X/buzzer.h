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

