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
    
#define ACAPT_N 1

typedef enum{
    MainPSensor=0,
    SndPSensor=1,
    TempSensor=2
} aSrcTyp;

void aCaptureInit(void);

bool aCaptGetResult(aSrcTyp sel, int16_t *outVal);

#ifdef	__cplusplus
}
#endif

#endif	/* ACAPTURE_H */

