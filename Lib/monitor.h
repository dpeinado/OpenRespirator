///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        monitor.h
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.01
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MONITOR_H
#define MONITOR_H

/// I N C L U D E S ///////////////////////////////////////////////////////////

/// D E F I N I T I O N S /////////////////////////////////////////////////////
#define PHASE_EXHALATION	0
#define PHASE_INHALATION	1

/// S T R U C T U R E S ///////////////////////////////////////////////////////
typedef struct t_monitor
{
	int phase;
	int last_phase;
	int trigger_flag;
	int apnoea_flag;
	int cycle_fail;
	int cycle_fail_flag;
	int cycle_fail_count;
	int inhale_time_count;
	int exhale_time_count;
	int bpm;
	int ti;
	int te;		// Expiratory time
	int ie_ratio;
	int spont;
	int p_trigger;
	int p_mean;
	int p_mean_display;
	int paw_50ms;
	int paw_2000ms;
	int p_min;
	int p_max;
} t_monitor;

/// E X T E R N S /////////////////////////////////////////////////////////////
extern t_monitor monitor;

/// P U B L I C  P R O T O T Y P E S //////////////////////////////////////////
void ServiceMonitor(void);
void InitMonitor(void);

#endif //MONITOR_H

