

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/mcc.h"
#include "monitor.h"

void InitializePressure (void) {
    //ADCC_Initialize(); // Already in mcc.c
    ADCC_EnableContinuousConversion();
    ADCC_StartConversion(channel_ANA0);
}


uint16_t GetPressure_mbar (void) {
    
    return GetPressure_pa()/100;
}

uint8_t GetPressure_kpa (void) {
    
    return GetPressure_pa()/1000;
}

uint16_t GetPressure_pa (void) {
    
    adc_result_t adc = ADCC_GetConversionResult();
    uint24_t mv = ( adc * 5000 )/ 4096;  // Move from 12 bits to 5V range
    uint16_t p = mv - 1000 ; // remove offset of 1V
    return p;
}

void SetSV1(bool value) {
    if (value) IO_RA1_SetHigh();
    else IO_RA1_SetLow();
}
