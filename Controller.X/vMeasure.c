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

#include "ORespGlobal.h"
#include "vMeasure.h"
#include "aCapture.h"
#include "cmath.h"

// Constant from acc diff pressure to ml (2ms acc time): 65 for 6mm orifice.
#define VOLK 55
uint32_t volume;

void vMeasureIsr(void) {
    int16_t instP;
    uint16_t v1, v2;
    if (aCaptGetResult(VolPSensor, &instP)) {
        if (instP > 0) {
            v1 = instP << 4;
            v2 = isqrt(v1);
        } else {
            v1 = 0;
            v2 = 0;
        }
        if (v2 > ZVOLTH) {
            volume = volume + v2;
        }
    }
}

void vMeasureRst(void) {
    PIE4bits.TMR2IE = 0;
    volume = 0;
    PIE4bits.TMR2IE = 1;
}

void vMeasureInit(void) {
    volume = 0;
    TMR2_SetInterruptHandler(vMeasureIsr);
    TMR2_Start();
}

// Return volume in ml.
uint16_t vMeasureGet(void) {
    uint32_t rawV;
    PIE4bits.TMR2IE = 0;
    rawV=volume;
    PIE4bits.TMR2IE = 1;
    
    return rawV/VOLK;
}
