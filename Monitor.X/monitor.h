/* 
 * File:   monitor.h
 * Author: carlospardo
 *
 * Created on April 3, 2020, 12:33 PM
 */

#ifndef MONITOR_H
#define	MONITOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
// Pressure functions
void InitializePressure (void);
int16_t GetPressure_mbar (void);
int16_t GetPressure_mbar02 (void); // 20 ubar units
int16_t GetPressure_kpa (void);
int16_t GetPressure_pa (void);
void MonitorPressureTask(void);
void MonitorDump(void);
void SetCalibrateState(bool calib);

// Solenoid functions
void SetSV1 (bool);

// Output values
uint16_t GetTdi(void);
uint16_t GetTde(void);
uint16_t GetTi(void);
uint16_t GetTe(void);
uint16_t GetBp(void);

int16_t GetIp(void);
int16_t GetEp(void);
int16_t GetMaxPressure(void);
int GetMonitorState(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MONITOR_H */

