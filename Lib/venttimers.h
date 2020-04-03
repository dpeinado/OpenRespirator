///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        venttimers.h
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.01
// Company:         Nu-Turn Solutions
//
///////////////////////////////////////////////////////////////////////////////

#ifndef VENTTIMERS_H
#define VENTTIMERS_H

/// I N C L U D E S ///////////////////////////////////////////////////////////

/// D E F I N I T I O N S /////////////////////////////////////////////////////
#define TIMER_10MS_COUNT	26500
#define TIMER_10MS_FLAG		0
#define PHASE_DONE_FLAG		1
#define SLOPE_STEP_FLAG		2
#define VALVE_STABLE_FLAG	3
#define BREATH_DISPLAY_FLAG	4
#define LCD_REFRESH_FLAG	5
#define P_MEAN_DISPLAY_FLAG	6

/// S T R U C T U R E S ///////////////////////////////////////////////////////

/// E X T E R N S /////////////////////////////////////////////////////////////

/// P U B L I C  P R O T O T Y P E S //////////////////////////////////////////
void ServiceTimers(void);
void SetTimer(int timer_flag, int ticks);
void ClearTimerFlag(int timer_flag);
int GetTimerFlag(int timer_flag);
int GetResetTimerFlag(int timer_flag, int ticks);
void InitTimers(void);

#endif //VENTTIMERS_H

