///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// Filename:        calibrate.c
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.00
//
// Calibration routine 
//
///////////////////////////////////////////////////////////////////////////////

/// I N C L U D E S ///////////////////////////////////////////////////////////
#include <p18f26k20.h>
#include <stdlib.h>
#include <string.h>
#include <delays.h>
#include "controller.h"
#include "setting.h"
#include "calibrate.h"
#include "display.h"

/// P R I V A T E  D E F I N I T I O N S //////////////////////////////////////
#pragma romdata const_table	// default is 'near' when in small memory model
const rom near unsigned char CAL_MSG[12][21] =
   {"Engineering Mode    ",		// 0
	"Trigger Sensitivity ",		// 1
	"Adjust PIP dial:    ",		// 2
    "Set input reg 2bar  ",		// 3
    "Set fresh gas 8l/min",		// 4
    "Zero pressure sensor",		// 5
    "Set pressure 30mbar ",		// 6
    "Calibrate circuit   ",		// 7
    "Please Wait ...     ",		// 8
    "Re-Boot Now         ",		// 9
    "                    ",		// 10
    "Command Fail        "};	// 11

const rom near unsigned char CONFIRM[7] = "NO YES";

const rom near int P_TABLE[41] =
	{  0,286,302,317,333,349,365,380,396,412,428,443,459,475,491,506,522,538,
//	  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17mbar
	 554,569,585,601,617,632,648,664,680,695,711,727,743,758,774,790,806,821,
//	  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35mbar
	 837,853,869,884,900};
//	  36  37  38  39  40mbar

/// P R I V A T E  V A R I A B L E S //////////////////////////////////////////
#pragma udata
unsigned char now;

/// G L O B A L  V A R I A B L E S ////////////////////////////////////////////
/// redeclared as EXTERN in header file
int p_offset;
int p_10range;
int trigger_sens;

/// P R I V A T E  P R O T O T Y P E S ////////////////////////////////////////
int GetYesNo(void);
void CommandFail(void);
unsigned char ReadEEPROM(unsigned char addr);
unsigned char WriteEEPROM(unsigned char addr, unsigned char data);


/// D E C L A R A T I O N S ///////////////////////////////////////////////////
#pragma code
///////////////////////////////////////////////////////////////////////////////
// Functions:       void Calibrate(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void Calibrate(void)
{
	int i;
	int pwm;
	int temp;
	unsigned char temp_str[4];

	///////////////////////////////////////////////////////////////////////////
	// Set up calibration mode text ///////////////////////////////////////////
	memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 0],20);	// "Engineering Mode"
	memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
	memcpypgm2ram(&lcd_string[28],&CONFIRM,3);		// "NO" text
	lcd_refresh(lcd_string);						// Update lcd

	if(!GetYesNo())
		return;			// If NO, exit calibration routine

	///////////////////////////////////////////////////////////////////////////
	memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[1],20);	// "Trigger Sensitivity"
	memcpypgm2ram(&lcd_string[20],&CAL_MSG[2],20);	// "Adjust PIP dial:   "
	lcd_refresh(lcd_string);						// Update lcd

	while(BUTTON_INSP_HOLD)		// While confirm is not pressed
	{
		ClrWdt();			// Reset watchdog timer - just in case
		Delay10KTCYx(40);		// Wait 100mS (@16MHz clock), to absorb any button bounce

		ADCTasks();				// Read potentiometer ADC
		ServiceSettings();		// Convert into 0-15 value range
		trigger_sens = setting.peep;	// Display as trigger_sens
		itoa(trigger_sens,(char*)temp_str);
		if(temp_str[1] == 0)
		{
			temp_str[1] = temp_str[0];
			temp_str[0] = ' ';
		}
		lcd_string[37] = temp_str[0];
		lcd_string[38] = temp_str[1];		
		lcd_refresh(lcd_string);
	}

	// When button is pressed, store default trigger sensitivity
	temp = WriteEEPROM(1,trigger_sens);
	if(temp != trigger_sens)
		CommandFail();		// Error storing calibration value

	///////////////////////////////////////////////////////////////////////////
	memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 3],20);	// "Set input reg 2bar"
	memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
	memcpypgm2ram(&lcd_string[28],&CONFIRM,3);		// "NO" text
	lcd_refresh(lcd_string);						// Update lcd

	if(!GetYesNo())
		return;			// If NO (not 2.5bar), exit

	///////////////////////////////////////////////////////////////////////////
	memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 4],20);	// "Set fresh gas 8l/min"
	memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
	memcpypgm2ram(&lcd_string[28],&CONFIRM,3);		// "NO" text
	lcd_refresh(lcd_string);						// Update lcd

	if(!GetYesNo())
		return;			// If NO (not 8 l/min), exit

	///////////////////////////////////////////////////////////////////////////
	memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 5],20);	// "Zero pressure sensor"
	memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
	memcpypgm2ram(&lcd_string[28],&CONFIRM,3);		// "NO" text
	lcd_refresh(lcd_string);						// Update lcd

	if(!GetYesNo())
	{
		memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 8],20);	// "Please Wait ..."
		memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
		lcd_refresh(lcd_string);						// Update lcd

		// ADC p_offset; 5 sec averaging
		// Store p_offset in EEPROM
		temp = WriteEEPROM(2,p_offset);
		if(temp != p_offset)
			CommandFail();		// Error storing calibration value
	}
	
	///////////////////////////////////////////////////////////////////////////
	memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 6],20);	// "Set pressure 30mbar"
	memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
	memcpypgm2ram(&lcd_string[28],&CONFIRM,3);		// "NO" text
	lcd_refresh(lcd_string);						// Update lcd

	if(!GetYesNo())
	{
		memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 8],20);	// "Please Wait ..."
		memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
		lcd_refresh(lcd_string);						// Update lcd

		/// ADC p_10range; 5 sec averaging
		// Store p_10range in EEPROM
		temp = WriteEEPROM(3,p_10range);
		if(temp != p_10range)
			CommandFail();		// Error storing calibration value
	}

	///////////////////////////////////////////////////////////////////////////
	memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 7],20);	// "Calibrate circuit"
	memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
	memcpypgm2ram(&lcd_string[28],&CONFIRM,3);		// "NO" text
	lcd_refresh(lcd_string);						// Update lcd

	if(!GetYesNo())
	{
		memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 8],20);	// "Please Wait ..."
		memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
		lcd_refresh(lcd_string);						// Update lcd

		;	// Cycle 0mbar to 40mbar, store PWM values
		for(i=0; i<41; i++)
		{
			// Adjust pwm to get i mbar
			// Settle for 1 sec
			
			temp = WriteEEPROM(i + 4,pwm);
			if(temp != pwm)
				CommandFail();		// Error storing calibration value
		}
	}

	// Prompt for restart /////////////////////////////////////////////////////
	memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[ 9],20);	// "Re-Boot Now" top line
	memcpypgm2ram(&lcd_string[20],&CAL_MSG[10],20);	// Clear bottom line
	lcd_refresh(lcd_string);						// Update lcd
	while(1)
		ClrWdt();
}	// end Calibrate()


///////////////////////////////////////////////////////////////////////////////
// Function:		void GetCalValues(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        Retreives calibration values from EEPROM and stores in 
//					global RAM variables
///////////////////////////////////////////////////////////////////////////////
void GetCalValues(void)
{
	unsigned char i;
	
	trigger_sens = ReadEEPROM(1);	// Load default trigger sensitivity
	p_offset = ReadEEPROM(2);
	p_10range = ReadEEPROM(3);

	for(i=0;i<41;i++)
		pressure_table[i] = ReadEEPROM(i + 4);
		
// TEMPORARY
	trigger_sens = 8;
	p_offset = 1494;
	p_10range = 650;
	for(i=0;i<41;i++)
		pressure_table[i] = P_TABLE[i];

}	// end GetCalValues()


///////////////////////////////////////////////////////////////////////////////
// Functions:       int GetYesNo(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
int GetYesNo(void)
{
	int accept;
	
	// Wait until button is released
	while(!BUTTON_INSP_HOLD)
		ClrWdt();			// Reset watchdog timer - just in case
	Delay10KTCYx(40);		// Wait 100mS (@16MHz clock), to absorb any button bounce

	accept = FALSE;
	while(BUTTON_INSP_HOLD)		// While confirm is not pressed
	{
		if(!BUTTON_MONITOR)			// Select button press
		{
			if(accept)
			{
				accept = FALSE;
				memcpypgm2ram(&lcd_string[28],&CONFIRM,3);		// NO text
			}	
			else
			{
				accept = TRUE;
				memcpypgm2ram(&lcd_string[28],&CONFIRM[3],3);	// YES text
			}	
			lcd_refresh(lcd_string);
			Delay10KTCYx(40);		// Wait 100mS (@16MHz clock)
		}
		ClrWdt();			// Reset watchdog timer
	}	
	// Wait until encoder button is released
	while(!BUTTON_INSP_HOLD)
		ClrWdt();			// Reset watchdog timer
	
	Delay10KTCYx(40);		// Wait 100mS (@16MHz clock), to absorb any button bounce

	return accept;
}

///////////////////////////////////////////////////////////////////////////////
// Functions:       void CommandFail(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void CommandFail(void)
{
	memcpypgm2ram(&lcd_string[ 0],&CAL_MSG[11],20);	// "Command Failed" top line
	memcpypgm2ram(&lcd_string[16],&CAL_MSG[ 9],20);	// "Reboot now"
	lcd_refresh(lcd_string);						// Update lcd
	while(1)
		ClrWdt();			// Reset watchdog timer - just in case
}	// end CommandFail()


///////////////////////////////////////////////////////////////////////////////
// Functions:       unsigned char ReadEEPROM(int addr)
//					unsigned char WriteEEPROM(int addr, unsigned char data)
// PreCondition:    None
// Input:           Address and chararcter to write. Returns final EEPROM
//					value at the given address.
// Output:          None
// Side Effects:    EEPROM capacity is reduced to 1/5.
// Overview:        Uses Microchip standard read and write sequence. Write is
//					made to 5 consequtive EEPROM cell locations, with a read-back
//					check on the first location. Read is performed on 5 consequtive
//					cell locations and the 5 compared. On discrepancy, a 'best 3
//					of 5 test' is performed and all 5 cells refreshed/rewritten 
//					with the best 3 (at least) value.
// Note:            Best 3 of 5 algoritms detects and rectifies any single or
//					two EEPROM cell deterioration. Only the lower 1/5 of the
//					EEPROM area must be addressed, or unknown error may occur.
///////////////////////////////////////////////////////////////////////////////
unsigned char ReadEEPROM(unsigned char addr)
{
	unsigned char i;
	unsigned char val;
	unsigned char cell_val[5][2];

	for(i=0;i<5;i++)
	{	
		EEADR = (addr * 5) + i;	// Location
		EECON1bits.EEPGD = 0;	// Ensure we point to data EEPROM
		EECON1bits.CFGS = 0;	// Assure we point to data EEPROM
		EECON1bits.RD = 1;		// EEPROM read routine
		val = EEDATA;			// Data on first clock cycle
		cell_val[i][0] = val;	// Data into array
		cell_val[i][1] = 0;		// Clear repeat counter
	}
	EEADR = 200;	// Make sure address is left pointing to unimportant cell,
					// ... just in case sporadic/brown-out write occurs
	for(i=0;i<5;i++)
	{
		if(cell_val[i][0] == cell_val[0][0]) cell_val[i][1]++;
		if(cell_val[i][0] == cell_val[1][0]) cell_val[i][1]++;
		if(cell_val[i][0] == cell_val[2][0]) cell_val[i][1]++;
		if(cell_val[i][0] == cell_val[3][0]) cell_val[i][1]++;
		if(cell_val[i][0] == cell_val[4][0]) cell_val[i][1]++;
	}

	if(cell_val[0][1] == 5) // Are all 5 EEPROM cell locations identical
	{
		return val;			// If YES, straight forward return
	}

	for(i=0;i<5;i++)		// If NO, resolve cell deterioration
	{
		val = cell_val[i][1];
		if(val > 2)					// Is cell value 3 or more?
		{
			val = cell_val[i][0];	// If YES, rewrite all 5 cell locations
			WriteEEPROM(addr,val);	// ... with the best value
			return val;				// ... and return with the best value
		}
	}	
	return 0;		// We got to here because there were no 3 best matches
					// .. return with zero. Program checksum test must detect
}	// end ReadEEPROM()
///////////////////////////////////////////////////////////////////////////////
unsigned char WriteEEPROM(unsigned char addr, unsigned char data)
{
	unsigned char i;
	BOOL store_int;

	for(i=0;i<5;i++)
	{
		EEADR = (addr * 5) + i;	// Location
		EEDATA = data;			// Data
		EECON1bits.EEPGD = 0;	// Ensure we point to data EEPROM
		EECON1bits.CFGS=0;		// Assure we point to data EEPROM
		store_int = INTCONbits.GIE;	// Store prior interrupt enable setting
		INTCONbits.GIE = 0;		// Disable interrupts
		EECON1bits.WREN = 1;	// EEPROM write routine
		EECON2 = 0x55;
		EECON2 = 0xaa;
		EECON1bits.WR = 1;
		while (!PIR2bits.EEIF)	// Wait for write complete flag
			;
		PIR2bits.EEIF = 0;		// Clear the flag for next use
		EECON1bits.WREN = 0; 	// Inhibit write, to protect agsinst spurious action
		INTCONbits.GIE = store_int;	// Re-instant prior interrupt enable setting
	}	
	// Read back check - on first data item (only) ////////////////////////////
	EEADR = (addr * 5);		// Location of 1st data item
	EECON1bits.EEPGD = 0;	// Ensure we point to data EEPROM
	EECON1bits.CFGS = 0;	// Assure we point to data EEPROM
	EECON1bits.RD = 1;		// EEPROM read routine
	i = EEDATA;				// Get Data

	EEADR = 200;	// Make sure address is left pointing to unimportant cell,
					// ... just in case sporadic/brown-out write occurs
	if(i == data)
		return i;			// Read-back test successful
	else
		return 0;			// Read-back test fail
}	// end WriteEEPROM()


/*// EOF calibrate.c ///////////////////////////////////////////////////////////////

