

#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/mcc.h"
#include "monitor.h"
#include "tick.h"

int adcOffset;
int targetHigh;
int targetLow;
int timeUp;

#define STATE_OFF           0
#define STATE_CALIBRATE     1
#define STATE_LOW           2
#define STATE_TDI           3
#define STATE_HIGH          4
#define STATE_TDE           5

int state;

uint32_t tt1, tt2, tt3, tt4, tdi, tde;


uint32_t GetTde(void) { return tde; }
uint32_t GetTdi(void) { return tdi; }
int GetMonitorState(void) { return state; }
uint8_t prSlowBuffer[25];
uint8_t prFastBuffer[25];
uint16_t count;
uint8_t prSlow;
uint8_t prFast;


void MonitorPressureTask(void) {
    uint8_t pr;
    int next = state;
    uint32_t tt;
    uint16_t temp;
    
    pr = GetPressure_mbar();
    tt = tick_get_slow();
    count ++;
    prFastBuffer[count%25]=pr;
    temp = 0;
    for (int i=0; i<25; i++) temp +=prSlowBuffer[i];
    prFast = temp/25;
    if (count%40==0) {
        prSlowBuffer[count/40]=pr;
        temp = 0;
        for (int i=0; i<25; i++) temp +=prSlowBuffer[i];
        prSlow = temp/25;
    }
    if (count==40*25*25) count =0;
    
    
    switch (state) {
        case STATE_OFF:
            if (pr<targetLow)  next = STATE_LOW;
            if (pr>targetHigh) next = STATE_HIGH;
            break;
        case STATE_CALIBRATE:
            break;
        case STATE_LOW:
            tt1 = tt;
            if (prFast>prSlow) {
                next = STATE_TDI;
            }
            break;
        case STATE_TDI:
            if (prFast<=prSlow) {
                tt2 = tt;
                next = STATE_HIGH;
                tdi = tt2-tt1;
                printf("\r\nTDI: %lu\r\n", tdi);    
            }
            break;
        case STATE_HIGH:
            tt3 = tt;
            if (prFast<prSlow) {
                next = STATE_TDE;
            }
            break;
        case STATE_TDE:
            if (prFast>=prSlow) {
                tt4 = tt;
                next = STATE_LOW;
                tde = tt4-tt3;
                printf("\r\nTDE: %lu\r\n", tde);
            }
            break;
        default:
            break; 
    };
    state = next;
    
}

void InitializePressure (void) {
    //ADCC_Initialize(); // Already in mcc.c
    ADCC_EnableContinuousConversion();
    ADCC_StartConversion(channel_ANA0);
    targetHigh = 25; // mbar
    targetLow  = 10; // mbar
    adcOffset = 0; // Read from EEPROM or calibrate. ADC counts
    state = STATE_OFF;
    count =0;
}


uint8_t GetPressure_mbar (void) {
    
    return GetPressure_pa()/100;
}

uint8_t GetPressure_kpa (void) {
    
    return GetPressure_pa()/1000;
}

uint16_t GetPressure_pa (void) {
    
    adc_result_t adc = ADCC_GetConversionResult();
    //printf("ADC: %d ", adc);
    uint32_t mv = adc;
    mv = ( mv * 5000 )/ 4096;  // Move from 12 bits to 5V range
    //printf("V: %d mV ", mv);
    int16_t p = mv - 1000 - adcOffset; // remove offset of 1V
    if (p<0) p=0;
    return p;
}

void SetSV1(bool value) {
    if (value) IO_RA1_SetHigh();
    else IO_RA1_SetLow();
}
