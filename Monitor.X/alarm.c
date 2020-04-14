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
bool batteryFailAlarm = false;
bool monitorFailAlarm = false;
bool controlFailAlarm = false;
bool gasFailureAlarm = false;
bool highPressureAlarmLow = false;
bool highPressureAlarmHigh = false;
bool circuitFailureAlarm = false;
bool baterryLowAlarmMed = false;
bool baterryLowAlarmHigh = false;
int epBellowSetAlarm = 0;
int epAboveSetAlarm = 0;
int ipBellowSetAlarm = 0;
int ipAboveSetAlarm = 0;
bool tdiTooLongAlarm = false;
bool tdeTooLongAlarm = false;
bool noPowerSupplyAlarm = false;

// Alarm check functions. Called every second
bool BatteryFailAlarm(void) { return batteryFailAlarm; }
bool MonitorFailAlarm(void) { return monitorFailAlarm; };
bool ControlFailAlarm(void) { return controlFailAlarm; };
bool GasFailureAlarm(void) { return gasFailureAlarm; };
bool HighPressureAlarmLow(void) {
    static int count = 0;
    if (GetMaxPressure()>38) {
        highPressureAlarmLow = true;
        count ++;
    } else
    {
        highPressureAlarmLow= false;     
        count = 0;
    }
    if (count >= 3) SetHighPressureAlarmHigh();
    return highPressureAlarmLow;
};
bool HighPressureAlarmHigh(void) { 
    if (highPressureAlarmHigh) {
        SetSV1(false);
    } else {
        SetSV1(true);
    }
    return highPressureAlarmHigh;
};
bool CircuitFailureAlarm(void) { return circuitFailureAlarm; };
bool BaterryLowAlarmMed(void) { return baterryLowAlarmMed; };
bool BaterryLowAlarmHigh(void) { return baterryLowAlarmHigh; };
bool EPBellowSetAlarm(void) {
    if (epBellowSetAlarm> 3) return true;
    else return false;
};
bool EPAboveSetAlarm(void) { return epAboveSetAlarm; };
bool IPBellowSetAlarm(void) { return ipBellowSetAlarm; };
bool IPAboveSetAlarm(void) { return ipAboveSetAlarm; };
bool TdiTooLongAlarm(void) { 
    if (GetTdi() > 700) tdiTooLongAlarm= true;
    else tdiTooLongAlarm = false;
    return tdiTooLongAlarm;
};

bool TdeTooLongAlarm(void) {
    if (GetTde() > 700) tdiTooLongAlarm= true;
    else tdeTooLongAlarm = false;
    return tdeTooLongAlarm;
};

bool NoPowerSupplyAlarm(void) { return noPowerSupplyAlarm; };

// External function to set or clear alarms

void SetBatteryFailAlarm(void) { batteryFailAlarm = true; };
void SetMonitorFailAlarm(void) { monitorFailAlarm = true; };
void SetControlFailAlarm(void) { controlFailAlarm = true; };
void SetGasFailureAlarm(void) { gasFailureAlarm = true; };
void SetHighPressureAlarmLow(void) { highPressureAlarmLow = true; };
void SetHighPressureAlarmHigh(void) { highPressureAlarmHigh = true; };
void SetCircuitFailureAlarm(void) { circuitFailureAlarm = true; };
void SetBaterryLowAlarmMed(void) { baterryLowAlarmMed = true; };
void SetBaterryLowAlarmHigh(void) { baterryLowAlarmHigh = true; };
void SetEPBellowSetAlarm(void) { epBellowSetAlarm++;  if (epBellowSetAlarm>6) epBellowSetAlarm=6; };
void SetEPAboveSetAlarm(void) { epAboveSetAlarm++;   if (epAboveSetAlarm>6) epAboveSetAlarm=6; };
void SetIPBellowSetAlarm(void) { ipBellowSetAlarm++;   if (ipBellowSetAlarm>6) ipBellowSetAlarm=6; };
void SetIPAboveSetAlarm(void) { ipAboveSetAlarm++;   if (ipAboveSetAlarm>6) ipAboveSetAlarm=6; };
void SetTdiTooLongAlarm(void) { tdiTooLongAlarm = true; };
void SetTdeTooLongAlarm(void) { tdeTooLongAlarm = true; };
void SetNoPowerSupplyAlarm(void) { noPowerSupplyAlarm = true; };

void ClearBatteryFailAlarm(void) { batteryFailAlarm = false; };
void ClearMonitorFailAlarm(void) { monitorFailAlarm = false; };
void ClearControlFailAlarm(void) { controlFailAlarm = false; };
void ClearGasFailureAlarm(void) { gasFailureAlarm = false; };
void ClearHighPressureAlarmLow(void) { highPressureAlarmLow = false; };
void ClearHighPressureAlarmHigh(void) { highPressureAlarmHigh = false; };
void ClearCircuitFailureAlarm(void) { circuitFailureAlarm = false; };
void ClearBaterryLowAlarmMed(void) { baterryLowAlarmMed = false; };
void ClearBaterryLowAlarmHigh(void) { baterryLowAlarmHigh = false; };
void ClearEPBellowSetAlarm(void) { epBellowSetAlarm--; if (epBellowSetAlarm<0) epBellowSetAlarm=0; };
void ClearEPAboveSetAlarm(void) { epAboveSetAlarm--; if (epAboveSetAlarm<0) epAboveSetAlarm=0; };
void ClearIPBellowSetAlarm(void) { ipBellowSetAlarm--; if (ipBellowSetAlarm<0) ipBellowSetAlarm=0; };
void ClearIPAboveSetAlarm(void) { ipAboveSetAlarm--; if (ipAboveSetAlarm<0) ipAboveSetAlarm=0; };
void ClearTdiTooLongAlarm(void) { tdiTooLongAlarm = false; };
void ClearTdeTooLongAlarm(void) { tdeTooLongAlarm = false; };
void ClearNoPowerSupplyAlarm(void) { noPowerSupplyAlarm = false; };



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
    {"EP Bellow Set   ", ALARM_MED,  EPBellowSetAlarm},
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

int displayStatus;


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
    if (muteSec && muteSec<115) {
        muteSec = 0;
        return;
    }
    if (AnyAlarm()) {
        BuzzerClear();
        muteSec = 120;
        if (alarmData[HigherAlarm()].func==HighPressureAlarmHigh) {
            ClearHighPressureAlarmHigh();
        }
    }
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
            AlarmDisplay(alarmData[id].type,alarmData[id].name);
            SetAlarmLED();
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
    printf("AH %d %d\r\n", muteSec, histSec);
    if (muteSec && AnyAlarm()) muteSec--;
    else muteSec = 0;
    if (histSec) histSec--;
    AlarmCheckTask();    
}

void AlarmInit() {
    BuzzerInit();
    for (int i=0; i<NUM_ALARMS; i++) {
        alarms[i] = 0;
        testAlarm[i] =0;
    }
    displayStatus = DISPLAY_NORMAL;
    
    TMR5_SetInterruptHandler(AlarmHandler);
    TMR5_StartTimer();
}

