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
void SetAlarmLED(uint8_t);
void ButtonTask(void);
void ToggleAlarmLED(void);
void ButtonInit(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BUTTONS_H */

