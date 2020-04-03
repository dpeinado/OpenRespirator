///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// Filename:        acquire.c
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.00
//
// ADC acquisition and data convertion algorithms/routines 
//
///////////////////////////////////////////////////////////////////////////////

/// I N C L U D E S ///////////////////////////////////////////////////////////
#include <p18f26k20.h>
#include <adc.h>
#include <delays.h>
#include "acquire.h"
#include "calibrate.h"

/// P R I V A T E  V A R I A B L E S //////////////////////////////////////////
#pragma udata

/// G L O B A L  V A R I A B L E S ////////////////////////////////////////////
// Following are redeclared as EXTERN in header file
int pressure;
int pot_bpm;
int pot_ti;
int pot_peep;
int pot_pip;

/// P R I V A T E  P R O T O T Y P E S ////////////////////////////////////////
int ADC2Pressure(int val);

/// D E C L A R A T I O N S ///////////////////////////////////////////////////
#pragma code
///////////////////////////////////////////////////////////////////////////////
// Function:        void ADCTasks(void)
// PreCondition:    None
// Input:           Uses global variables
// Output:          Writes to global variables
// Side Effects:    None
// Overview:		Reads 10-bit ADC. Add-up 4 samples taken at 180uS interval,
//					to produce 12-bit equivalent value. Sample ready in ..uS.
///////////////////////////////////////////////////////////////////////////////
void ADCTasks(void)
{
	static int sample_count = 0;
	static int pressure_sample_accum = 0;
//	static int pot_bpm_accum;
//	static int pot_ti_accum;
//	static int pot_peep_accum;
//	static int pot_pip_accum;

	// Get pressure sensor ADC into accumulator of 4 samples //////////////////
	SetChanADC(ADC_CH0);			// Set channel for pressure sensor
	Delay10TCYx(ADC_SETTLING_CYCLES); // Settling delay
	ConvertADC();					// Start conversion
	while(BusyADC());				// Wait for completion
	pressure_sample_accum += ReadADC(); // Read result into block

	// Test if sampling build is complete /////////////////////////////////
	sample_count++;
	if(sample_count > 3)	
	{
		pressure = ADC2Pressure(pressure_sample_accum);	// Convert pressure

		SetChanADC(ADC_CH4);			// Set channel for BPM potentiomater
		Delay10TCYx(ADC_SETTLING_CYCLES); // Settling delay
		ConvertADC();					// Start conversion
		while(BusyADC());				// Wait for completion
		pot_bpm = ReadADC();			// Read result
		
		SetChanADC(ADC_CH3);			// Set channel for i-Time potentiometer
		Delay10TCYx(ADC_SETTLING_CYCLES); // Settling delay
		ConvertADC();					// Start conversion
		while(BusyADC());				// Wait for completion
		pot_ti = ReadADC();				// Read result

		SetChanADC(ADC_CH2);			// Set channel for PEEP potentiomater
		Delay10TCYx(ADC_SETTLING_CYCLES); // Settling delay
		ConvertADC();					// Start conversion
		while(BusyADC());				// Wait for completion
		pot_peep = ReadADC();			// Read result
			
		SetChanADC(ADC_CH1);			// Set channel for PIP potentiometer
		Delay10TCYx(ADC_SETTLING_CYCLES); // Settling delay
		ConvertADC();					// Start conversion
		while(BusyADC());				// Wait for completion
		pot_pip = ReadADC();			// Read result

		// Reset parameters for next cycle
		sample_count = 0;
		pressure_sample_accum = 0;					// Reset for next cycle
	}
}	// End ADCTasks()


///////////////////////////////////////////////////////////////////////////////
// Function:        int ADC2Pressure(void)
// PreCondition:    p_offset and p_range holds valid calibration values
// Input:           Uses global variables
// Output:          Writes to global variables
// Side Effects:    None
// Overview:		Converts 12-bit ADC value to mbar pressure value
// Note:            
///////////////////////////////////////////////////////////////////////////////
int ADC2Pressure(int val)
{
	long i;

	i = val - p_offset;
	i = i * 100;
	i = i + 5;			// Correct 0.05 mbar for integer rounding down
	i = i / p_10range;

	return (int)i;
}	// End ADC2Pressure()
///////////////////////////////////////////////////////////////////////////////
// Functions:       void InitADC(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void InitADC(void)
{	
	// Set up three ADC channels //////////////////////////////////////////////
	OpenADC(ADC_FOSC_8 & 			// Converter clock source
			ADC_RIGHT_JUST & 		// Format with 2 MSB in ADRESH
			ADC_8_TAD,				// Acquisition time: TADQmin = 7.5uS
			ADC_CH0 & 				// Start with proximal pressure sensor in focus
			ADC_INT_OFF & 			// Interrupt disabled
			ADC_VREFPLUS_VDD & 		// Positive voltage reference to VDD
			ADC_VREFMINUS_VSS,		// Negative voltage reference to VSS
			ADC_5ANA);				// (31) Enable AN0,AN1,AN2,AN3,AN4	

}	// end InitADC()

// EOF acquire.c ////////////////////////////////////////////////////////////
