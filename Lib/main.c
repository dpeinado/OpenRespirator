///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        main.c
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.00
//
//  Program entry point.
///////////////////////////////////////////////////////////////////////////////

/// I N C L U D E S ///////////////////////////////////////////////////////////
#include <p18f26k20.h>
#include "mpuconfig.h"
#include "controller.h"
#include "monitor.h"
#include "setting.h"
#include "venttimers.h"
#include "calibrate.h"
#include "acquire.h"
#include "display.h"

/// M P U  C O N F I G U R A T I O N  S E T T I N G S /////////////////////////
// Refer to mpuconfig.h, P18F26K20.INC in C18 directory, and PIC18F26K20
// datasheet for details of configuration settings and their effects

/// V A R I A B L E S /////////////////////////////////////////////////////////

/// P R I V A T E   P R O T O T Y P E S ///////////////////////////////////////
void Init(void);

/// V E C T O R   R E M A P P I N G ///////////////////////////////////////////
#pragma code
// Default high interrupt vector at address 00000008h (for PIC18 device)
void _high_ISR (void)
{
    ;	// Empty service routine, just in case called
}
// Default low interrupt vector at address 00000018h (for PIC18 device)
void _low_ISR (void)
{
    ;	// Empty service routine, just in case called
}

/// D E C L A R A T I O N S ///////////////////////////////////////////////////
#pragma code
///////////////////////////////////////////////////////////////////////////////
// Functions:       void main(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        Main program entry point
// Note:            None
///////////////////////////////////////////////////////////////////////////////
void main(void)
{
	// Initialise /////////////////////////////////////////////////////////////
    Init();
    
    // Test for calibration routine call //////////////////////////////////////
//	if((!BUTTON_MONITOR) && (!BUTTON_INSP_HOLD)) // Read hardware, settings not yet called
//		Calibrate();
		
	// Main program loop //////////////////////////////////////////////////////
    while(1)
    {
	    ADCTasks();
		ServiceTimers();
		
		if(GetResetTimerFlag(TIMER_10MS_FLAG,TIMER_10MS_COUNT))
		{	
			ServiceMonitor();
			ServiceSettings();
//	setting.mode = MODE_CMV;
//	setting.inspiration_hold = FALSE;
//	setting.soft_slope = FALSE;
//	setting.bpm = 30;
//	setting.ti = 10;

			BlendControl();				// Blender not implemented
			CycleControl();			
			
			if(GetResetTimerFlag(LCD_REFRESH_FLAG,8))	// 10 ticks = 100mS
				UpdateLCDText();		// Refresh display
		}
    }
}  //end main()


///////////////////////////////////////////////////////////////////////////////
// Functions:       void Init(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        Initializes hardware and important global variables.
// Note:            None
///////////////////////////////////////////////////////////////////////////////
void Init(void)
{
	// Designate MPU pins /////////////////////////////////////////////////////
	PORTA = 0;						// Initialise ports
	PORTB = 0;
	PORTC = 0;
	TRISA = 0b11111111;				// PORTA all inputs
	TRISB = 0b00000000;				// PORTB all outputs
	TRISC = 0b11111001;				// PORTC RC1, RC2 outputs, remaining inputs

	OSCCON = 0b01110110;	// Set oscillator: 16MHz, HFINTOSC, 	
//	OSCCON = 0b01010110;	// Set oscillator: 4MHz, HFINTOSC,  	
	
	INTCON = 0;				// Ensure all interrupts are disabled

	// Initialize variables ///////////////////////////////////////////////////
	GetCalValues();			// Retrieve calibration values from EEPROM	
	InitSettings();
	InitADC();
	InitController();
	InitMonitor();
	InitDisplay();
	InitTimers();
} //end Init

/// EOF main.c ///////////////////////////////////////////////////////////////

