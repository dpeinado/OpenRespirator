///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        venttimers.c
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.40
//
// 
//
///////////////////////////////////////////////////////////////////////////////

/// I N C L U D E S ///////////////////////////////////////////////////////////
#include <p18f26k20.h>
#include <timers.h>
#include "venttimers.h"
#include "controller.h"
#include "setting.h"

/// P R I V A T E  D E F I N I T I O N S //////////////////////////////////////

/// V A R I A B L E S /////////////////////////////////////////////////////////
#pragma udata

int phase_timer;
int phase_done_flag;
int slope_step_timer;
int slope_step_flag;
int switch_timer;
int valve_stable_flag;
int breath_display_timer;
int breath_display_flag;
int lcd_update_timer;
int lcd_update_flag;
int p_mean_display_timer;
int p_mean_display_flag;

/// P R I V A T E  P R O T O T Y P E S ////////////////////////////////////////

/// D E C L A R A T I O N S ///////////////////////////////////////////////////
#pragma code
///////////////////////////////////////////////////////////////////////////////
// Functions:       void ServiceTimers(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        Service timers
// Note:            None
///////////////////////////////////////////////////////////////////////////////
void ServiceTimers(void)
{
	//////////////////////////////////////////////////////////////////////////
	// 10mS event timer tick /////////////////////////////////////////////////
	if(!INTCONbits.TMR0IF)
		return;		// If NO, do nothing

   	ClrWdt();					// Just in case

	///////////////////////////////////////////////////////////////////////////
	// Timer for controlling cycle phase times ////////////////////////////////
	if(phase_timer < 1)
		phase_done_flag = TRUE;
	else
		phase_timer--;
		
	if(phase_timer > (6000 / setting.bpm))	// Limit max time
		phase_timer = (6000 / setting.bpm);

	///////////////////////////////////////////////////////////////////////////
	// Timer for controlling pressure wave slope //////////////////////////////
	if(slope_step_timer < 1)
		slope_step_flag = TRUE;
	else
		slope_step_timer--;
	
	///////////////////////////////////////////////////////////////////////////
	// Timer for recent level change //////////////////////////////////////////
	if(switch_timer < 1)
		valve_stable_flag = TRUE;
	else
		switch_timer--;
	
		///////////////////////////////////////////////////////////////////////////
	// Timer for controlling cycle phase times ////////////////////////////////
	if(breath_display_timer < 1)
		breath_display_flag = FALSE;
	else
		breath_display_timer--;
	
	///////////////////////////////////////////////////////////////////////////
	// Timer for controlling cycle phase times ////////////////////////////////
	if(lcd_update_timer < 1)
		lcd_update_flag = TRUE;
	else
		lcd_update_timer--;
	
	///////////////////////////////////////////////////////////////////////////
	// Timer updating displayed mean pressure value, every 1 sec //////////////
	if(p_mean_display_timer < 1)
		p_mean_display_flag = TRUE;
	else
	{
		p_mean_display_timer--;
	}

}	//end ServiceTimers()	


///////////////////////////////////////////////////////////////////////////////
// Functions:       void SetTimer(int timer_flag, int ticks)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void SetTimer(int timer_flag, int ticks)
{
	switch(timer_flag)
	{
		case TIMER_10MS_FLAG:
			WriteTimer0(ticks);	// Timer counter
			INTCONbits.TMR0IF = FALSE;
			break;
			
		case PHASE_DONE_FLAG:
			phase_timer = ticks;
			phase_done_flag = FALSE;
			break;
			
		case SLOPE_STEP_FLAG:
			slope_step_timer = ticks;
			slope_step_flag = FALSE;
			break;
		
		case VALVE_STABLE_FLAG:
			switch_timer = ticks;
			valve_stable_flag = FALSE;
			break;

		case BREATH_DISPLAY_FLAG:
			breath_display_timer = ticks;
			breath_display_flag = TRUE;
			break;
			
		case LCD_REFRESH_FLAG:
			lcd_update_timer = ticks;
			lcd_update_flag = FALSE;
			break;
			
		case P_MEAN_DISPLAY_FLAG:
			p_mean_display_timer = ticks;
			p_mean_display_flag = FALSE;
			break;
			
		default:
			break;
	}
}	// end SetTimerFlag()


///////////////////////////////////////////////////////////////////////////////
// Functions:       void ClearTimerFalg(int timer_flag)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void ClearTimerFlag(int timer_flag)
{
	switch(timer_flag)
	{
		case TIMER_10MS_FLAG:
			INTCONbits.TMR0IF = FALSE;
			break;
			
		case PHASE_DONE_FLAG:
			phase_done_flag = FALSE;
			break;
			
		case SLOPE_STEP_FLAG:
			slope_step_flag = FALSE;
			break;
		
		case VALVE_STABLE_FLAG:
			valve_stable_flag = FALSE;
			break;

		case BREATH_DISPLAY_FLAG:
			breath_display_flag = FALSE;
			break;
			
		case LCD_REFRESH_FLAG:
			lcd_update_flag = FALSE;
			break;
			
		case P_MEAN_DISPLAY_FLAG:
			p_mean_display_flag = FALSE;
			break;
			
		default:
			break;
	}
}	// end ClearTimerFlag()

///////////////////////////////////////////////////////////////////////////////
// Functions:       int GetTimerFlag(int timer_flag)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
int GetTimerFlag(int timer_flag)
{
	int result;
		
	switch(timer_flag)
	{
		case TIMER_10MS_FLAG:
			result = INTCONbits.TMR0IF;
			break;
			
		case PHASE_DONE_FLAG:
			result = phase_done_flag;
			break;
			
		case SLOPE_STEP_FLAG:
			result = slope_step_flag;
			break;
		
		case VALVE_STABLE_FLAG:
			result = valve_stable_flag;
			break;

		case BREATH_DISPLAY_FLAG:
			result = breath_display_flag;
			break;
			
		case LCD_REFRESH_FLAG:
			result = lcd_update_flag;
			break;
			
		case P_MEAN_DISPLAY_FLAG:
			result = p_mean_display_flag;
			break;
	}
	return result;
}	// end GetTimerFlag()


///////////////////////////////////////////////////////////////////////////////
// Functions:       int GetResetTimerFlag(int timer_flag, int ticks)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
int GetResetTimerFlag(int timer_flag, int ticks)
{
	int result;

	result = FALSE;	// Assume flag is clear
	switch(timer_flag)
	{
		case TIMER_10MS_FLAG:
			if(INTCONbits.TMR0IF)
			{
				result = TRUE;
				WriteTimer0(ticks);	// Timer counter
				INTCONbits.TMR0IF = FALSE;
			}
			break;
			
		case PHASE_DONE_FLAG:
			if(phase_done_flag)
			{
				result = TRUE;
				phase_timer = ticks;
				phase_done_flag = FALSE;
			}
			break;
			
		case SLOPE_STEP_FLAG:
			if(slope_step_flag)
			{
				result = TRUE;
				slope_step_timer = ticks;
				slope_step_flag = FALSE;
			}
			break;
		
		case VALVE_STABLE_FLAG:
			if(valve_stable_flag)
			{
				result = TRUE;
				switch_timer = ticks;
				valve_stable_flag = FALSE;
			}
			break;

		case BREATH_DISPLAY_FLAG:
			if(breath_display_flag)
			{
				result = TRUE;
				breath_display_timer = ticks;
				breath_display_flag = FALSE;
			}
			break;
			
		case LCD_REFRESH_FLAG:
			if(lcd_update_flag)
			{
				result = TRUE;
				lcd_update_timer = ticks;
				lcd_update_flag = FALSE;
			}
			break;
			
		case P_MEAN_DISPLAY_FLAG:
			if(p_mean_display_flag)
			{
				result = TRUE;
				p_mean_display_timer = ticks;
				p_mean_display_flag = FALSE;
			}
			break;
			
		default:
			break;
	}	
	return result;
}	// end GetResetTimerFlag()


///////////////////////////////////////////////////////////////////////////////
// Functions:       void InitTimers(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void InitTimers(void)
{
	// Set up base timer //////////////////////////////////////////////////////
	OpenTimer0( TIMER_INT_OFF &		// Interrupt disabled
				T0_16BIT &			// 16-bit mode
				T0_SOURCE_INT &		// Internal clock source (TOSC)
				T0_PS_1_1);			// 1:1 prescale

	WriteTimer0(TIMER_10MS_COUNT);	// Timer counter
	INTCONbits.TMR0IF = 0;			// Flag
	
	phase_timer = 1;
	phase_done_flag = FALSE;
	slope_step_timer = 1;
	slope_step_flag = FALSE;
	switch_timer = 500;				// Hold off breath detection for 5 sec	
	valve_stable_flag = FALSE;
	breath_display_timer = 1;
	breath_display_flag = FALSE;
	lcd_update_timer = 300;			// Set first to 3 sec, for splash screen display
	lcd_update_flag = FALSE;
	p_mean_display_timer = 1;
	p_mean_display_flag = FALSE;
}	// end InitVentilator()

//// EOF venttimers ///////////////////////////////////////////////////////////
