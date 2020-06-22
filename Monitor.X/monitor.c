// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA.
//
//  Copyright © 2020 Carlos Pardo
//  This file is part of project: OpenRespirator
//


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
#include "../Controller.X/cmath.h"

#define ABS(x) (((x)>0)?(x):(-x))
#define MIN(x,y) ((x)>(y)?(y):(x))
#define MAX(x,y) ((x)>(y)?(x):(y))

#define NUM_CHANNELS 4

struct adcChannel {
    adcc_channel_t ch;
    volatile bool dataValid;
    volatile adc_result_t data;
    uint8_t ref;
} ;
static int currentChannel;

static struct adcChannel adcChannels[] = {
    {PRS, false, 0, 0}, // #define ADC_ID_PRS 0
    {MEAS5, false, 0, 3}, // #define ADC_ID_5V 1
    {PRSV, false, 0, 0},// #define ADC_ID_VOL 2
    {MEAS12,false, 0, 3}// #define ADC_ID_12V 3
};

void AdcHandler(void);

void AdcInit(void) {
    currentChannel = 0;
    ADCC_SetADIInterruptHandler(AdcHandler);
    
    ADCON0bits.ADON = 0;
    ADREF = adcChannels[currentChannel].ref;
    ADCC_StartConversion(adcChannels[currentChannel].ch);
}

bool AdcReady(void) {
    return ADCC_IsConversionDone();
}

void AdcReStartCycle(void) {
    currentChannel = 0;
    ADCON0bits.ADON = 0;  // Disable
    ADREF = adcChannels[currentChannel].ref; // Change reference
    ADCC_StartConversion(adcChannels[currentChannel].ch);
}

bool AdcDataReady(int id) {
    return adcChannels[id].dataValid;
}

adc_result_t AdcSeeData(int id) {
    adc_result_t data;
    data = adcChannels[id].data;
    //adcChannels[id].dataValid=false;
    return data;
}

adc_result_t AdcGetData(int id) {
    adc_result_t data;
    data = adcChannels[id].data;
    adcChannels[id].dataValid=false;
    return data;
}

void AdcHandler(void) {
    static int cnt = 0;
    //if (cnt==0) printf("adc:%02X\r\n", ADSTAT);

    
    if (!AdcReady()) {
        printf("EAI: %02X\r\n", ADSTAT);
        ADCC_Initialize();
        AdcInit();
        return;
    }
    // Interrupt received from ADC
    adcChannels[currentChannel].data = ADCC_GetConversionResult();
    adcChannels[currentChannel].dataValid = true;
    
    // New channel
    if (currentChannel < (NUM_CHANNELS-1))
    {
        // Start new conversion
        currentChannel++;
        ADCON0bits.ADON = 0;  // Disable
        ADREF = adcChannels[currentChannel].ref; // Change reference
        ADCC_StartConversion(adcChannels[currentChannel].ch); // Enable & Start
    } else {
        // Wait for TIMER0 to start a new cycle
        currentChannel = 0;
        // ADC Signal every two seconds
        cnt++;
        if (cnt>=1000) { cnt = 0;  }
        if (cnt==0) printf("o\r\n");
    }   
}


static int16_t adcOffset, adcVOffset;
static int16_t targetHigh;
static int16_t targetLow;
static int16_t targetPmax;
static uint16_t targetBp;
static bool enable;

#define STATE_OFF           0
#define STATE_CALIBRATE     1
#define STATE_LOW           2
#define STATE_HIGH          4

static int state;

static uint32_t volumeAcc;
static int16_t volume;
static int16_t volumeMax, volumeMin;
static int16_t tt1, tt2, tt3, tt4, ttExt, tt12, tt34, ttNoAlarm, ttAlarm;
static bool enableAlarms;
static bool volumeControl;
static int16_t tdi, tde, ti, te, bp, bpm;
static int16_t pi,pe, pmax, pmin, maxPressure, minPressure, alarmMaxPressure, pHigh, pLow;
static int32_t pHighAcc, pLowAcc;
static int16_t numHigh, numLow;
static bool alarmSV1;
static bool normalSV1;
static bool disableAlarmSV1;
static int16_t rpi, rpe, lrpi, lrpe;
static int16_t hiLimit, loLimit;

int16_t GetTde(void) { return tde*2; } // Time in ms
int16_t GetTdi(void) { return tdi*2; } // Time in ms
int16_t GetTi(void) { return ti*2; } // Time in ms
int16_t GetTe(void) { return te*2; } // Time in ms
int16_t GetBp(void) { return bp*2; } // Time in ms
int16_t GetBpm(void) { return bpm; } // Breaths per minute
int16_t GetEp(void) { return MAX(MAX(lrpe/5, pLow/5), pe/5); } // 
int16_t GetIp(void) { return MAX(MAX(lrpi/5, pHigh/5), pi/5); } // 
int16_t GetVolume(void) { return volume; }
int16_t GetPmax(void) { return pmax/5; }
int16_t GetMaxPressure(void) {
    int temp = alarmMaxPressure/5;
    //printf("\r\n MAXP: %d\r\n", temp);
    alarmMaxPressure = 0;
    return temp;
}
int GetMonitorState(void) { return state; }
void ClearVars(void);
static int16_t prSlowBuffer[25];
static int16_t prFastBuffer[25];
static int16_t tdeBuffer[10];
static int16_t tdiBuffer[10];
static int16_t numtdi, numtde;
static int16_t count;
static int16_t prSlow, prSlowDev, prSlowNumStable;
static int16_t prFast, prFastDev;
static int32_t tt;

int16_t GetTargetIp() {
    return targetHigh/5; 
} 

int16_t GetTargetEp() {
    return targetLow/5; 
}

void SetVolumeControl(bool vc) { volumeControl = vc; }

void SetTarget(int16_t pmax, int16_t ip, int16_t ep, uint16_t br, int16_t vmax, int16_t vmin) {
    //if (ip!= targetHigh/5) printf("\r\n IP: %d \r\n", ip);
    //if (ep!= targetLow/5) printf("\r\n EP: %d \r\n", ep);
    //if (vmax!= volumeMax) printf("\r\n Vmax: %d \r\n", vmax);
    //if (vmin!= volumeMin) printf("\r\n Vmin: %d \r\n", vmin);
    //if (pmax!= targetPmax/5) printf("\r\n Pmax: %d \r\n", pmax);
    //if (ip!= targetHigh) printf("\r\n IP: %d \r\n", ip);
    targetHigh = ip*5;
    targetLow  = ep*5;
    if (br!=0) targetBp   = 60000/br; else targetBp = 3000;
    targetPmax = pmax*5;
    volumeMax = vmax;
    volumeMin = vmin;
}

void MonitorDump(void) {
    
    
            printf("\r\nNew Breath: TT12: %d TT2: %d TT3: %d TT34: %d TT4: %d TT1: %d ", tt12*2, tt2*2, tt3*2, tt34*2, tt4*2, tt1*2);
            printf("TI: %d TE: %d TDI: %d TDE: %d\r\n", ti*2, te*2, tdi*2,  tde*2);
            printf("   Pmax: %d lIP: %d IP: %d Phigh: %d -- ", pmax/5, lrpi/5, pi/5, pHigh/5);
            printf("Pmin: %d lEP: %d EP: %d Plow:%d -- ", pmin/5, lrpe/5, pe/5, pLow/5 );
            printf("HiLimit: %d LoLimit: %d -- Volume: %d\r\n", hiLimit/5, loLimit/5, volume);
            printf("ADCoffset: %d ADCVoffset: %d 12V: %d mV 5V: %d mV -- ", adcOffset/8, adcVOffset/8, See12V(), See5V());
            printf("Targets: Pmax: %d IP: %d EP: %d V: %d/%d\r\n", targetPmax/5, targetHigh/5, targetLow/5, volumeMax, volumeMin);

            
    //printf("\r\nSlow: ");
    //for (int i=0; i<25; i++ ) printf(" %d", prSlowBuffer[i]);
    //printf(": %d\r\n", prSlow);
    
    //printf("\r\nFast: ");
    //for (int i=0; i<25; i++ ) printf(" %d", prFastBuffer[i]);
    //printf(": %d\r\n", prFast);
    
    //printf("\r\nTdi: ");
    //for (int i=0; i<10; i++ ) printf(" %d", tdiBuffer[i]*2);
    //printf(": %d\r\n", tdi*2);
    
    //printf("\r\nTde: ");
    //for (int i=0; i<10; i++ ) printf(" %d", tdeBuffer[i]*2);
    //printf(": %d\r\n", tde*2);

}


void SetCalibrateState(bool calib) {
    TMR0_StopTimer();
    if (calib) {
        //printf("\r\nSTART CALIBRATE\r\n");
        state = STATE_CALIBRATE;
    } else {
        //printf("\r\nSTOP CALIBRATE\r\n");
        //if (adcOffset/8>300 || adcOffset/8<-300) {
        //    printf("\r\nBAD CALIBRATION.\r\n");
        //    adcOffset = 42*8;
        //} 
        //if (adcVOffset/8>300 || adcVOffset/8<-300) {
        //    printf("\r\nBAD VOLUME CALIBRATION.\r\n");
        //    adcOffset = 228*8;
        //}
        //state = STATE_CALIBRATE;
        if (state == STATE_CALIBRATE) {
            printf("\r\nPreCalib: Pres:%d Off:%d //Volume pres:%d Off:%d\r\n", GetPressure_pa(), adcOffset/8, GetPressureV_pa(), adcVOffset/8);
            state = STATE_OFF;
            printf("\r\nCalibrated: Pres:%d Off:%d //Volume pres:%d Off:%d\r\n", GetPressure_pa(), adcOffset/8, GetPressureV_pa(), adcVOffset/8);
        }
        UnDisplayCalibrate();
    }
    ClearVars();
    TMR0_StartTimer();
}

void MonitorEnable(void) { enable = true; }
void MonitorDisable(void) { enable = false; }

void MonitorPressureTask(void) { // Every 2 ms
    int16_t pr, v12, adcv, v5;
    uint32_t sqpr;
    
    int next;
    static bool measPi=false;
    static bool measPe=false;
    
    next = state;
    
    uint16_t temp;
    
    // Define Valve state
    if (normalSV1 && (alarmSV1 || disableAlarmSV1)) VALVE_SetHigh();
    else VALVE_SetLow();
    
    // Timer0 signal every 2 seconds
    static int16_t cnt=0;
    if (cnt==0) printf("0\r\n");
    cnt = (cnt+1)%1000;
        
    if (!AdcDataReady(ADC_ID_PRS)) {
        printf(" (ErrDR) ");
        // Restart ADC
        AdcReStartCycle();
        // TBD: Add alarm
        
        return;
    } 
 
    // Check ADC status
    if (!AdcReady()) {
            printf("=");
            //AdcReStartCycle();
            //AdcInit();)
            return;
    }
     
    if (state == STATE_CALIBRATE) {
        static int cnt=0;
        adc_result_t tmp, tmp2;
        tmp = GetPressure_pa();
        adcOffset  = (7*adcOffset + 8*tmp)/8; //
        tmp2 = GetPressureV_pa();
        adcVOffset = (7*adcVOffset + 8*tmp2)/8;
        AdcReStartCycle();
        cnt++;
        if (cnt==250) { // 500 ms
            //printf("C: %d %d %d %d\r\n", tmp, adcOffset/8, tmp2, adcVOffset/8);
            cnt = 0;
            //DisplayCalibrate(tmp, adcOffset, tmp2, adcVOffset);
        }
        DisplayCalibrate(tmp, adcOffset/8, tmp2, adcVOffset/8);
        
        // Timer 0 restart conversion cycle
        
        return;
    } else {
        // Get pressure and filter 50 ms constant ( prFast) and 300 ms constant (prSlow)
        pr  = GetPressure_mbar02();
        if (pr<0) pr = 0;
        if (pr>100*5) pr = 0;
        
        v12 = See12V(); // 12V is read in Alarm
        v5 = See5V();
        // Timer 0 restart conversion cycle
        AdcReStartCycle();
    }
        
    // Return if no RUN from Controller
    if (!enable) {
        ClearVars();
        ClearVAboveMaxAlarm();
        ClearVBelowMinAlarm();
        ClearTdiTooLongAlarm();
        ClearTdeTooLongAlarm();
        ClearEPBelowSetAlarm();
        ClearEPAboveSetAlarm();
        ClearIPBelowSetAlarm();
        ClearIPAboveSetAlarm();
        ClearVAboveMaxAlarm();
        ClearVBelowMinAlarm();
        ClearCircuitFailureAlarm();
        ClearGasFailureAlarm();
        alarmSV1=true;
        enableAlarms=false;
        return;
    } else {
        static int ecnt = 0;
        ecnt++;
        if (ecnt==0) printf("e\r\n");
        ecnt= (ecnt+1)%1000;
    }
    
    // Pressure signal every 2 seconds
    static int16_t cnt2=0;
    if (cnt2==0) printf("P\r\n");
    cnt2 = (cnt2+1)%1000;
    
    // Time variables
    tt++;
    ttExt++;
    ttNoAlarm++;
    ttAlarm++;
    
    // Pression filters   
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
   
    // Alarm if no breath in 6s
    if (enableAlarms) {
        if (ttAlarm>6*500) {
            SetCircuitFailureAlarm();
        } else {
            ClearCircuitFailureAlarm();
        }
    } else {
        ttAlarm = 0;
    }
    // Only alarm after 15s after setting
    if (ttNoAlarm>15*500) {
        enableAlarms = true;
        ttNoAlarm=15*500+1;
    }
    
    // Calculate volume
    
    adcv = GetPressureV_pa();
    
    uint16_t v1 = adcv > 0 ? adcv<<4 : 0;
    uint16_t v2 = isqrt(v1); 
    volumeAcc += v2 > 20 ? v2 : 0;

    // Limits of pressure estimations
    if (ttExt>6*500) { // Every 6 s we should have at least one breath
        //printf("\r\n pLow: %d pHigh: %d Min: %d Max: %d pe: %d pi:%d \r\n", pLow/5, pHigh/5, minPressure/5, maxPressure/5, pe/5, pi/5);
        // Define Hi and Low limits for next cycle
        //hiLimit = maxPressure - (maxPressure-minPressure)/3;
        hiLimit = minPressure + 2*(maxPressure-minPressure)/5;
        loLimit = minPressure + (maxPressure-minPressure)/3;
        
        // Correct limits in rare conditions
        if (loLimit<(targetLow+5*5)) {
            //printf("\r\nCorrected loLimit = %d (%d) \r\n", targetLow/5+5, loLimit/5);
            loLimit = targetLow+5*5;
        }
            
        if (hiLimit<loLimit) hiLimit = loLimit;
        
        // Store estimation of last cycle
        pe = pLow;
        pi = pHigh; 
        pmax = maxPressure;
        pmin = minPressure;
       
        if (pmax< targetLow && enableAlarms) SetGasFailureAlarm();
        // else ClearGasFailureAlarm(); // Latched alarm. Clear with Mute
        
        // Reset maximum and minimum periodically
        //pLow = minPressure;
        //pHigh = maxPressure;
        maxPressure = 0;
        minPressure = 40*5;
        ttExt=0;
    }
    
    count ++;   
    if (count>=6*25) count =0; // 25*6 time for both filters to go around
    
    if (tt>30000) {
        //printf("\r\n No breath detected in 1 min\r\n");
        ClearVars() ; // No breaths for more than 1 min
        measPe = false;
        measPi = false;
    }
    
    // Absolute maximum for the alarm. Max is cleared every second by alarmtask
    if (prFast>alarmMaxPressure) {
        alarmMaxPressure = prFast;
        //printf("\r\n MAXP: %d\r\n", pr/5);
    }
    
    // Get absolute maximum / minimum breaths during the 6 seconds period
    if (pr>maxPressure) {
        maxPressure = pr;
        //printf("\r\n MAXP: %d\r\n", pr/5);
    }
    if (pr<minPressure) {
        minPressure = pr;
        //printf("\r\n MINP: %d\r\n", pr/5);
    }
   
    // Average signals within limits. This will average last 10*25*6 samples: 3 seconds of samples (Between 1 and 2 extra breaths)
    if (prSlow>hiLimit) {
        //TST1_SetHigh();
        //pHigh = (pHigh*39+prSlow)/40;
        pHighAcc += prSlow;
        numHigh++;
    }
    if (prSlow<loLimit) {
        //TST1_SetLow();
        //pLow = (pLow*39+prSlow)/40;    
        pLowAcc += prSlow;
        numLow++;
    }

    // Calc start of breath 
    int16_t halfLimit;
    halfLimit = (hiLimit+loLimit)/2;
    
    // Starting state
    if ( state == STATE_OFF) {
        if (pi> (targetHigh+targetLow)/2) { // Don�t do anything until estimators are reasonable
            if (prSlow < targetLow) next = STATE_LOW; else next = STATE_HIGH;
        }
    }
    
    // Use limits and Pe / PI to identify rise and falls
    
    // Start of rise
    if (prSlow<loLimit && prFast>= loLimit) { // Crossing low limit up
        //TST1_SetHigh();
        //printf("\r\nUP12: %ld pr: %d\r\n", tt, prSlow/5); 
        if (tt>400) { // Filter more events during ramp-up
            bp = tt;
            if (tt3>tt2) ti = tt3-tt2;
            
            if (tt>tt4) te = tt-tt4;
            
            if (bp!=0) bpm = 60000/(bp*2);
            if (numHigh>0) {
                pHigh = pHighAcc / numHigh;
                pHighAcc = 0;
                numHigh = 0;
            }
            if (numLow>0) {
                pLow = pLowAcc / numLow;
                numLow = 0;
                pLowAcc = 0;
            }

            tt1 = tt;  // Start of event
            
            MonitorDump();
            /*
            printf("\r\nNew Breath: TT12: %d TT2: %d TT3: %d TT34: %d TT4: %d TT1: %d ", tt12*2, tt2*2, tt3*2, tt34*2, tt4*2, tt1*2);
            printf("TI: %d TE: %d TDI: %d TDE: %d\r\n", ti*2, te*2, tdi*2,  tde*2);
            printf("   Pmax: %d lIP: %d IP: %d Phigh: %d -- ", pmax/5, lrpi/5, pi/5, pHigh/5);
            printf("Pmin: %d lEP: %d EP: %d Plow:%d -- ", pmin/5, lrpe/5, pe/5, pLow/5 );
            printf("HiLimit: %d LoLimit: %d -- Volume: %d\r\n", hiLimit/5, loLimit/5, volume);
            printf("ADCoffset: %d ADCVoffset: %d 12V: %d mV Mic: Min/Max/Freq:%d/%d/%d -- ", adcOffset/8, adcVOffset/8, v12, micMin, micMax, micFreq);
            printf("Targets: Pmax: %d IP: %d EP: %d V: %d/%d\r\n", targetPmax/5, targetHigh/5, targetLow/5, volumeMax, volumeMin);
            */
            
            tt=0; // This is the start of time
            
            tt2=0;
            tt3=0;
            tt4=0;
            tt34=0;
            if (tt1<=500) volumeAcc = v2 > 20 ? v2 : 0;;
            ttAlarm = 0;
        }
        tt12 = tt; // Last event of rising below lolimit
        // Store PE measured during TE
        measPe=false;
        measPi=false;
        lrpe = rpe;
        // Check alarms
        if (enableAlarms) {
            if (lrpe>(targetLow+15)) SetEPAboveSetAlarm(); else ClearEPAboveSetAlarm();
            if (lrpe<(targetLow-15)) SetEPBelowSetAlarm(); else ClearEPBelowSetAlarm();
        } else {
            ClearEPAboveSetAlarm();
            ClearEPBelowSetAlarm();
        }

    }
    
    // Define start of Volume capture at 90% of cycle
    if (tt1>500 && tt==(tt1*9)/10) volumeAcc=0;
    
    
    static bool down = false;
    
    // Start of fall
    if (prSlow>hiLimit && prFast<= hiLimit) { // Crossing high limit down
        //TST1_SetLow();
        if (!down) {
            //printf("\r\nDOWN34: %ld pr: %d\r\n", tt, prSlow/5, bpm); 
            tt3 = tt; // First sample going down
            down = true;
//            printf("\r\nTest D34: TT12:%d TT2:%d TT3:%d TT34:%d TT4:%d TI:%d TE:%d TDI: %d TDE:%d\r\n", tt12*2, tt2*2, tt3*2, tt34*2, tt4*2, ti*2, te*2, tdi*2, tde*2);
            
        }
        tt34 = tt; // Last sample
        // Store PI measured during TI
        measPe=false;
        measPi=false;
        lrpi = rpi;
        // Check alarms
        if (enableAlarms && !volumeControl) {
            //if (lrpi>(targetHigh+20)) SetIPAboveSetAlarm(); else ClearIPAboveSetAlarm();
            //if (lrpi<(targetHigh-20)) SetIPBelowSetAlarm(); else ClearIPBelowSetAlarm(); 
            if (pHigh>(targetHigh+20) && lrpi>(targetHigh+20) ) SetIPAboveSetAlarm();
            if (pHigh<(targetHigh+20) || lrpi<(targetHigh+20) ) ClearIPAboveSetAlarm();
            if (pHigh<(targetHigh-20) && lrpi<(targetHigh-20) ) SetIPBelowSetAlarm(); 
            if (pHigh>(targetHigh-20) || lrpi>(targetHigh-20) ) ClearIPBelowSetAlarm();
 
        } else {
            ClearIPAboveSetAlarm();
            ClearIPBelowSetAlarm();
        }
    } else if (tt>(tt3+200)) down = false;
    
    // Transition to HIGH at the top
    if (prSlow>=(pi-3*5) && state == STATE_LOW) { // State is LOW and I am at the top
        //TST2_SetHigh();
        next = STATE_HIGH;
        tt2 = tt;
//        printf("\r\nUP: %d %d\r\n", tt2, prSlow/5);
        if (tt2>0) {
            tdiBuffer[numtdi%10] = ((tt2-0));
            tdi = 0;
            for (int i=0; i<MIN(10,numtdi+1); i++) tdi = tdi + tdiBuffer[i];                   
            tdi = tdi/MIN(10,numtdi+1);
//            printf("\r\nTDI(%d): %d %d\r\n", numtdi%10, ((tt2-0)*2), tdi*2); 
            //MonitorDump();
            if (enableAlarms) numtdi++;
            if (numtdi>=100) numtdi=10;
            // Now we can start averaging Pi
            measPi=true;
            rpi=pi;
        }

//        printf("\r\nUP: %d %d\r\n", tt2, prSlow/5);
    }
    
    // Transition to LOW at the bottom
    if (prSlow<=(pe+2*5) && state == STATE_HIGH) { // State is HIGH and I am at the bottom
        //TST2_SetLow();
        next = STATE_LOW;
        tt4 = tt;
//        printf("\r\nDOWN: %d %d\r\n", tt4, prSlow/5);

        if (tt4>tt3) {
            tdeBuffer[numtde%10] = ((tt4-tt3));
            tde = 0;
            for (int i=0; i<MIN(numtde+1,10); i++) tde = tde + tdeBuffer[i];                   
            tde = tde/MIN(numtde+1,10);
//            printf("\r\nTDE: %d %d @ %lu %d %d\r\n", numtde, tde, tt, prFast, prSlow);
            //MonitorDump();
            if (enableAlarms) numtde++;
            if (numtde>=100) numtde=10;
            // Now we can start averaging Pe
            measPe = true;
            rpe = pe;
        }
        volume = (volumeAcc)/75; // Theory 60 I put 75 to increase 25%
         volumeAcc = 0;

//        printf("\r\nDOWN: %d %d\r\n", tt4, prSlow/5);
    }
    
    // New estimators: 5*25*2 = 250 ms We will average the last 250 ms
    if (measPi) rpi = (rpi*4+prSlow)/5;
    if (measPe) rpe = (rpe*4+prSlow)/5;

    // TBD: Timeout alarms of events
    if (enableAlarms) {
#if 0
        if (tt>(bp+bp/2) && bp>500) {
            if (!CircuitFailureAlarm()) printf("\r\n CIRCUIT FAILURE: No breath in %d ms\r\n", bp+bp/2);
            SetCircuitFailureAlarm();
        }
        if (tt>(tt3+tt3/2) && tt3>500 && state==STATE_HIGH) {
            if (!CircuitFailureAlarm()) printf("\r\n CIRCUIT FAILURE: No exhalation in %d ms\r\n", tt3+tt3/2);
            SetCircuitFailureAlarm();
        }
#endif
        if (tde>350) SetTdeTooLongAlarm(); else ClearTdeTooLongAlarm();
        if (tdi>350) SetTdiTooLongAlarm(); else ClearTdiTooLongAlarm();
        if (volume>volumeMax) SetVAboveMaxAlarm(); else ClearVAboveMaxAlarm();
        if (volume<volumeMin) SetVBelowMinAlarm(); else ClearVBelowMinAlarm();

    } else {
        ClearTdeTooLongAlarm();
        ClearTdiTooLongAlarm();
        ClearVAboveMaxAlarm();
        ClearVBelowMinAlarm();
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
    ttNoAlarm = 0;
    ttAlarm = 0;
    enableAlarms = false;
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
    rpi = 0;
    rpe = 0;
    lrpi = 0;
    lrpe = 0;
    maxPressure = 0;
    minPressure = 40*5;
    pHighAcc = 0;
    pLowAcc = 0;

    alarmMaxPressure = 0;
    prSlow = 0;
    prSlowDev = 0;
    prSlowNumStable = 0;
    hiLimit = 40*5;
    loLimit = 0;
    volumeAcc = 0;
    volume = 0;
    
    alarmSV1=true;
    
    ClearEPAboveSetAlarm();
    ClearEPBelowSetAlarm();
    ClearIPAboveSetAlarm();
    ClearIPBelowSetAlarm();
    ClearCircuitFailureAlarm();
}

void MonitorInit (void) {
    //ADCC_Initialize(); // Already in mcc.c
    AdcInit();
    //ADCC_EnableContinuousConversion();
    //ADCC_StartConversion(PRS);
    targetHigh = 20*5; // 0.2 mbar
    targetLow  = 7*5; // 0.2 mbar
    targetPmax = 25*5;
    adcVOffset = 162*8; // Read from EEPROM or calibrate. ADC counts
    adcOffset = 174*8;
    volumeMax = 500;
    volumeMin = 100;
    disableAlarmSV1=false;
    state = STATE_OFF;
    enable = false;
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
    adc_result_t adc = AdcGetData(ADC_ID_PRS);
    if (state == STATE_CALIBRATE) {
        return adc;
    }
    //printf("ADC: %d ", adc);
    int32_t mv = adc - adcOffset/8;
    mv = ( mv * 5000 )/ 4096;  // Move from 12 bits to 5V range
    //printf("V: %d mV ", mv);
    int32_t p32 = (mv*100)/45;
    int16_t p = ((int16_t) p32); //  - (state == STATE_CALIBRATE ? 0 : adcOffset/8); 
    if (p<0) p=0;
    return p;
}

int16_t SeePressure_pa (void) {
    adc_result_t adc = AdcSeeData(ADC_ID_PRS);
    int32_t mv = adc - adcOffset/8;
    mv = ( mv * 5000 )/ 4096;
    int32_t p32 = (mv*100)/45;
    int16_t p = ((int16_t) p32);
    if (p<0) p=0;
    return p;
}

int16_t GetPressureV_pa (void) {
    
    adc_result_t adcv = AdcGetData(ADC_ID_VOL);
    if (state == STATE_CALIBRATE) {
        return adcv;
    }    
    //printf("ADC: %d ", adc);
    int32_t mv = adcv - adcVOffset/8;
    mv = ( mv * 5000 )/ 4096;  // Move from 12 bits to 5V range
    //printf("V: %d mV ", mv);
    int16_t p = mv;
    //p = p - 1000  - (state == STATE_CALIBRATE ? 0 : adcVOffset/8); // remove offset of 1V
    if (p<0) p=0;
    return p;
}

int16_t SeePressureV_pa (void) {
    adc_result_t adcv = AdcSeeData(ADC_ID_VOL);
    int32_t mv = adcv - adcVOffset/8;
    mv = ( mv * 5000 )/ 4096;
    int16_t p = mv;
    return p;
}

int16_t See5V (void) {
    adc_result_t adc = AdcSeeData(ADC_ID_5V);
    int32_t mv = adc;
    mv = ( mv*4096) / 4096;
    int16_t v12 = mv/2; // Resistor network
    return v12;
}

int16_t Get5V (void) {
    adc_result_t adc = AdcGetData(ADC_ID_5V);
    int32_t mv = adc;
    mv = ( mv*4096) / 4096;
    int16_t v12 = mv/2;
    return v12;
}

int16_t See12V (void) {
    adc_result_t adc = AdcSeeData(ADC_ID_12V);
    int32_t mv = adc;
    mv = ( mv*4096) / 4096;
    int16_t v12 = (mv*33)/10; // Resistor network
    return v12;
}

int16_t Get12V (void) {
    adc_result_t adc = AdcGetData(ADC_ID_12V);
    int32_t mv = adc;
    mv = ( mv*4096) / 4096;
    int16_t v12 = (mv*33)/10;
    return v12;
}

void DisableAlarmSV1(void) {
    disableAlarmSV1=true;
}

void EnableAlarmSV1(void) {
    disableAlarmSV1=false;
}

bool GetDisableAlarmSV1(void) { return disableAlarmSV1; }

void SetAlarmSV1(bool value) {
    static bool last= false;
    if (last!=value) printf("\r\nAlSV1: %d\r\n", value);
    alarmSV1 = value;
    last = value;
}

void SetSV1(bool value) {
    static bool last= false;
    if (last!=value) printf("\r\nSV1: %d\r\n", value);
    last = value;
    normalSV1 = value;
}

bool GetAlarmSV1(void) { return alarmSV1; }

