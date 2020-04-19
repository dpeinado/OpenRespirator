// Analog capture handling.
// Captures analog data and perform basic processing for direct use by rest of the software.

#include "mcc_generated_files/adcc.h"
#include "aCapture.h"
#include "ORespGlobal.h"

aSrcTyp curASrc;

// Variables store results in raw format, conversion is done only on call to get result.
// Pressure sensor.
// MPXV5010
int16_t mainPSensCal = 164;
int16_t auxPSensCal = 962;
#define PSENS_K 1
#define ASENS_K 1
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
        case VddSensor:
            return channel_ANE0;
            break;
        default:
           // ERROR.
           ERROR_CONDITION(100);
           return -1;
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
    if (curASrc <= AuxPSensor ){
        ADCON0bits.ADON = 0;
        // ADNREF VSS; ADPREF VDD; 
        ADREF = 0x00;
    } else {
        ADCON0bits.ADON = 0;
        // ADNREF VSS; ADPREF FVR; 
        ADREF = 0x03;
    }
    ADCC_StartConversion(adcGetCh(curASrc));
            
    // Process current capture.
    if (adcSel<ACAPT_N){
        resultTbl[adcSel]=adcData;

        resultTblVal[adcSel]++;
        if (resultTblVal[adcSel]==0){
            resultTblVal[adcSel]=1;                
        }
        
        if (adcSel == MainPSensor) {
            // Compute also the other filters.
            // LPI with about 4ms Tau.
            resultTbl[Flt0PSensor]=(3*resultTbl[Flt0PSensor]+(adcData<<2))>>2;
            // LPI with about 16ms Tau.
            resultTbl[Flt1PSensor]=(15*resultTbl[Flt1PSensor]+(adcData<<4))>>4;
            // LPI with about 64ms Tau.
            resultTbl[Flt2PSensor]=(63*resultTbl[Flt2PSensor]+(adcData<<6))>>6;
            // LPI with about 0.5 seconds Tau. 
            resultTbl[Flt3PSensor]=(511*resultTbl[Flt3PSensor]+(adcData<<9))>>9;
            
            resultTblVal[Flt0PSensor]=resultTblVal[MainPSensor];
            resultTblVal[Flt1PSensor]=resultTblVal[MainPSensor];
            resultTblVal[Flt2PSensor]=resultTblVal[MainPSensor];
            resultTblVal[Flt3PSensor]=resultTblVal[MainPSensor];
        } 
    } else {
        // ERROR.
        ERROR_CONDITION(101);
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

void aCaptureSetOff(aSrcTyp sel, int16_t offVal){
    if (sel == MainPSensor) {
        mainPSensCal = offVal;
    } else if (sel == AuxPSensor) {
        auxPSensCal = offVal;
    } else {
        ERROR_CONDITION(102);
    }
}

void aCaptRstFlt(aSrcTyp sel) {
    switch (sel) {
            PIE1bits.ADTIE = 0;
        case Flt3PSensor:
            resultTbl[sel] = resultTbl[MainPSensor]<<9;
            break;
        case Flt2PSensor:
            resultTbl[sel] = resultTbl[MainPSensor]<<6;
            break;
        case Flt1PSensor:
            resultTbl[sel] = resultTbl[MainPSensor]<<4;
            break;
        case Flt0PSensor:
            resultTbl[sel] = resultTbl[MainPSensor]<<2;
            break;
        default:
            // ERROR.
            ERROR_CONDITION(103);
    }
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
        default:
            // Nothing to be done.
            break;
    }
    
    switch (sel){
        case MainPSensor:
        case Flt0PSensor:
        case Flt1PSensor:
        case Flt2PSensor:
        case Flt3PSensor:
            if (lclRaw < mainPSensCal) {                lclRaw = mainPSensCal-lclRaw;
                *outVal = - (lclRaw/PSENS_K);
            } else {
                lclRaw = lclRaw - mainPSensCal;
                *outVal=(lclRaw/PSENS_K);
            }
            return true;
        case AuxPSensor:
            *outVal = (lclRaw - auxPSensCal)/ASENS_K;
            return true;
        case VddSensor:
            *outVal = lclRaw<<1;
            return true;
        default:
            // ERROR.
            ERROR_CONDITION(104);
    }
}


