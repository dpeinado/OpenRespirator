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
    
// ADC functions
#define ADC_ID_PRS 0
#define ADC_ID_MIC 1
#define ADC_ID_VOL 2
#define ADC_ID_12V 3
    
#include "mcc_generated_files/adcc.h"
    
    
bool AdcDataReady(int);
adc_result_t AdcGetData(int);
    
// Pressure functions
void MonitorInit (void);
int16_t GetPressure_mbar (void);
int16_t GetPressure_mbar02 (void); // 20 ubar units
int16_t GetPressure_kpa (void);
int16_t GetPressure_pa (void);
void MonitorPressureTask(void);
void MonitorDump(void);
void SetCalibrateState(bool);
void SetTarget(int16_t pmax, int16_t ip, int16_t ep, uint16_t br, int16_t vmax, int16_t vmin);
void MonitorEnable(void);
void MonitorDisable(void);
int16_t GetTargetIp(void);
int16_t GetTargetEp(void);
int16_t GetPressureV_pa(void);
int16_t Get12V(void);
int16_t GetVolume(void);
int16_t GetPmax(void);
void SetAlarmSV1(bool);

// Solenoid functions
void SetSV1 (bool);

// Output values
int16_t GetTdi(void);
int16_t GetTde(void);
int16_t GetTi(void);
int16_t GetTe(void);
int16_t GetBp(void);
int16_t GetBpm(void);
int16_t GetTargetIp();
int16_t GetTargetEp();

int16_t GetIp(void);
int16_t GetEp(void);
int16_t GetMaxPressure(void);
int GetMonitorState(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MONITOR_H */

