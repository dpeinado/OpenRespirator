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
#define ATOT_N 7
    
#define MPRESSURE_MBAR(x) ((int16_t) 45*x)
#define APRESSURE_MBAR(x) ((int16_t) 100*x)
    
typedef enum{
    MainPSensor=0,
    AuxPSensor=1,
    MicSensor=2,
    Flt0PSensor=3,
    Flt1PSensor=4,
    Flt2PSensor=5,
    Flt3PSensor=6,
} aSrcTyp;

void aCaptureInit(void);

bool aCaptGetResult(aSrcTyp sel, int16_t *outVal);

// Initialized filter with the instantaneous value.
void aCaptRstFlt(aSrcTyp sel);

#ifdef	__cplusplus
}
#endif

#endif	/* ACAPTURE_H */

