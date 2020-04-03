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
uint16_t GetPressure_mbar (void);
uint8_t GetPressure_kpa (void);
uint16_t GetPressure_pa (void);

// Solenoid functions
void SetSV1 (bool);


#ifdef	__cplusplus
}
#endif

#endif	/* MONITOR_H */

