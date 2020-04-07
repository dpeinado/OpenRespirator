// Analog capture handling.
// Captures analog data and perform basic processing for direct use by rest of the software.

#include "mcc_generated_files/adcc.h"
#include "aCapture.h"
#include "ORespGlobal.h"

aSrcTyp curASrc;

// Variables store results in raw format, conversion is done only on call to get result.
// Pressure sensor.
// MPXV5004G
int16_t mainPSensCal = 780;
#define PSENS_K 1
// Currently sample every channel at 27.5KHz.

uint16_t resultTbl[ATOT_N];

uint8_t  resultTblVal[ATOT_N]; // 0, no value. >1, value valid, plus indication of value change.

// Table with correspondences.
adcc_channel_t adcGetCh(aSrcTyp sel){
    switch (sel){
        case MainPSensor:
            return channel_ANE1;
            break;
        case AuxPSensor:
            return channel_ANE2;
            break;
        case MicSensor:
            return channel_ANE0;
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
    aSrcTyp adcSel;
    
    adcData = ADCC_GetConversionResult();
    adcSel=curASrc;
    
    // Next capture.
    curASrc++;
    if (curASrc==ACAPT_N){
        curASrc=0;
    }
    ADCC_StartConversion(adcGetCh(curASrc));
            
    // Process current capture.
    if (adcSel<ACAPT_N){
        // IIR 1/4, output with 2 decimal bits.
        resultTbl[adcSel]=(3*resultTbl[adcSel] + 4*adcData)>>2;

        resultTblVal[adcSel]++;
        if (resultTblVal[adcSel]==0){
            resultTblVal[adcSel]=1;                
        }
        
        if (adcSel == MainPSensor) {
            // Compute also the other filters.
            resultTbl[Flt1PSensor]=(3*resultTbl[Flt1PSensor]+adcData)>>2;
            resultTbl[Flt2PSensor]=(7*resultTbl[Flt2PSensor]+adcData)>>3;
            resultTbl[Flt3PSensor]=(15*resultTbl[Flt3PSensor]+adcData)>>4;
            resultTblVal[Flt1PSensor]=resultTblVal[MainPSensor];
            resultTblVal[Flt2PSensor]=resultTblVal[MainPSensor];
            resultTblVal[Flt3PSensor]=resultTblVal[MainPSensor];
        }

    } else {
        // ERROR.
        ERROR_CONDITION(1);
    }
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


