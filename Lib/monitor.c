///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        monitor.c
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.40
//
///////////////////////////////////////////////////////////////////////////////

/// I N C L U D E S ///////////////////////////////////////////////////////////
#include <p18f26k20.h>
#include "monitor.h"
#include "controller.h"
#include "setting.h"
#include "venttimers.h"
#include "acquire.h"
#include "display.h"

/// P R I V A T E  D E F I N I T I O N S //////////////////////////////////////

/// V A R I A B L E S /////////////////////////////////////////////////////////
#pragma udata
t_monitor monitor;

/// P R I V A T E  P R O T O T Y P E S ////////////////////////////////////////
void FilterP(void);
void SetMessage(void);

/// D E C L A R A T I O N S ///////////////////////////////////////////////////
#pragma code
///////////////////////////////////////////////////////////////////////////////
// Functions:       void ServiceMonitor(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void ServiceMonitor(void)
{
	static int p_min_temp = 500;
	static int p_max_temp = -500;
	
	///////////////////////////////////////////////////////////////////////////
	// Recalculate filtered pressure values used in breath and phase detection
	FilterP();

	///////////////////////////////////////////////////////////////////////////
	// Breath detection ///////////////////////////////////////////////////////	
	if(monitor.phase == PHASE_EXHALATION)		// During exhalation phase only
		if(GetTimerFlag(VALVE_STABLE_FLAG))		// Not during valve deactivation
			if(!GetTimerFlag(BREATH_DISPLAY_FLAG)) // Not while already displaying
												// Is there breath?
				if((monitor.paw_50ms + trigger_sens) < monitor.paw_2000ms)
				{										// If YES
					SetTimer(BREATH_DISPLAY_FLAG, 30);	// Show flag for 300mS
					monitor.trigger_flag = TRUE;		// Can also be reset by Controller
					monitor.p_trigger = monitor.paw_2000ms; // Record pressure
				}												  // .. for bar graph
				else
				{
					monitor.trigger_flag = FALSE;
				}

	///////////////////////////////////////////////////////////////////////////
	// Determine phase by monitoring (controller has own mandatory phase) /////
	if(setting.mode > MODE_CPAP)
	{												// If in mandatory/support modes
		if(monitor.paw_50ms > (monitor.p_mean + 2))	// Is pressure > mean pressure + 0.2mbar
			monitor.phase = PHASE_INHALATION;
		else
			monitor.phase = PHASE_EXHALATION;
	}
	else 
	{												// If CPAP mode
		if(GetTimerFlag(BREATH_DISPLAY_FLAG)) 		// Determine by breath detection
			monitor.phase = PHASE_INHALATION;
		else
			if(monitor.paw_50ms > (monitor.p_mean - 2))
				monitor.phase = PHASE_EXHALATION;	// Revert phase when pressure restored
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Count inhale/exhale times and look for min/max pressures ///////////////
	if(monitor.phase == PHASE_INHALATION)		// Increment exhalation phase counter
	{
		monitor.inhale_time_count++;			// ... used in E:I ratio and apnoea detection

		if(setting.mode > MODE_CPAP)
		{
			if(monitor.paw_50ms > p_max_temp)		// Look for maximum pressure
				p_max_temp = monitor.paw_50ms;		// ... during inhalation
		}
		else
		{
			if(monitor.paw_50ms < p_min_temp)		// Look for minimum pressure
				p_min_temp = monitor.paw_50ms;		// ... during CPAP inhalation
		}
	}
	else
	{
		monitor.exhale_time_count++;

		if(setting.mode > MODE_CPAP)
		{
			if(monitor.paw_50ms < p_min_temp)		// Look for minimum pressure
				p_min_temp = monitor.paw_50ms;		// ... during exhalation
		}
		else
		{
			if(monitor.paw_50ms > p_max_temp)		// Look for maximum pressure
				p_max_temp = monitor.paw_50ms;		// ... during CPAP exhalation
		}
	}
	
	// If inhalation conincide with a trigger, then count as spont; else machine
	
	///////////////////////////////////////////////////////////////////////////	
	// Transfer values on inhalation/exhalation direction change //////////////
	if(monitor.last_phase != monitor.phase)	// Have we just changed direction?
	{
		if(monitor.phase == PHASE_INHALATION)
		{											// If just end of an expiration
			monitor.ti = monitor.inhale_time_count;	// Determine monitored ti and
			monitor.inhale_time_count = 0;			// ... reset exhalation time counter
			if(p_min_temp < 490)					// Determine p_min
				monitor.p_min = p_min_temp;
			p_min_temp = 500;
		}
		else
		{											// If just end of an inspiration
			monitor.te = monitor.exhale_time_count;	// Determine te and
			monitor.exhale_time_count = 0;			// ... reset exhalation time counter
			if(p_max_temp > -490)					// Determine p_max
				monitor.p_max = p_max_temp;
			p_max_temp = -500;
		}
	}
		
	monitor.last_phase = monitor.phase;

	///////////////////////////////////////////////////////////////////////////
	// Calculate monitored parameters /////////////////////////////////////////
	// Calculate monitored bpm
	// Calculate monitored IE ratio
	// Calculate monitored spont
	
	// Test for cycle fail: if pmin is not PEEP, and pmax is not PIP
	if(((monitor.p_min - 8) < (setting.peep * 10)) || 
	   ((monitor.p_min + 8) > (setting.peep * 10)) ||
	   ((monitor.p_max - 8) < (setting.pip  * 10)) ||
	   ((monitor.p_max + 8) > (setting.pip  * 10)))
		monitor.cycle_fail = TRUE;
	else
		monitor.cycle_fail = FALSE;
	
	///////////////////////////////////////////////////////////////////////////
	// Apnoea test ////////////////////////////////////////////////////////////
	if(setting.apnoea_monitor)
		if(monitor.exhale_time_count > 1500)
			monitor.apnoea_flag = TRUE;		// Set apnoea flag
		else
			monitor.apnoea_flag = FALSE;	// Clear apnoea flag
	else
		monitor.apnoea_flag = FALSE;		// Clear apnoea flag

	///////////////////////////////////////////////////////////////////////////
	// Update display message /////////////////////////////////////////////////
	SetMessage();

}	//end CycleControl()


///////////////////////////////////////////////////////////////////////////////
// Functions:       void FilterP(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void FilterP(void)
{
	static long p_accum = 0;
	static long paw_50ms_accum = 0;
	static long paw_2000ms_accum = 0;
	
	if(GetResetTimerFlag(P_MEAN_DISPLAY_FLAG,100))	// Update again in 1 sec
		monitor.p_mean_display = monitor.p_mean;
	
	paw_50ms_accum = paw_50ms_accum - monitor.paw_50ms + pressure;
	monitor.paw_50ms = paw_50ms_accum / 3;

	paw_2000ms_accum = paw_2000ms_accum - monitor.paw_2000ms + pressure;
	monitor.paw_2000ms = paw_2000ms_accum / 50;
		
	p_accum = p_accum - monitor.p_mean + pressure;
	monitor.p_mean = p_accum / 1000;	//
}

///////////////////////////////////////////////////////////////////////////////
// Functions:       void SetMessage(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void SetMessage(void)
{
	message = 0;	// Assume ok

	if(setting.high_ie_flag)
		message = 7;	// I:E ratio > 2:1
		
	if(monitor.apnoea_flag)
		message = 6; // Apnoea alarm

/*	if(monitor.cycle_fail)
	{
		monitor.cycle_fail_count--;
		if(monitor.cycle_fail_count < 0)	// Longer than 5 sec?
			monitor.cycle_fail_flag = TRUE;
	}
	else
	{
		monitor.cycle_fail_count = 50;		// Reset 5 sec grace timer
		monitor.cycle_fail_flag = FALSE;
	}
*/		
	if(setting.mode == MODE_STANDBY)
		message = 3;
}

///////////////////////////////////////////////////////////////////////////////
// Functions:       void InitMonitor(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void InitMonitor(void)
{	
	monitor.apnoea_flag = FALSE;
	monitor.p_mean = 0;
	monitor.p_mean_display = 0;
	monitor.cycle_fail_count = 100;	// Set 10 sec grace on power up
}	// end InitMonitor()

//// EOF ventilator.c /////////////////////////////////////////////////////////
