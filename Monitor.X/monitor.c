

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

int32_t tt1, tt2, tt3, tt4, ttExt, tt12, tt34;
int16_t tdi, tde, ti, te, bp, bpm;
int16_t pi,pe, maxPressure, minPressure, alarmMaxPressure, pHigh, pLow;
int16_t rpi, rpe, lrpi, lrpe;
int16_t hiLimit, loLimit;

int16_t GetTde(void) { return tde*2; } // Time in ms
int16_t GetTdi(void) { return tdi*2; } // Time in ms
int16_t GetTi(void) { return ti*2; } // Time in ms
int16_t GetTe(void) { return te*2; } // Time in ms
int16_t GetBp(void) { return bp*2; } // Time in ms
int16_t GetBpm(void) { return bpm; } // Breaths per minute
int16_t GetEp(void) { return (lrpe/5); } // 
int16_t GetIp(void) { return (lrpi/5); } // 
int16_t GetMaxPressure(void) {
    int temp = alarmMaxPressure/5;
    //printf("\r\n MAXP: %d\r\n", temp);
    alarmMaxPressure = 0;
    return temp;
}
int GetMonitorState(void) { return state; }
void ClearVars(void);
int16_t prSlowBuffer[25];
int16_t prFastBuffer[25];
int16_t tdeBuffer[10];
int16_t tdiBuffer[10];
int16_t numtdi, numtde;
int16_t count;
int16_t prSlow, prSlowDev, prSlowNumStable;
int16_t prFast, prFastDev;
int32_t tt;

void SetTarget(int16_t ip, int16_t ep, uint16_t br) {
    targetHigh = ip;
    targetLow  = ep;
    targetBp   = 60000/br;
}

void MonitorDump(void) {
    //printf("\r\nSlow: ");
    //for (int i=0; i<25; i++ ) printf(" %d", prSlowBuffer[i]);
    //printf(": %d\r\n", prSlow);
    
    //printf("\r\nFast: ");
    //for (int i=0; i<25; i++ ) printf(" %d", prFastBuffer[i]);
    //printf(": %d\r\n", prFast);
    
    printf("\r\nTdi: ");
    for (int i=0; i<10; i++ ) printf(" %d", tdiBuffer[i]*2);
    printf(": %d\r\n", tdi*2);
    
    printf("\r\nTde: ");
    for (int i=0; i<10; i++ ) printf(" %d", tdeBuffer[i]*2);
    printf(": %d\r\n", tde*2);

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
    static bool measPi=false;
    static bool measPe=false;
    
    next = state;
    
    uint16_t temp;
    
    //TST1_Toggle();
    
 //   ToggleAlarmLED();
    tt++;
    ttExt++;
    
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
 //       prSlowDev=0;
 //       for (int i=0; i<25; i++) prSlowDev +=ABS(prSlowBuffer[i]-prSlow);
 //       prSlowDev=prSlowDev/25;
    }
    
//    if (prSlowDev<5) prSlowNumStable++;
//    else prSlowNumStable=0;
    
//    if (prSlow==prFast) {
 //       TST2_SetHigh();
//    } else {
 //       TST2_SetLow();
//    }
    
    if (ttExt>6*500) { // Every 6 s we should have at least one breath
        printf("\r\n pLow: %d pHigh: %d Min: %d Max: %d pe: %d pi:%d \r\n", pLow/5, pHigh/5, minPressure/5, maxPressure/5, pe/5, pi/5);
        // Define Hi and Low limits for next cycle
        hiLimit = maxPressure - (maxPressure-minPressure)/5;
        loLimit = minPressure + (maxPressure-minPressure)/4;
        // Store estimation of last cycle
        pe = pLow;
        pi = pHigh; 
        
        // Reset maximum and minimum periodically
        pLow = minPressure;
        pHigh = maxPressure;
        maxPressure = 0;
        minPressure = 40*5;
        ttExt=0;
    }
    
    count ++;   
    if (count>=6*25) count =0; // 25*6 time for both filters to go around
    
    if (tt>45000) {
        printf("\r\n No breath detected in 1 min\r\n");
        ClearVars() ; // No breaths for more than 1 min
        measPe = false;
        measPi = false;
    }
    
    if (prSlow>alarmMaxPressure) {
        alarmMaxPressure = prSlow;
        //printf("\r\n MAXP: %d\r\n", pr/5);
    }
    
    if (prSlow>maxPressure) {
        maxPressure = prSlow;
        //printf("\r\n MAXP: %d\r\n", pr/5);
    }
    if (prSlow<minPressure) {
        minPressure = prFast;
        //printf("\r\n MAXP: %d\r\n", pr/5);
    }
   
    // Average signals within limits
    if (prSlow>hiLimit) {
        TST1_SetHigh();
        pHigh = (pHigh*9+prSlow)/10;
    }
    if (prSlow<loLimit) {
        TST1_SetLow();
        pLow = (pLow*9+prSlow)/10;    
    }

    // Calc start of breath 
    int16_t halfLimit;
    halfLimit = (hiLimit+loLimit)/2;
    
    if (prSlow<loLimit && prFast>= loLimit) { // Crossing low limit up
        TST1_SetHigh();
        if (tt>400) { // Filter more events during ramp-up
            bp = tt; 
            if (bp!=0) bpm = 60000/(bp*2);
 //           printf("\r\nUP12: %ld pr: %d bpm: %d\r\n", tt, prSlow/5, bpm);           
        }
        tt=0; // This is the start of time
        tt1 = tt;
        tt12 = tt;
        measPe=false;
        measPi=false;
        lrpe = rpe;
        if (lrpe>(targetLow+2*5)) SetEPAboveSetAlarm(); else ClearEPAboveSetAlarm();
        if (lrpe<(targetLow-2*5)) SetEPBellowSetAlarm(); else ClearEPBellowSetAlarm();

    }
    if (prSlow>hiLimit && prFast<= hiLimit) { // Crossing high limit down
        TST1_SetLow();
        tt3 = tt;
        tt34 = tt;
        measPe=false;
        measPi=false;
        lrpi = rpi;
        if (lrpi>(targetHigh+3*5)) SetIPAboveSetAlarm(); else ClearIPAboveSetAlarm();
        if (lrpi<(targetHigh-3*5)) SetIPBellowSetAlarm(); else ClearIPBellowSetAlarm(); 
    }
    // Starting state
    if ( state == STATE_OFF) {
        if (pi> (targetHigh+targetLow)/2) { // Don´t do anything until estimators are reasonable
            if (prSlow < targetLow) next = STATE_LOW; else next = STATE_HIGH;
        }
    }
    
    // Transition to HIGH at the top
    if (prSlow>=(pi-3*5) && state == STATE_LOW) { // State is LOW and I am in the top
        TST2_SetHigh();
        next = STATE_HIGH;
        tt2 = tt;
//        printf("\r\nUP: %ld %d\r\n", tt, prSlow/5);
        tdiBuffer[numtdi%10] = ((tt2-tt1));
        tdi = 0;
        for (int i=0; i<MIN(10,numtdi+1); i++) tdi = tdi + tdiBuffer[i];                   
        tdi = tdi/MIN(10,numtdi+1);
//        printf("\r\nTDI(%d): %d %d\r\n", numtdi%10, ((tt2-tt1)*2), tdi*2); 
        //MonitorDump();
        numtdi++;
        if (numtdi>=100) numtdi=10;
        measPi=true;
        rpi=pi;
    }
    
    if (prSlow<=(pe+2*5) && state == STATE_HIGH) {
        TST2_SetLow();
        next = STATE_LOW;
        tt4 = tt;
//        printf("\r\nDOWN: %ld %d\r\n", tt, prSlow/5);

        tdeBuffer[numtde%10] = ((tt4-tt3));
        tde = 0;
        for (int i=0; i<MIN(numtde+1,10); i++) tde = tde + tdeBuffer[i];                   
        tde = tde/MIN(numtde+1,10);
        //printf("\r\nTDE: %d %d @ %lu %d %d\r\n", ntde, tde, tt, prFast, prSlow);
        //MonitorDump();
        numtde++;
        if (numtde>=100) numtde=10;
        measPe = true;
        rpe = pe;
    }
    
    // New estimators
    if (measPi) rpi = (rpi*19+prSlow)/20;
    if (measPe) rpe = (rpe*19+prSlow)/20;
    
    // TBD: Timeout alarms of events
    if (tt>(bp+bp/5)) {
        if (!CircuitFailureAlarm()) printf("\r\n CIRCUIT FAILURE: No breath in %d ms\r\n", ((bp*12)/10)*2);
        SetCircuitFailureAlarm();
    }
    if (tt>(tt3+tt3/5) && state==STATE_HIGH) {
        if (!CircuitFailureAlarm()) printf("\r\n CIRCUIT FAILURE: No exhalation in %d ms\r\n", ((tt3*12)/10)*2);
        SetCircuitFailureAlarm();
    }
    
    
    if (state == STATE_CALIBRATE) {
        if (prFast == prSlow && prFast < ((targetLow+targetHigh)/2)) {
            static int cnt=0;
            adcOffset +=  prFast;
            cnt++;
            if (cnt==250) { // 500 ms
                //printf("\r\nC: %d %d\r\n", prFast, adcOffset);
                cnt = 0;
                DisplayCalibrate(prFast, adcOffset);
            }
            //DisplayCalibrate(prFast, adcOffset);
        }
    } 
            
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
    tt  = 0;
    ttExt = 0;
    tt1 = 0;
    tt2 = 0;
    tt3 = 0;
    tt4 = 0;
    tt12 = 0;
    tt34 = 0;
    bpm = 0;
    bp  = 0;
    
    pi = 0;
    pe = 0;
    rpi = targetHigh;
    rpe = targetLow;
    lrpi = targetHigh;
    lrpe = targetLow;
    maxPressure = 0;
    minPressure = 40*5;
    alarmMaxPressure = 0;
    prSlow = 0;
    prSlowDev = 0;
    prSlowNumStable = 0;
    hiLimit = 40*5;
    loLimit = 0;

    
    ClearEPAboveSetAlarm();
    ClearEPBellowSetAlarm();
    ClearIPAboveSetAlarm();
    ClearIPBellowSetAlarm();
 
}

void InitializePressure (void) {
    //ADCC_Initialize(); // Already in mcc.c
    ADCC_EnableContinuousConversion();
    ADCC_StartConversion(PRS);
    targetHigh = 30*5; // 0.2 mbar
    targetLow  = 7*5; // 0.2 mbar
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
