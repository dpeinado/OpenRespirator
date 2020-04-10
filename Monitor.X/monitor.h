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
uint8_t GetPressure_mbar (void);
uint8_t GetPressure_mbar02 (void); // 20 ubar units
uint8_t GetPressure_kpa (void);
uint16_t GetPressure_pa (void);
void MonitorPressureTask(void);
void MonitorDump(void);

// Solenoid functions
void SetSV1 (bool);

// Output values
uint16_t GetTdi(void);
uint16_t GetTde(void);
uint16_t GetTi(void);
uint16_t GetTe(void);
uint16_t GetBp(void);

uint8_t GetPi(void);
uint8_t GetPe(void);
int GetMonitorState(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MONITOR_H */

