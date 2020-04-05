///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        ventilator.c
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.40
//
///////////////////////////////////////////////////////////////////////////////

/// I N C L U D E S ///////////////////////////////////////////////////////////
#include <p18f26k20.h>
#include <timers.h>
#include <pwm.h>
#include "controller.h"
#include "monitor.h"
#include "setting.h"
#include "venttimers.h"
#include "acquire.h"
#include "display.h"

/// P R I V A T E  D E F I N I T I O N S //////////////////////////////////////

/// V A R I A B L E S /////////////////////////////////////////////////////////
#pragma udata
int pressure_table[41];
int peep_correct = 0;
int pip_correct = 0;
t_cycle cycle;

/// P R I V A T E  P R O T O T Y P E S ////////////////////////////////////////
void CorrectPWM(int phase);

/// D E C L A R A T I O N S ///////////////////////////////////////////////////
#pragma code
///////////////////////////////////////////////////////////////////////////////
// Functions:       void CycleControl(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void CycleControl(void)
{
	int temp;
	int start_inspiration_flag;
	
	///////////////////////////////////////////////////////////////////////////	
	// Phase control, in mandatory/support modes //////////////////////////////
	switch(cycle.phase)
	{		
		//////////////////////////////////////////////////////////////////////
		case PHASE_INHALATION:
										// Add alternative temination tests here
			if(GetTimerFlag(PHASE_DONE_FLAG))	// ... or wait for phase done flag
			{
				cycle.phase = PHASE_EXHALATION;	// Switch to exhalation phase
				if(setting.mode > MODE_CPAP)	// In mandatory/support modes
				{								// Set expiration time
					temp = (6000 / setting.bpm);
					temp -= (setting.ti * 10);
					if(temp < 1)
						temp = 1;
					SetTimer(PHASE_DONE_FLAG, temp);	// Set phase timer; clears flag									
					SetTimer(VALVE_STABLE_FLAG,5);		// Valve will deactivate and
				}										// ... be unstable for 50mS
				else
				{										// In Standby or CPAP
					ClearTimerFlag(PHASE_DONE_FLAG);	// Switch phase timer is off
				}										// ... to remain in exhalation
				
				// Test if we are in process of delivering 5 back up breaths
				if(cycle.backup_flag)
				{
					cycle.backup_breath_count--;		// Decrement backup count
					if(cycle.backup_breath_count < 1)	// Are we done?
					{
						cycle.backup_flag = FALSE;		// Switch off backup breaths
						cycle.backup_breath_count = 5;	// Set up for next use
						monitor.exhale_time_count = 0;	// To auto reset apnoea
					}
				}
				
				// Recalculate valve PIP correction
				CorrectPWM(PHASE_INHALATION); 
			}
			break;
			
		///////////////////////////////////////////////////////////////////////			
		case PHASE_EXHALATION:
			switch(setting.mode)
			{
				case MODE_STANDBY:					
				case MODE_CPAP:
					start_inspiration_flag = FALSE;	// Remain in exhalation phase
					break;

				case MODE_PSV:
					if(monitor.trigger_flag)			// Switch on trigger
						start_inspiration_flag = TRUE;
					if(monitor.apnoea_flag)				// Set backup on apnoea
						cycle.backup_flag = TRUE;
					if(cycle.backup_flag)					// During backup
						if(GetTimerFlag(PHASE_DONE_FLAG))	// Switch on time
							start_inspiration_flag = TRUE;
					break;
						
				case MODE_SIMV:
					if(monitor.trigger_flag)			// Switch on trigger
						start_inspiration_flag = TRUE;
					if(GetTimerFlag(PHASE_DONE_FLAG))	// Or, switch on time
						start_inspiration_flag = TRUE;
					break;
					
				case MODE_CMV:
					start_inspiration_flag = FALSE;
					if(GetTimerFlag(PHASE_DONE_FLAG))	// Switch on time
						start_inspiration_flag = TRUE;
					break;
					
				default:
					start_inspiration_flag = FALSE;
			}
			
			// Test if inspiration phase shall be started
			if(start_inspiration_flag)
			{
				start_inspiration_flag = FALSE;		// Use flag just once
				monitor.trigger_flag = FALSE;		// Use flag just once
				cycle.phase = PHASE_INHALATION;
				temp = setting.ti * 10;
				SetTimer(PHASE_DONE_FLAG, temp);	// Inhalation time
				
				// Recalculate valve PEEP correction
				CorrectPWM(PHASE_EXHALATION);
			
				// Set up slope
				if(setting.soft_slope)				// If slope is on
					cycle.slope_step = 1;			// Setup sloping
				else
					cycle.slope_step = 10;			// Set end of sloping
			}
			break;
	}

	///////////////////////////////////////////////////////////////////////////
	// Manual breath control //////////////////////////////////////////////////
	if(setting.inspiration_hold)
	{
		cycle.phase = PHASE_INHALATION;	// Force inspiration
		SetTimer(PHASE_DONE_FLAG, 1); 	// Set up for phase change to expiration,
	}									// ... as soon as button is released

	///////////////////////////////////////////////////////////////////////////
	// Exhalation valve control ///////////////////////////////////////////////
	if(cycle.phase == PHASE_INHALATION)
	{
		if(cycle.slope_step < 10)
		{
			if(GetTimerFlag(SLOPE_STEP_FLAG))
			{
				temp = (setting.ti * 2) / 3; 	// Slope up for 2/3 of iTime
				SetTimer(SLOPE_STEP_FLAG,temp);	// Transfer to timer
				temp = setting.pip - ((setting.pip - setting.peep) / cycle.slope_step);
				temp = pressure_table[temp] + pip_correct;
				SetDCPWM1(temp);		// Set exhalation valve, with sloping value
				cycle.slope_step++;		// Advance to next step
			}
		}
		else
		{
			SetDCPWM1(cycle.pwm_pip);	// Set exhalation valve with PIP
		}
	}
	else
	{
		SetDCPWM1(cycle.pwm_peep);		// Set exhalation valve with PEEP
	}
}	//end CycleControl()


///////////////////////////////////////////////////////////////////////////////
// Function:        void BlendControl(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void BlendControl(void)
{
	Nop();				// Not implemented
} //end BlendControl()


///////////////////////////////////////////////////////////////////////////////
// Functions:       void CorrectPWM(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void CorrectPWM(int phase)
{
	if(cycle.phase == PHASE_EXHALATION)
	{
//		if(p_min > peep)		// adjust peep_correct
//			peep_correct--;
		cycle.pwm_peep = pressure_table[setting.peep] + peep_correct;	// 
	}
	else	// phase == PHASE_INHALATION
	{
//		if(p_max > pip)		// adjust peep_correct
//			pip_correct--;
		cycle.pwm_pip = pressure_table[setting.pip] + pip_correct;	// 
	}
	
}	// end CorrectPWM()

///////////////////////////////////////////////////////////////////////////////
// Functions:       void InitController(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void InitController(void)
{
	// Set up two PWM channels ////////////////////////////////////////////////
	OpenTimer2( TIMER_INT_OFF &		// Interrupt disabled
				T2_PS_1_16 & 		// 1:16 pre-scale for PWM
				T2_POST_1_1);		// 1:1 postscale
	OpenPWM1(0xFF);					// 2.9KHz period (with Timer2 1:16 pre-scale and 48MHz)
	SetDCPWM1(0);					// Start at low cycle
//	SetOutputPWM1(SINGLE_OUT,PWM_MODE_1); 
//	OpenPWM2(0xFF);					// 2.9KHz period (with Timer2 1:16 pre-scale and 48MHz)
//	SetDCPWM2(0);					// Start at low cycle

	cycle.phase = PHASE_EXHALATION;
	SetTimer(PHASE_DONE_FLAG, 300);	// Set exhalation for 3 sec
	cycle.backup_flag = FALSE;		// Switch off backup breaths
	cycle.backup_breath_count = 5;
}	// end InitController()

//// EOF ventilator.c /////////////////////////////////////////////////////////
