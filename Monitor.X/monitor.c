

#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/mcc.h"
#include "monitor.h"
#include "buttons.h"
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

uint32_t tt1, tt2, tt3, tt4;
int16_t tdi, tde, ti, te, bp;
uint8_t pi,pe;

uint16_t GetTde(void) { return ((uint8_t) tde)*2; } // Time in ms
uint16_t GetTdi(void) { return ((uint8_t) tdi)*2; } // Time in ms
uint16_t GetTi(void) { return ((uint16_t) ti)*2; } // Time in ms
uint16_t GetTe(void) { return ((uint16_t) te)*2; } // Time in ms
uint16_t GetBp(void) { return ((uint16_t) bp)*2; } // Time in ms
uint8_t GetPe(void) { return ((uint8_t) pe/5); } // 
uint8_t GetPi(void) { return ((uint8_t) pi/5); } // 
int GetMonitorState(void) { return state; }
uint8_t prSlowBuffer[25];
uint8_t prFastBuffer[25];
uint16_t tdeBuffer[10];
uint16_t tdiBuffer[10];
uint8_t numtdi, numtde;
uint16_t count;
uint8_t prSlow;
uint8_t prFast;
uint32_t tt;

void MonitorDump(void) {
    printf("\r\nSlow: ");
    //for (int i=0; i<25; i++ ) printf(" %d", prSlowBuffer[i]);
    printf(": %d\r\n", prSlow);
    
    printf("\r\nFast: ");
    //for (int i=0; i<25; i++ ) printf(" %d", prFastBuffer[i]);
    printf(": %d\r\n", prFast);
    
    printf("\r\nTdi: ");
    for (int i=0; i<10; i++ ) printf(" %d", tdiBuffer[i]);
    printf(": %d\r\n", tdi);
    
    printf("\r\nTde: ");
    for (int i=0; i<10; i++ ) printf(" %d", tdeBuffer[i]);
    printf(": %d\r\n", tde);

}

#define ABS(x) ((x>0)?(x):(-x))
#define MIN(x,y) ((x>y)?(y):(x))
#define MAX(x,y) ((x>y)?(x):(y))

void MonitorPressureTask(void) {
    uint8_t pr;
    int next;
    next = state;
    
    uint16_t temp;
    
 //   ToggleAlarmLED();
    tt++;
    
    pr = GetPressure_mbar02();
    //tt = tick_get_slow();  
    prFastBuffer[count%25]=pr;
    temp = 0;
    for (int i=0; i<25; i++) temp +=prFastBuffer[i];
    prFast = temp/25;
    if (count%6==0) {
        prSlowBuffer[count/6]=pr;
        temp = 0;
        for (int i=0; i<25; i++) temp +=prSlowBuffer[i];
        prSlow = temp/25;
    }
    count ++;
    if (count>=6*25) count =0;
    
    
    switch (state) {
        case STATE_OFF:
            if (pr<targetLow)  next = STATE_LOW;
            if (pr>targetHigh) next = STATE_HIGH;
            break;
        case STATE_CALIBRATE:
            break;
        case STATE_LOW:   
            if (prFast>(prSlow)) {
                bp = tt-tt1;              
                te = tt-tt3;
                if (tt>16000) tt =0;
                tt1 = tt;              
                next = STATE_TDI;
                //printf("\r\nUP: %lu %d %d\r\n", tt, prFast, prSlow);
            }
            break;
        case STATE_TDI:
            if (prFast==prSlow) {
                int16_t ntdi;
                tt2 = tt;
                next = STATE_HIGH;
                ntdi = tt2-tt1;

                //if (numtdi<10 || (ABS(ntdi-tdi)<tdi/3)) { // New samples within 33%
                if (true) {
                    tdiBuffer[numtdi%10] = ntdi;
                    pi = prFast;
                    tdi = 0;
                    for (int i=0; i<MIN(10,numtdi+1); i++) tdi = tdi + tdiBuffer[i];                   
                    tdi = tdi/MIN(10,numtdi+1);
                    //printf("\r\nTDI: %d %d @ %lu %d %d\r\n", ntdi, tdi, tt, prFast, prSlow); 
                    //MonitorDump();
                }
                numtdi++;
                if (numtdi>=100) numtdi=10;
            }
            break;
        case STATE_HIGH:
            if ((prFast)<prSlow) {
                ti = tt-tt1;
                tt3 = tt;
                next = STATE_TDE;
                //printf("\r\nDOWN: %lu %d %d\r\n", tt, prFast, prSlow);
            }
            break;
        case STATE_TDE:
            if (prFast>=prSlow) {
                int16_t ntde;
                tt4 = tt;
                next = STATE_LOW;
                ntde = tt4-tt3;
               
                //if (numtde<10 || (ABS(ntde-tde)<tde/3)) { // New samples within 33%
                if (true) {
                    tdeBuffer[numtde%10] = ntde;
                    pe = prFast;
                    tde = 0;
                    for (int i=0; i<MIN(numtde+1,10); i++) tde = tde + tdeBuffer[i];                   
                    tde = tde/MIN(numtde+1,10);
                    //printf("\r\nTDE: %d %d @ %lu %d %d\r\n", ntde, tde, tt, prFast, prSlow);
                    //MonitorDump();
                }
                numtde++;
                if (numtde>=100) numtde=10; 
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
    ADCC_StartConversion(PRS);
    targetHigh = 25; // mbar
    targetLow  = 10; // mbar
    adcOffset = 0; // Read from EEPROM or calibrate. ADC counts
    state = STATE_OFF;
    count = 0;
    tt = 0;
    for (int i=0; i<25; i++) {
        prFastBuffer[i] = 0;
        prSlowBuffer[i] = 0;
    }
    for (int i=0; i<10; i++) {
        tdiBuffer[i] = 0;
        tdeBuffer[i] = 0;
    }
    numtdi=0;
    numtde=0;
    ti =0;
    te =0;
    tt1=0;
    tt2=0;
    tt3=0;
    tt4=0;
    bp=0;
    
    pi = 0;
    pe = 0;
    
    TMR0_SetInterruptHandler(MonitorPressureTask);
}

uint8_t GetPressure_mbar02 (void) {
    
    return GetPressure_pa()/20;
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
    if (value) VALVE_SetHigh();
    else VALVE_SetLow();
}
