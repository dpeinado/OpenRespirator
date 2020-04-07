/* 
 * File:   aCapture.h
 * Author: David
 *
 * Created on April 4, 2020, 12:19 PM
 */

#ifndef ACAPTURE_H
#define	ACAPTURE_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define ACAPT_N 3
#define ATOT_N 6
    
// Captures are converted from 12 to 14 bits by IIR filter 1/4,3/4, increasing resolution, for all captures.
#define MPRESSURE_MBAR(x) ((int16_t) 4*45*x)
#define APRESSURE_MBAR(x) ((int16_t) 4*100*x)
    
typedef enum{
    MainPSensor=0,
    AuxPSensor=1,
    MicSensor=2,
    Flt1PSensor=3,
    Flt2PSensor=4,
    Flt3PSensor=5,
} aSrcTyp;

void aCaptureInit(void);

bool aCaptGetResult(aSrcTyp sel, int16_t *outVal);

#ifdef	__cplusplus
}
#endif

#endif	/* ACAPTURE_H */

