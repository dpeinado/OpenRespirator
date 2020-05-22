#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "alarm.h"
#include "monitor.h"
#include "display.h"
#include "buzzer.h"
#include "buttons.h"

// Alarm state
static bool batteryFailAlarm = false;
static bool monitorFailAlarm = false;
static bool controlFailAlarm = false;
static bool gasFailureAlarm = false;
static bool highPressureAlarmLow = false;
static bool highPressureAlarmHigh = false;
static bool veryHighPressureAlarmHigh = false;
static bool circuitFailureAlarm = false;
static bool baterryLowAlarmMed = false;
static bool baterryLowAlarmHigh = false;
static int epBelowSetAlarm = 0;
static int epAboveSetAlarm = 0;
static int ipBelowSetAlarm = 0;
static int ipAboveSetAlarm = 0;
static bool tdiTooLongAlarm = false;
static bool tdeTooLongAlarm = false;
static bool vBelowMinAlarm = false;
static bool vAboveMaxAlarm = false;
static bool noPowerSupplyAlarm = false;
static int16_t alarmPmax = 40;

static bool veryHighSV1 = true;
static bool batterySV1  = true;
static bool controlSV1  = true;
static bool monitorSV1  = true;

// Alarm check functions. Called every second
bool BatteryFailAlarm(void) {
    static int timeBattery = 0;
    static int timeBelow10 = 0;
    static int timeFail = 0;
    int16_t bat;
    // Check Battery status  
    if (AdcDataReady(ADC_ID_12V)) {
        bat= Get12V();
        
        if (bat < 11300) { // BATTERY FAIL detection ( should be more than 10 min)
            timeFail++;
            if (timeFail>10*60) { // 10 min
                SetBatteryFailAlarm ();
                batterySV1=false;           
            }
        } else {
            timeFail = 0;
            batterySV1=true;
            ClearBatteryFailAlarm ();
        }
        
        if (bat < 11500) { // Bellow 10% 11
            timeBelow10++;
            SetBaterryLowAlarmMed();
            if (timeBelow10>3*60) { // 3min
                SetBaterryLowAlarmHigh();
                ClearBaterryLowAlarmHigh();
            }
        } else {
            timeBelow10 = 0;
            ClearBaterryLowAlarmMed();
            ClearBaterryLowAlarmHigh();
        }
        
        if (bat < 13000) { // Battery operated
            timeBattery++;
            SetNoPowerSupplyAlarm();
        }
        else
        {
            timeBattery = 0;
            ClearNoPowerSupplyAlarm();
        }
        return batteryFailAlarm;
    }

    
    return batteryFailAlarm;
}
bool MonitorFailAlarm(void) { monitorSV1 = true; return monitorFailAlarm; };
bool ControlFailAlarm(void) {
    if (controlFailAlarm) {
        controlSV1=false;
    }  
    return controlFailAlarm;
};
bool GasFailureAlarm(void) { return gasFailureAlarm; };
void SetMaxPressure(int16_t pr) { alarmPmax = pr; }

bool HighPressureAlarmLow(void) {
    static int count = 0;
    static int countVery = 0;
    int16_t max = GetMaxPressure();
    if (max>alarmPmax) {
        highPressureAlarmLow = true;
        count ++;
    } else
    {
        highPressureAlarmLow= false;     
        count = 0;
    }
    if (count >= 3) {
        SetHighPressureAlarmHigh();
        count = 3;
    }
    //else ClearHighPressureAlarmHigh(); // MUTE to clear
    
    if (max>70){
        SetVeryHighPressureAlarmHigh();
        veryHighSV1 = false;
    } 
    
    //else ClearVeryHighPressureAlarmHigh(); // MUTE to clear
    return highPressureAlarmLow;
};
bool HighPressureAlarmHigh(void) { 

    return highPressureAlarmHigh;
};
bool VeryHighPressureAlarmHigh(void) { 
    return veryHighPressureAlarmHigh;
};
bool CircuitFailureAlarm(void) {
    return circuitFailureAlarm;
};
bool BaterryLowAlarmMed(void) { return baterryLowAlarmMed; };
bool BaterryLowAlarmHigh(void) { return baterryLowAlarmHigh; };
bool EPBelowSetAlarm(void) {
    if (epBelowSetAlarm> 3) return true;
    else return false;
};
bool EPAboveSetAlarm(void) {
    if (epAboveSetAlarm> 3) return true;
    else return false;
};
bool IPBelowSetAlarm(void) {
    if (ipBelowSetAlarm> 3) { /*printf("IPB %d\r\n", ipBelowSetAlarm); */return true; }
    else return false;
};
bool IPAboveSetAlarm(void) {
    if (ipAboveSetAlarm> 3) { /*printf("IPA %d\r\n", ipAboveSetAlarm);*/ return true; }
    else return false;
};
bool TdiTooLongAlarm(void) { 
    //if (GetTdi() > 700) tdiTooLongAlarm= true;
    //else tdiTooLongAlarm = false;
    return tdiTooLongAlarm;
};

bool TdeTooLongAlarm(void) {
    //if (GetTde() > 700) tdiTooLongAlarm= true;
    //else tdeTooLongAlarm = false;
    return tdeTooLongAlarm;
};

bool VAboveMaxAlarm(void) {
    return vAboveMaxAlarm;
};

bool VBelowMinAlarm(void) {
    return vBelowMinAlarm;
};

bool NoPowerSupplyAlarm(void) { return noPowerSupplyAlarm; };

// External function to set or clear alarms

void SetBatteryFailAlarm(void) { batteryFailAlarm = true; };
void SetMonitorFailAlarm(void) { monitorFailAlarm = true; };
void SetControlFailAlarm(void) { controlFailAlarm = true; };
void SetGasFailureAlarm(void) { gasFailureAlarm = true; };
void SetHighPressureAlarmLow(void) { highPressureAlarmLow = true; };
void SetHighPressureAlarmHigh(void) { highPressureAlarmHigh = true; };
void SetVeryHighPressureAlarmHigh(void) { veryHighPressureAlarmHigh = true; };
void SetCircuitFailureAlarm(void) { circuitFailureAlarm = true; };
void SetBaterryLowAlarmMed(void) { baterryLowAlarmMed = true; };
void SetBaterryLowAlarmHigh(void) { baterryLowAlarmHigh = true; };
void SetEPBelowSetAlarm(void) { epBelowSetAlarm++;  if (epBelowSetAlarm>6) epBelowSetAlarm=6; };
void SetEPAboveSetAlarm(void) { epAboveSetAlarm++;   if (epAboveSetAlarm>6) epAboveSetAlarm=6; };
void SetIPBelowSetAlarm(void) { ipBelowSetAlarm++;   if (ipBelowSetAlarm>6) ipBelowSetAlarm=6; };
void SetIPAboveSetAlarm(void) { ipAboveSetAlarm++;   if (ipAboveSetAlarm>6) ipAboveSetAlarm=6; };
void SetTdiTooLongAlarm(void) { tdiTooLongAlarm = true; };
void SetTdeTooLongAlarm(void) { tdeTooLongAlarm = true; };
void SetVAboveMaxAlarm(void) { vAboveMaxAlarm = true; };
void SetVBelowMinAlarm(void) { vBelowMinAlarm = true; };
void SetNoPowerSupplyAlarm(void) { noPowerSupplyAlarm = true; };

void ClearAllAlarm(void) {
    ClearBatteryFailAlarm();
    ClearMonitorFailAlarm();
    ClearControlFailAlarm();
    ClearGasFailureAlarm();
    ClearHighPressureAlarmLow();
    ClearHighPressureAlarmHigh();
    ClearVeryHighPressureAlarmHigh();
    ClearCircuitFailureAlarm();
    ClearBaterryLowAlarmMed();
    ClearBaterryLowAlarmHigh();
    ClearEPBelowSetAlarm();
    ClearEPAboveSetAlarm();
    ClearIPBelowSetAlarm();
    ClearIPAboveSetAlarm();
    ClearTdiTooLongAlarm();
    ClearTdeTooLongAlarm();
    ClearVAboveMaxAlarm();
    ClearVBelowMinAlarm();
    ClearNoPowerSupplyAlarm();
}

void ClearBatteryFailAlarm(void) { batteryFailAlarm = false; };
void ClearMonitorFailAlarm(void) { monitorFailAlarm = false; };
void ClearControlFailAlarm(void) { controlFailAlarm = false; };
void ClearGasFailureAlarm(void) { gasFailureAlarm = false; };
void ClearHighPressureAlarmLow(void) { highPressureAlarmLow = false; };
void ClearHighPressureAlarmHigh(void) { highPressureAlarmHigh = false; };
void ClearVeryHighPressureAlarmHigh(void) { veryHighPressureAlarmHigh = false; };
void ClearCircuitFailureAlarm(void) { circuitFailureAlarm = false; };
void ClearBaterryLowAlarmMed(void) { baterryLowAlarmMed = false; };
void ClearBaterryLowAlarmHigh(void) { baterryLowAlarmHigh = false; };
void ClearEPBelowSetAlarm(void) { epBelowSetAlarm--; if (epBelowSetAlarm<0) epBelowSetAlarm=0; };
void ClearEPAboveSetAlarm(void) { epAboveSetAlarm--; if (epAboveSetAlarm<0) epAboveSetAlarm=0; };
void ClearIPBelowSetAlarm(void) { ipBelowSetAlarm--; if (ipBelowSetAlarm<0) ipBelowSetAlarm=0; };
void ClearIPAboveSetAlarm(void) { ipAboveSetAlarm--; if (ipAboveSetAlarm<0) ipAboveSetAlarm=0; };
void ClearTdiTooLongAlarm(void) { tdiTooLongAlarm = false; };
void ClearTdeTooLongAlarm(void) { tdeTooLongAlarm = false; };
void ClearVAboveMaxAlarm(void) { vAboveMaxAlarm = false; };
void ClearVBelowMinAlarm(void) { vBelowMinAlarm = false; };
void ClearNoPowerSupplyAlarm(void) { noPowerSupplyAlarm = false; };

struct alarm { char name[17]; int type; bool (* func)(void); } a;

struct alarm alarmData[] = {
    {"Battery Fail  ", ALARM_HIGH, BatteryFailAlarm },
    {"Monitor Fail  ", ALARM_HIGH, MonitorFailAlarm },
    {"Control Fail  ", ALARM_HIGH, ControlFailAlarm },
//    1234567890123456
    {"Very High Pres", ALARM_HIGH, VeryHighPressureAlarmHigh},
    {"High Pressure ", ALARM_HIGH, HighPressureAlarmHigh},
    {"High Pressure ", ALARM_LOW,  HighPressureAlarmLow},   
    {"Circuit Fail  ", ALARM_HIGH, CircuitFailureAlarm},
    {"Gas Failure   ", ALARM_HIGH, GasFailureAlarm},
    {"Battery Low   ", ALARM_MED,  BaterryLowAlarmMed},
    {"Battery Low   ", ALARM_HIGH, BaterryLowAlarmHigh},
    {"EP Below Set  ", ALARM_MED,  EPBelowSetAlarm},
    {"EP Above Set  ", ALARM_MED,  EPAboveSetAlarm},
    {"IP Below Set  ", ALARM_MED,  IPBelowSetAlarm},
    {"IP Above Set  ", ALARM_MED,  IPAboveSetAlarm},
    {"Tdi too long  ", ALARM_MED,  TdiTooLongAlarm},
    {"Tde too long  ", ALARM_MED,  TdeTooLongAlarm},
    {"Over Max Vol  ", ALARM_MED,  VAboveMaxAlarm},
    {"Below Min Vol ", ALARM_MED,  VBelowMinAlarm},
    {"No Power Suppl", ALARM_LOW,  NoPowerSupplyAlarm}
};

#define NUM_ALARMS sizeof(alarmData)/sizeof(a)

// testAlarm
static int testAlarm[NUM_ALARMS];

// Alarms at the same time
static int alarms[NUM_ALARMS];

// Seconds muted
static int muteSec = 0;

// Seconds Hist
static int histSec = 0;

// Hist num
static int hist = 0;

static int displayStatus;
static bool alarmCheck = false;

#define DISPLAY_NORMAL 0
#define DISPLAY_ALARM  1
#define DISPLAY_HIST   2

void TestAlarm(int id) {
    testAlarm[id] = !testAlarm[id];
    printf("\r\nTest Alarm: ");
    for (int i=0; i<NUM_ALARMS; i++) 
    {
        if (testAlarm[i]) putchar('0'+i); else putchar('.');
    }
    printf("\r\n");
}

void AlarmSet(uint8_t id) {
    alarms[id] = 1;
}

void AlarmClear(uint8_t id){
    alarms[id] = 0;
}

int HigherAlarm(void) {
    int max=-1;
    for (int i=0; i<NUM_ALARMS; i++) {
        if (alarms[i]) {
            if (max<0) max = i;
            else if (alarmData[i].type>alarmData[max].type) max = i;
        }
    }
    return max;
}
 
bool AnyAlarm(void) {
    for (int i=0; i<NUM_ALARMS; i++) {
        if (alarms[i]) return true;
    }
    return false;
}

void MuteAlarm(void) {
    printf("MUTE");
    
    if (muteSec && muteSec<115) { // Re-enable sound
        muteSec = 0;
    }

    if (AnyAlarm()) { // If any alarm
        // Start Mute
        BuzzerClear();
        muteSec = 120;
        // Clear latched alarms
        int high = HigherAlarm();
        if (alarmData[high].func==HighPressureAlarmHigh) {
//            printf("\r\nClear High Alarm\r\n");
            ClearHighPressureAlarmHigh();
        }
        if (alarmData[high].func==VeryHighPressureAlarmHigh) {
//            printf("\r\nClear Very High Alarm\r\n");
            ClearVeryHighPressureAlarmHigh();
        }
        if (alarmData[high].func==MonitorFailAlarm) {
//            printf("\r\nClear Monitor Alarm\r\n");
            ClearMonitorFailAlarm();
        }
        if (alarmData[high].func==ControlFailAlarm) {
//            printf("\r\nClear Control Alarm\r\n");
            ClearControlFailAlarm();
        }
        if (alarmData[high].func==GasFailureAlarm) {
//            printf("\r\nClear Gas Failure Alarm\r\n");
            ClearGasFailureAlarm();
        }
        if (alarmData[high].func==CircuitFailureAlarm) {
//            printf("\r\nClear Circuit Failure Alarm\r\n");
            ClearCircuitFailureAlarm();
        }
    }
    
    // This is here just in case .. To be debug
    ClearVeryHighPressureAlarmHigh();
    ClearHighPressureAlarmHigh();
    ClearCircuitFailureAlarm();
    ClearGasFailureAlarm();
    ClearControlFailAlarm();
    ClearMonitorFailAlarm();
    
    controlSV1 = true;
    monitorSV1 = true;
    veryHighSV1 = true;
    
    SetAlarmSV1(controlSV1 && monitorSV1 && veryHighSV1 && batterySV1);
}

char *GetAlarmState(void) {
    if (displayStatus==DISPLAY_HIST) return "Hi";
    else {
        if (AnyAlarm() && muteSec) return "Mu";
        else if (AnyAlarm()) return "Al";
    }
    return "  ";
}

void AlarmUpdateLCD(void) {
    
    if (histSec) {
        displayStatus = DISPLAY_HIST;
        AlarmDisplay(alarmData[hist].type, alarmData[hist].name);
        SetAlarmLED();
    } else {
        if (displayStatus==DISPLAY_ALARM && AnyAlarm())
        {
            int id;
            SetAlarmLED();
            id = HigherAlarm();
            //printf("\r\nAlarm: %d %s\r\n", id, alarmData[id].name);
            AlarmDisplay(alarmData[id].type,alarmData[id].name);
            displayStatus = DISPLAY_NORMAL;           
        } else {
            // Display Parameters TBD
            // MonitorUpdateLCD();
            ValueDisplay();
            ClearAlarmLED();
            if (AnyAlarm()) displayStatus = DISPLAY_ALARM;
        }
    }
}

void HistAlarm(void) {
    TMR5_StopTimer();
    int i;
    // First Push
    if (histSec == 0 ) {
        // Find first alarm
        for (i=0; i<NUM_ALARMS; i++) {
            if (alarms[i]) break;
        }
        // No alarm
        if (i == NUM_ALARMS) {
            TMR5_StartTimer();
            return;
        }
        histSec = 5;
        hist=i;
    } else {
        // Following push
        // Find next
        for (i=hist+1; i<NUM_ALARMS; i++) {
            if (alarms[i]) break;
        }
        // If no more, start again
        if (i>=NUM_ALARMS) {
            // Find first alarm
            for (i=0; i<NUM_ALARMS; i++) {
                if (alarms[i]) break;
            }
            // No alarm
            if (i >= NUM_ALARMS) {
                histSec = 0;
                TMR5_StartTimer();
                return;
            }
        }
        // Else assign alarm to hist
        histSec = 5;
        hist = i;
    }
    // Display Alarm in LCD
    AlarmUpdateLCD();
    TMR5_StartTimer();
}

void AlarmCheckTask(void) {
    SetAlarmSV1(controlSV1 && monitorSV1 && veryHighSV1 && batterySV1);
    if (alarmCheck == false) return;
    alarmCheck = false;
    // Current buzzer state
    static int current = 0;
    
    // Check each alarm and set or clear it needed
    for (int id=0; id<NUM_ALARMS; id++) {
        if (alarmData[id].func() || testAlarm[id]) {
            AlarmSet(id);
        } else {
            AlarmClear(id);
        }
    }
    // Update LCD
    AlarmUpdateLCD();
    
    // Update buzzer state
    // I am mute ?
    if (muteSec == 0)
    {
        // There is an alarm
        if (AnyAlarm()) {
            // The type of the alarm
            int type = alarmData[HigherAlarm()].type;
            // The type of the higher alarm is the same than the one playing ?
            if (type!=current) {
                // Update alarm
                current = type;
                BuzzerSet(type);
            } 
        } else {
            // No alarm
            current = 0;
            BuzzerClear();
        }
    } else {
        // I am mute
        current = 0;
        BuzzerClear();
    }
}



void AlarmHandler(void) {

    //printf("AH %d %d\r\n", muteSec, histSec);
    if (muteSec && AnyAlarm()) muteSec--;
    else muteSec = 0;
    if (histSec) histSec--;
    //AlarmCheckTask(); // Task is done at app level, not at IRQ
    alarmCheck = true; // Tell main loop to do checkTask
    
    static int16_t cnt = 0;
    if (cnt==0) printf("A\r\n");
    cnt = (cnt+1)%2;
}

void AlarmInit() {
    BuzzerInit();
    for (int i=0; i<NUM_ALARMS; i++) {
        alarms[i] = 0;
        testAlarm[i] =0;
    }
    displayStatus = DISPLAY_NORMAL;
    alarmPmax = 40;
    
    TMR5_SetInterruptHandler(AlarmHandler);
    TMR5_StartTimer();
}

