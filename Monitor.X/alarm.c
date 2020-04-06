#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "alarm.h"
#include "display.h"
#include "buzzer.h"
#include "buttons.h"

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

#define NUM_ALARMS sizeof(alarmData)/sizeof(a)

// testAlarm
int testAlarm[NUM_ALARMS];

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

void AlarmUpdateLCD(void) {
    static int displayStatus = DISPLAY_NORMAL;
    
    if (histSec) {
        displayStatus = DISPLAY_HIST;
        AlarmDisplay(alarmData[hist].type, alarmData[hist].name);
        SetAlarmLED();
    } else {
        if (displayStatus==DISPLAY_ALARM && AnyAlarm())
        {
            int id;
            id = HigherAlarm();
            AlarmDisplay(alarmData[id].type,alarmData[id].name);
            SetAlarmLED();
            displayStatus = DISPLAY_NORMAL;           
        } else {
            // Display Parameters TBD
            // MonitorUpdateLCD();
            ValueDisplay(0,0.0,0.0);
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
    
    if (AnyAlarm()) {
        int type = alarmData[HigherAlarm()].type;
        if (type!=current) {
            current = type; 
            // Update Buzzer
            if (muteSec==0) BuzzerSet(type);
        }
    } else {
        current = 0;
        BuzzerClear();
    }
}



void AlarmHandler(void) {
    //printf("AH\r\n");
    if (muteSec) muteSec--;
    if (histSec) histSec--;
    AlarmCheckTask();    
}

void AlarmInit() {
    BuzzerInit();
    for (int i=0; i<NUM_ALARMS; i++) {
        alarms[i] = 0;
        testAlarm[i] =0;
    }
    
    TMR5_SetInterruptHandler(AlarmHandler);
    TMR5_StartTimer();
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
