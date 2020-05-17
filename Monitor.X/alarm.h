/* 
 * File:   alarm.h
 * Author: carlospardo
 *
 * Created on April 4, 2020, 5:08 PM
 */

#ifndef ALARM_H
#define	ALARM_H

#ifdef	__cplusplus
extern "C" {
#endif



// --------------------- BUZZER ALARM STATE
#define ALARM_NONE 0
#define ALARM_LOW  1 // E 200ms LOW 200ms C 200ms LOW 16s
#define ALARM_MED  2 // C 200ms LOW 200ms A 200ms LOW 200ms F 200ms LOW 8s
#define ALARM_HIGH 4 // C 180ms LOW 100ms A 180ms LOW 100ms F 180ms LOW 400ms A 180ms LOW 100ms F 180ms LOW 1s
                     // ... C 180ms LOW 100ms A 180ms LOW 100ms F 180ms LOW 400ms A 180ms LOW 100ms F 180ms LOW 4s

void AlarmInit(void);
void AlarmCheckTask(void);

void MuteAlarm(void);
void HistAlarm(void);

void TestAlarm(int);
char *GetAlarmState(void);
void SetMaxPressure(int16_t);

void ClearAllAlarm(void);

//bool CircuitFailureAlarm(void);

void SetBatteryFailAlarm(void);
void SetMonitorFailAlarm(void);
void SetControlFailAlarm(void);
void SetGasFailureAlarm(void);
void SetHighPressureAlarmLow(void);
void SetHighPressureAlarmHigh(void);
void SetVeryHighPressureAlarmHigh(void);
void SetCircuitFailureAlarm(void);
void SetBaterryLowAlarmMed(void);
void SetBaterryLowAlarmHigh(void);
void SetEPBelowSetAlarm(void);
void SetEPAboveSetAlarm(void);
void SetIPBelowSetAlarm(void);
void SetIPAboveSetAlarm(void);
void SetTdiTooLongAlarm(void);
void SetTdeTooLongAlarm(void);
void SetVAboveMaxAlarm(void);
void SetVBelowMinAlarm(void);
void SetNoPowerSupplyAlarm(void);

void ClearBatteryFailAlarm(void);
void ClearMonitorFailAlarm(void);
void ClearControlFailAlarm(void);
void ClearGasFailureAlarm(void);
void ClearHighPressureAlarmLow(void);
void ClearHighPressureAlarmHigh(void);
void ClearVeryHighPressureAlarmHigh(void);
void ClearCircuitFailureAlarm(void);
void ClearBaterryLowAlarmMed(void);
void ClearBaterryLowAlarmHigh(void);
void ClearEPBelowSetAlarm(void);
void ClearEPAboveSetAlarm(void);
void ClearIPBelowSetAlarm(void);
void ClearIPAboveSetAlarm(void);
void ClearTdiTooLongAlarm(void);
void ClearTdeTooLongAlarm(void);
void ClearVAboveMaxAlarm(void);
void ClearVBelowMinAlarm(void);
void ClearNoPowerSupplyAlarm(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ALARM_H */

