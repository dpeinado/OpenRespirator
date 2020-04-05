///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        controller.h
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.01
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CONTROLLER_H
#define CONTROLLER_H

/// I N C L U D E S ///////////////////////////////////////////////////////////

/// D E F I N I T I O N S /////////////////////////////////////////////////////
#define TIMER_10MS			26500
#define PHASE_EXHALATION	0
#define PHASE_INHALATION	1

/// S T R U C T U R E S ///////////////////////////////////////////////////////
typedef struct t_cycle
{
	int phase;		// Boolean
	int pwm_peep;
	int pwm_pip;
	int slope_step;
	int backup_flag;
	int backup_breath_count;
} t_cycle;

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;

#define OK      TRUE
#define FAIL    FALSE

/// E X T E R N S /////////////////////////////////////////////////////////////
extern int pressure_table[41];
extern t_cycle cycle;

/// P U B L I C  P R O T O T Y P E S //////////////////////////////////////////
void CycleControl(void);
void BlendControl(void);
void ServiceTimers(void);
void InitController(void);

#endif //CONTROLLER_H

