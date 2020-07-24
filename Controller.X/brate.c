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

#include "brate.h"

uint8_t  eBRate;
uint24_t bRateHist;
uint24_t bRatePtr;
uint8_t  bRateCnt;

// Keep Rate in last 20 inspirations.
void bRateInit(void) {
    bRateHist = 0;
    bRatePtr = 1;
    eBRate = 0;
    bRateCnt = 0;
}

void bRateUpdt(bool triggered) {
    bRatePtr = bRatePtr << 1;
    bRateCnt++;
    if (bRateCnt > 20){
        bRateCnt = 20;
    }
    if (bRatePtr == (0x1L << 20)) {
        bRatePtr = 1;
    }

    if (bRateHist & bRatePtr) {
        eBRate -= 5;
    }
    if (triggered) {
        eBRate += 5;
        bRateHist |= bRatePtr;
    } else {
        bRateHist &= ~bRatePtr;
    }
}

uint8_t bRateGet(void) {
    return eBRate*20/bRateCnt;
}
