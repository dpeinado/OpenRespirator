/* Author: David Ortiz
 *
 * Created on April 4, 2020, 12:56 PM
 */

#ifndef ORESPGLOBAL_H
#define	ORESPGLOBAL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"
#include "aCapture.h"
#include "time.h"

// User parameter limits.
#define BPM_MIN 10
#define BPM_MAX 30
#define IP_MIN 4
#define IP_MAX 60
#define PEEP_MIN 4
#define PEEP_MAX 25
    // Breath detect trigger, in mbar.
#define BDT_MIN 1
#define BDT_MAX 8
// Volume limit. With limitations of this equipment volume below 100ml can not be provided with accuracy. Probably lower limit should be higher.
#define VOL_MIN 100
#define VOL_MAX 1500
#define VOL_AMIN 50
// At this moment this is only due to restriction on display.
#define VOL_AMAX 1500

// Default value for volume alarms. +-25%
#define VOL_ALRM_DFL 25
    
// Max Volume overshoot. In mililiters.
#define VOVERSHOOT_LIMIT 25
// Limit for pressure quanta estimation. Needed to saturate quanta measurement to avoid a deathlock.
#define PQUANTA_LIMIT    MPRESSURE_MBAR(8)
// Limit for pressure quanta estimation. Needed to saturate quanta measurement to avoid a deathlock.
#define VQUANTA_LIMIT    90
// Zero volume threshold for aux sensor.
#define ZVOLTH 20

#define PEEP_COMP   MPRESSURE_MBAR(0.4)

    
// Objective time for inspiration.
#define INSP_TIME 500
    //////////////////////////////////////
    // Actuation pins.
    //////////////////////////////////////
#if 0
#define SV2ISOPEN LATAbits.LATA2
#define OPEN_SV2 LATAbits.LATA2 = 1
#define CLOSE_SV2 LATAbits.LATA2 = 0
#else
#define SV2ISOPEN (LATAbits.LATA2|LATCbits.LATC3)
#define OPEN_SV2 LATAbits.LATA2 = 1;LATCbits.LATC3 = 1
#define CLOSE_SV2 LATAbits.LATA2 = 0;LATCbits.LATC3 = 0

#define OPEN_SV2LOW LATAbits.LATA2 = 1;LATCbits.LATC3 = 0
#define OPEN_SV2MED LATAbits.LATA2 = 0;LATCbits.LATC3 = 1
#define SV2LOWISOPEN LATAbits.LATA2
#define SV2MEDISOPEN LATCbits.LATC3

//#define SV2ISOPEN (sv2_pwmval!=0)
//#define OPEN_SV2 sv2_pwmval=100;PWM5_LoadDutyValue(sv2_pwmval)
//#define CLOSE_SV2 sv2_pwmval=0;PWM5_LoadDutyValue(sv2_pwmval)
#endif
    
#define SV3ISOPEN LATAbits.LATA3
#define OPEN_SV3 LATAbits.LATA3 = 1
#define CLOSE_SV3 LATAbits.LATA3 = 0

#define BLED_ON  LATDbits.LATD6 = 1
#define BLED_OFF LATDbits.LATD6 = 0

#define BUZZERISON  LATDbits.LATD0
#define BUZZER_ON  LATDbits.LATD0 = 1
#define BUZZER_OFF LATDbits.LATD0 = 0

#define DEBUG
//#define OCTAVE

#ifdef DEBUG
#define ERROR_CONDITION(k) CLOSE_SV2;CLOSE_SV3;printf("Fatal %d",k)
#else
#define ERROR_CONDITION(k) CLOSE_SV2;CLOSE_SV3;BUZZER_ON; while(1)
#endif



#ifdef DEBUG
#ifdef OCTAVE
#define OCTAVE_PRINT(x) printf x
#define DEBUG_PRINT(x) do {} while (0)
#else
#define OCTAVE_PRINT(x) do {} while (0)
#define DEBUG_PRINT(x) printf x
#endif
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif


    typedef enum {
        VMODE_PRESSURE = 0,
        VMODE_VOLUME = 1
    } vmodeT;
    
    typedef enum {
    CTRL_UNCAL,
    CTRL_STOP,
    CTRL_RUN,
    CTRL_SLEEP
} ctrlStatusT;

    // Global variables.
    extern ctrlStatusT ctrlStatus;
    extern ctrlStatusT ctrlStatus;
    extern vmodeT VentMode; // 0 --> Pressure-control, 1 --> Volume-control.
    extern uint8_t BPM;
    extern uint16_t IDuration, EDuration;
    extern uint8_t IP;
    extern uint8_t MaxP;
    extern uint8_t MaxV;
    extern uint8_t BdTrig;
    extern uint8_t LowVAlarm;
    extern uint8_t HighVAlarm;
    extern uint8_t PEEP;
    extern uint8_t eBRate;
    extern int16_t vddValMean;
    // Change detection for all user settings.
    extern bool chBdTrig, chBPM, chIP, chMaxP, chPEEP, chLowVAlarm, chHighVAlarm, chMaxV, chPEEP, chVentMode;
    extern int16_t intIP, intMaxV;

    extern uint16_t lastCycleVol;
    extern uint16_t sv2_pwmval;
    extern time_t rSV2ValveORT, rSV2ValveCRT, rSV3ValveORT;
    extern uint16_t lungC, lungR;


#ifdef	__cplusplus
}
#endif

#endif	/* ORESPGLOBAL_H */

