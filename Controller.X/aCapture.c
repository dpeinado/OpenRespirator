// Analog capture handling.
// Captures analog data and perform basic processing for direct use by rest of the software.

#include "mcc_generated_files/adcc.h"
#include "aCapture.h"
#include "ORespGlobal.h"

aSrcTyp curASrc;

// Variables store results in raw format, conversion is done only on call to get result.
// Pressure sensor.
// MPXV5004G
int16_t mainPSensCal = 1190;
#define PSENS_K 1

uint16_t resultTbl[ACAPT_N];

uint8_t  resultTblVal[ACAPT_N]; // 0, no value. >1, value valid, plus indication of value change.

// Table with correspondences.
adcc_channel_t adcGetCh(aSrcTyp sel){
    switch (sel){
        case MainPSensor:
            return channel_ANE2;
            break;
        default:
           // ERROR.
           ERROR_CONDITION(1);
    }
}

void adcCaptureIsr(void){
    // Store processed, captured data. Launch next capture.
    // Processed data is only 8 bits.
    uint16_t adcData;
    
    if (curASrc<ACAPT_N){
        resultTblVal[curASrc]=resultTblVal[curASrc]+1;
        if (resultTblVal[curASrc] == 0){
            resultTblVal[curASrc]=1;
        }
        resultTbl[curASrc]=ADCC_GetConversionResult();
    } else {
        // ERROR.
        ERROR_CONDITION(1);
    }
    
    // Next capture.
    curASrc++;
    if (curASrc==ACAPT_N){
        curASrc=0;
    }
    ADCC_StartConversion(adcGetCh(curASrc));
}

// Analog capture initialization.
void aCaptureInit(void){
    int idx;
    curASrc=MainPSensor;

    for (idx=0;idx<ACAPT_N;idx++)
        resultTblVal[idx]=0;

    ADCC_StartConversion(adcGetCh(curASrc));    
    ADCC_SetADIInterruptHandler(adcCaptureIsr);
    // Enable ADC Irq.
    PIE1bits.ADIE = 1;
}

// All results are 8 bit.
bool aCaptGetResult(aSrcTyp sel, int16_t *outVal){
    uint16_t lclRaw;
    uint8_t lclValid;
    
    // Get value from table.
    PIE1bits.ADIE = 0;
    lclRaw=resultTbl[sel];
    lclValid=resultTblVal[sel];
    PIE1bits.ADIE = 1;
    
    if (lclValid == 0) {
        return false;
    }
    
    switch (sel){
        case MainPSensor:
            if (lclRaw < mainPSensCal) {
                lclRaw = mainPSensCal-lclRaw;
                *outVal = - (lclRaw/PSENS_K);
            } else {
                lclRaw = lclRaw - mainPSensCal;
                *outVal=(lclRaw/PSENS_K);
            }
            return true;
        default:
            // ERROR.
            ERROR_CONDITION(1);
    }
}


