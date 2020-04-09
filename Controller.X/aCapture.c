// Analog capture handling.
// Captures analog data and perform basic processing for direct use by rest of the software.

#include "mcc_generated_files/adcc.h"
#include "aCapture.h"
#include "ORespGlobal.h"

aSrcTyp curASrc;

// Variables store results in raw format, conversion is done only on call to get result.
// Pressure sensor.
// MPXV5010
int16_t mainPSensCal = 190;
#define PSENS_K 1
// Currently sample every channel at 27.5KHz.

uint32_t resultTbl[ATOT_N];

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
    uint32_t adcData;
    aSrcTyp adcSel;

    adcData = ADCC_GetFilterValue();
//    adcData = ADCC_GetConversionResult();
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
        resultTbl[adcSel]=adcData;

        resultTblVal[adcSel]++;
        if (resultTblVal[adcSel]==0){
            resultTblVal[adcSel]=1;                
        }
        
        if (adcSel == MainPSensor) {
            // Compute also the other filters.
            // LPI with about 4ms Tau.
            resultTbl[Flt0PSensor]=(3*resultTbl[Flt0PSensor]+4*adcData)>>2;
            // LPI with about 16ms Tau.
            resultTbl[Flt1PSensor]=(15*resultTbl[Flt1PSensor]+16*adcData)>>4;
            // LPI with about 64ms Tau.
            resultTbl[Flt2PSensor]=(63*resultTbl[Flt2PSensor]+64*adcData)>>6;
            // LPI with about 2 seconds Tau. Must fit in 32 bit, full precision not possible on 32 bit.
            // Only loose 2 bits, not relevant for this.
            resultTbl[Flt3PSensor]=(1023*resultTbl[Flt3PSensor]+512*adcData)>>10;
            
            resultTblVal[Flt0PSensor]=resultTblVal[MainPSensor];
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

    ADCC_SetADTIInterruptHandler(adcCaptureIsr);
//    ADCC_SetADIInterruptHandler(adcCaptureIsr);

    ADCC_StartConversion(adcGetCh(curASrc));    
    // Enable ADC Irq.
    PIE1bits.ADTIE = 1;
}

// All results are 8 bit.
bool aCaptGetResult(aSrcTyp sel, int16_t *outVal){
    uint32_t lclRaw;
    uint8_t lclValid;
    
    // Get value from table.
    PIE1bits.ADTIE = 0;
    lclRaw=resultTbl[sel];
    lclValid=resultTblVal[sel];
    PIE1bits.ADTIE = 1;
    
    if (lclValid == 0) {
        return false;
    }
    
    // Scale.
    
    switch (sel){
        case Flt3PSensor:
            lclRaw=lclRaw>>9;
            break;
        case Flt2PSensor:
            lclRaw=lclRaw>>6;
            break;
        case Flt1PSensor:
            lclRaw=lclRaw>>4;
            break;
        case Flt0PSensor:
            lclRaw=lclRaw>>2;
            break;
    }
    
    switch (sel){
        case MainPSensor:
        case Flt0PSensor:
        case Flt1PSensor:
        case Flt2PSensor:
        case Flt3PSensor:
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
            ERROR_CONDITION(10);
    }
}


