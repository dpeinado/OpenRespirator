// Author: David Ortiz

#include "vMeasure.h"
#include "aCapture.h"
#include "cmath.h"

// Constant from acc diff pressure to ml (2ms acc time): 65 for 6mm orifice.
#define VOLK 65

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
        if (v2 > 20) {
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
