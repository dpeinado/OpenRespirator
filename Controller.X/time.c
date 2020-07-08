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
// Author: David Ortiz
//  This file is part of project: OpenRespirator
//

#include "mcc_generated_files/tmr0.h"
#include "mcc_generated_files/tmr1.h"
#include "time.h"

void timeInit(void){
    TMR0_StartTimer();
    TMR1_StartTimer();
}

// TIME function. Maintains global time. Resolution 0.1ms. Span about 6s. 
time_t timeGet(void){
    return TMR1_ReadTimer();
}

// Returns time difference between start and end times.
time_t timeDiff(time_t startT, time_t endT){
    time_t intDur;
    
    intDur = endT - startT;
    if (intDur>0x8000){
        // One of the counters have overflowed.
        intDur = startT-endT;
    }
    return intDur;
}

// Checks if time has finish and refresh tstamp.
bool timeElapsedR(time_t *prevTime, time_t duration){
    time_t intTime, intDur;
    intTime = TMR1_ReadTimer();
    intDur = intTime - *prevTime;
    if (intDur < duration) {
        return false;
    } else {
        *prevTime = intTime;
        return true;
    }
}

// Only checks if time hasfinished.
bool timeElapsed(time_t prevTime, time_t duration){
    time_t intTime, intDur;
    intTime = TMR1_ReadTimer();
    intDur = intTime - prevTime;
    if (intDur < duration) {
        return false;
    } else {
        return true;
    }
}

void timeDelayMs(time_t delms){
    time_t tstamp;
    tstamp=timeGet();
    while (!timeElapsed(tstamp, TIME_MS(delms)));
}

