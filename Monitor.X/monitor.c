

#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/mcc.h"
#include "monitor.h"
#include "display.h"
#include "alarm.h"
#include "buttons.h"
#include "tick.h"

int16_t adcOffset;
int16_t targetHigh;
int16_t targetLow;
uint16_t targetBp;

#define STATE_OFF           0
#define STATE_CALIBRATE     1
#define STATE_LOW           2
#define STATE_TDI           3
#define STATE_HIGH          4
#define STATE_TDE           5

int state;

uint32_t tt1, tt2, tt3, tt4;
int16_t tdi, tde, ti, te, bp;
int16_t pi,pe, maxPressure;

uint16_t GetTde(void) { return ((uint8_t) tde)*2; } // Time in ms
uint16_t GetTdi(void) { return ((uint8_t) tdi)*2; } // Time in ms
uint16_t GetTi(void) { return ((uint16_t) ti)*2; } // Time in ms
uint16_t GetTe(void) { return ((uint16_t) te)*2; } // Time in ms
uint16_t GetBp(void) { return ((uint16_t) bp)*2; } // Time in ms
int16_t GetEp(void) { return (pe/5); } // 
int16_t GetIp(void) { return (pi/5); } // 
int16_t GetMaxPressure(void) {
    int temp = maxPressure/5;
    printf("\r\n MAXP: %d\r\n", temp);
    maxPressure = 0;
    return temp;
}
int GetMonitorState(void) { return state; }
void ClearVars(void);
int16_t prSlowBuffer[25];
int16_t prFastBuffer[25];
uint16_t tdeBuffer[10];
uint16_t tdiBuffer[10];
uint16_t numtdi, numtde;
uint16_t count;
int16_t prSlow;
int16_t prFast;
uint32_t tt;

void SetTarget(int16_t ip, int16_t ep, uint16_t br) {
    targetHigh = ip;
    targetLow  = ep;
    targetBp   = 60000/br;
}

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

void SetCalibrateState(bool calib) {
    TMR0_StopTimer();
    if (calib) {
        //printf("\r\nSTART CALIBRATE\r\n");
        state = STATE_CALIBRATE;
    } else {
        //printf("\r\nSTOP CALIBRATE\r\n");
        printf("\r\nC: %d %d\r\n", GetPressure_mbar02(), adcOffset);
        state = STATE_OFF;
        UnDisplayCalibrate();
    }
    ClearVars();
    TMR0_StartTimer();
}

void MonitorPressureTask(void) { // Every 2 ms
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
    if (pr>maxPressure) {
        maxPressure = pr;
        //printf("\r\n MAXP: %d\r\n", pr/5);
    }
    
    if (count>=6*25) count =0;
    
    if (tt>45000) ClearVars() ; // No breaths for more than 1 min
    
    switch (state) {
        case STATE_OFF:
            if (prSlow<=targetLow)  next = STATE_LOW;
            if (prSlow>=targetHigh) next = STATE_HIGH;
            break;
        case STATE_CALIBRATE:
            if (prFast == prSlow && prFast < ((targetLow+targetHigh)/2)) {
                static int cnt=0;
                adcOffset +=  prFast;
                cnt++;
                if (cnt==250) { // 500 ms
                    printf("\r\nC: %d %d\r\n", prFast, adcOffset);
                    cnt = 0;
                    DisplayCalibrate(prFast, adcOffset);
                }
                //DisplayCalibrate(prFast, adcOffset);
            }
            break;
        case STATE_LOW:   
            if (prFast>prSlow && prSlow<= ((targetLow+targetHigh)/2)) {
                bp = tt-tt1;              
                te = tt-tt3;
                if (tt>16000) tt =0;
                tt1 = tt;              
                next = STATE_TDI;
                //printf("\r\nUP: %lu %d %d\r\n", tt, prFast, prSlow);
            }
            break;
        case STATE_TDI:
            if (prFast==prSlow && prSlow>=((targetLow+targetHigh)/2)) {
                int16_t ntdi;
                tt2 = tt;
                next = STATE_HIGH;
                ntdi = tt2-tt1;

                //if (numtdi<10 || (ABS(ntdi-tdi)<tdi/3)) { // New samples within 33%
                if (true) {
                    tdiBuffer[numtdi%10] = ntdi;
                    pi = prFast;
                    if (pi>targetHigh+3*5) SetIPAboveSetAlarm(); else ClearIPAboveSetAlarm();
                    if (pi<targetHigh-3*5) SetIPBellowSetAlarm(); else ClearIPBellowSetAlarm();
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
            if ((prFast)<prSlow && prSlow>=((targetLow+targetHigh)/2)) {
                ti = tt-tt1;
                tt3 = tt;
                next = STATE_TDE;
                //printf("\r\nDOWN: %lu %d %d\r\n", tt, prFast, prSlow);
            }
            break;
        case STATE_TDE:
            if (prFast>=prSlow && prSlow<=((targetLow+targetHigh)/2)) {
                int16_t ntde;
                tt4 = tt;
                next = STATE_LOW;
                ntde = tt4-tt3;
               
                //if (numtde<10 || (ABS(ntde-tde)<tde/3)) { // New samples within 33%
                if (true) {
                    tdeBuffer[numtde%10] = ntde;
                    pe = prFast;
                    if (pe>targetLow+2*5) SetEPAboveSetAlarm(); else ClearEPAboveSetAlarm();
                    if (pe<targetLow-2*5) SetEPBellowSetAlarm(); else ClearEPBellowSetAlarm();
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

void ClearVars(void) {
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
    numtdi = 0;
    numtde = 0;
    ti = 0;
    te = 0;
    tdi = 0;
    tde = 0;
    tt1 = 0;
    tt2 = 0;
    tt3 = 0;
    tt4 = 0;
    bp  = 0;
    
    pi = 0;
    pe = 0;
    maxPressure = 0;
 
}

void InitializePressure (void) {
    //ADCC_Initialize(); // Already in mcc.c
    ADCC_EnableContinuousConversion();
    ADCC_StartConversion(PRS);
    targetHigh = 25*5; // 0.2 mbar
    targetLow  = 5*5; // 0.2 mbar
    adcOffset = 0; // Read from EEPROM or calibrate. ADC counts
    state = STATE_OFF;
    ClearVars();
    TMR0_SetInterruptHandler(MonitorPressureTask);
    TMR0_StartTimer();
}

int16_t GetPressure_mbar02 (void) {
    
    return GetPressure_pa()/20;
}

int16_t GetPressure_mbar (void) {
    
    return GetPressure_pa()/100;
}

int16_t GetPressure_kpa (void) {
    
    return GetPressure_pa()/1000;
}

int16_t GetPressure_pa (void) {
    
    adc_result_t adc = ADCC_GetConversionResult();
    //printf("ADC: %d ", adc);
    uint32_t mv = adc;
    mv = ( mv * 5000 )/ 4096;  // Move from 12 bits to 5V range
    //printf("V: %d mV ", mv);
    int16_t p = mv;
    p = p - 1000 - adcOffset; // remove offset of 1V
    // if (p<0) p=0;
    return p;
}

void SetSV1(bool value) {
    if (value) VALVE_SetHigh();
    else VALVE_SetLow();
}
