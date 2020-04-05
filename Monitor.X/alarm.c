#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "alarm.h"
#include "display.h"
#include "buzzer.h"

#define NUM_ALARMS 10

bool BatteryFailAlarm(void);
bool MonitorFailAlarm(void);
bool ControlFailAlarm(void);
bool GasFailureAlarm(void);
bool HighPressureAlarmLow(void);
bool HighPressureAlarmHigh(void);
bool CircuitFailureAlarm(void);
bool BaterryLowAlarmMed(void);
bool BaterryLowAlarmHigh(void);
bool EPAboveSetAlarm(void);
bool IPBellowSetAlarm(void);
bool IPAboveSetAlarm(void);
bool TdiTooLongAlarm(void);
bool TdeTooLongAlarm(void);
bool NoPowerSupplyAlarm(void);

struct alarm { char name[17]; int type; bool (* func)(void); } a;

struct alarm alarmData[] = {
    {"Battery Fail    ", ALARM_HIGH, BatteryFailAlarm },
    {"Monitor Fail    ", ALARM_HIGH, MonitorFailAlarm },
    {"Control Fail    ", ALARM_HIGH, ControlFailAlarm },
    {"Gas Failure     ", ALARM_HIGH, GasFailureAlarm},
    {"High Pressure   ", ALARM_LOW,  HighPressureAlarmLow},
    {"High Pressure   ", ALARM_HIGH, HighPressureAlarmHigh},
    {"Circuit Failure ", ALARM_HIGH, CircuitFailureAlarm},
    {"Battery Low     ", ALARM_MED,  BaterryLowAlarmMed},
    {"Battery Low     ", ALARM_HIGH, BaterryLowAlarmHigh},
    {"EP Above Set    ", ALARM_MED,  EPAboveSetAlarm},
    {"IP Bellow Set   ", ALARM_MED,  IPBellowSetAlarm},
    {"IP Above Set    ", ALARM_MED,  IPAboveSetAlarm},
    {"Tdi too long    ", ALARM_MED,  TdiTooLongAlarm},
    {"Tde too long    ", ALARM_MED,  TdeTooLongAlarm},
    {"No Power Supply ", ALARM_LOW,  NoPowerSupplyAlarm}
};

// Alarms at the same time
int alarms[NUM_ALARMS];

// Seconds muted
int muteSec = 0;

// Seconds Hist
int histSec = 0;

// Hist num
int hist = 0;

// TBD Timer
void MuteAlarm(void) {
    BuzzerClear();
    muteSec = 120;
}

#define DISPLAY_NORMAL 0
#define DISPLAY_ALARM  1
#define DISPLAY_HIST   2


int HigherAlarm(void) {
    int max=0;
    for (int i=0; i<NUM_ALARMS; i++) {
        if (alarms[i]!=-1 && alarmData[alarms[i]].type>alarmData[alarms[max]].type) max = i;
    }
    return max;
}

bool AnyAlarm(void) {
    for (int i=0; i<NUM_ALARMS; i++) {
        if (alarms[i]!=-1) return true;
    }
    return false;
}

void AlarmUpdateLCD(void) {
    static int displayStatus = DISPLAY_NORMAL;
    
    if (histSec) {
        displayStatus = DISPLAY_HIST;
        AlarmDisplay(hist, alarmData[alarms[hist]].name);
    } else {
        if (displayStatus==DISPLAY_ALARM)
        {
            int id;
            id = HigherAlarm();
            AlarmDisplay(id,alarmData[alarms[id]].name);
            displayStatus = DISPLAY_NORMAL;
        } else {
            // Display Parameters TBD
            // MonitorUpdateLCD();
            ValueDisplay(0,0.0,0.0);
            if (AnyAlarm()) displayStatus = DISPLAY_ALARM;
        }
    }
}

void HistAlarm(void) {
    int i;
    if (histSec ==0 ) {
    for (i=0; i<NUM_ALARMS; i++) {
        if (alarms[i]!=-1) break;
    }
    if (i == NUM_ALARMS) return;
    histSec=5;
    hist=0;
    } else {
        histSec=5; 
        hist++;
        if (hist==NUM_ALARMS) hist=0;
        for (i=hist; i<NUM_ALARMS; i++) {
            if (alarms[i]!=-1) break;
        }
        if (i == NUM_ALARMS) hist--; else hist = i;
    }
    AlarmUpdateLCD();
}

void AlarmCheckTask(void) {
    // Check each alarm and set or clear it needed
    for (int id=0; id<sizeof(alarmData)/sizeof(a); id++) {
        if (alarmData[id].func()) {
            AlarmSet(id);
        } else {
            AlarmClear(id);
        }
    }
    // Update LCD
    AlarmUpdateLCD();
    // Update Buzzer
    if (muteSec==0) BuzzerSet(alarmData[alarms[HigherAlarm()]].type);
}


void AlarmSet(uint8_t id) {
    for (int i=0; i<NUM_ALARMS; i++) {
        if (alarms[i]==-1) {
            alarms[i] = id;
            break;
        }
    }
}

void AlarmClear(uint8_t id){
    for (int i=0; i<NUM_ALARMS; i++) {
        if (alarms[i]==id) {
            alarms[i] = -1;
            break;
        }
    }
}

void AlarmHandler(void) {
    if (muteSec) muteSec--;
    if (histSec) histSec--;
    AlarmCheckTask();    
}

void AlarmInit() {
    BuzzerInit();
    for (int i=0; i<NUM_ALARMS; i++) alarms[i] = -1;
    TMR5_StartTimer();
    TMR5_SetInterruptHandler(AlarmHandler);
}

bool BatteryFailAlarm(void) {
    return false;
}
bool MonitorFailAlarm(void) {
    return false;
}
bool ControlFailAlarm(void) {
    return false;
}
bool GasFailureAlarm(void) {
    return false;
}
bool HighPressureAlarmLow(void) {
    return false;
}
bool HighPressureAlarmHigh(void) {
    return false;
}
bool CircuitFailureAlarm(void) {
    return false;
}
bool BaterryLowAlarmMed(void) {
    return false;
}
bool BaterryLowAlarmHigh(void) {
    return false;
}
bool EPAboveSetAlarm(void) {
    return false;
}
bool IPBellowSetAlarm(void) {
    return false;
}
bool IPAboveSetAlarm(void) {
    return false;
}
bool TdiTooLongAlarm(void) {
    return false;
}
bool TdeTooLongAlarm(void) {
    return false;
}
bool NoPowerSupplyAlarm(void) {
    return false;
}
