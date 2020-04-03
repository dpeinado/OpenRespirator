///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        display.c
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.00
//
// LCD handling routines
// 
///////////////////////////////////////////////////////////////////////////////

/// I N C L U D E S ///////////////////////////////////////////////////////////
#include <p18f26k20.h>
#include <stdlib.h>
#include <timers.h>
#include <string.h>
#include <delays.h>
#include "controller.h"
#include "monitor.h"
#include "display.h"
#include "setting.h"
#include "acquire.h"

/// P R I V A T E  D E F I N I T I O N S //////////////////////////////////////
#pragma romdata const_table	// default is 'near' when in small memory model
const rom near unsigned char LCD_MSG[8][21] =
   {"                    ",
	"  Test Ventilator   ",
	"    Non-Clinical    ",
	"  VENTILATION OFF   ",
	"Cycle Fail          ",
	"High Leak           ",
	"Apnoea              ",
	"I:E ratio above 2:1 "};

const rom near unsigned char SCROLL_MSG[78] =
	"BPM ..  IE .:..  Spont ..%  Pmin ..  Pmax ..  and some others to be defined  ";

/// P R I V A T E  V A R I A B L E S //////////////////////////////////////////


/// G L O B A L  V A R I A B L E S ////////////////////////////////////////////
#pragma udata
unsigned char bpm_str[4];		// 3 char + nil terminater
unsigned char i_time_str[4];
unsigned char peep_str[4];
unsigned char pip_str[4];
unsigned char p_mean_str[5];
/// redeclared as EXTERN in header file
int message;
unsigned char lcd_string[41];	// Also used by Calibrate()

/// P R I V A T E  P R O T O T Y P E S ////////////////////////////////////////
void Vars2Text(void);
void ScrollParameters(void);
void lcd_initialize(void);
void lcd_write_8(unsigned char instruction);
void lcd_write_4(unsigned char instruction);
void lcd_write_char(unsigned char chr);
void lcd_refresh(unsigned char* lcd_text);

/// D E C L A R A T I O N S ///////////////////////////////////////////////////
#pragma code
///////////////////////////////////////////////////////////////////////////////
// Functions:       void UpdateLCDText(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void UpdateLCDText(void)
{
	int i;
	int bar_p;
	
	///////////////////////////////////////////////////////////////////////////
	// Convert ventilation parameter values into ASCII test strings  //////////
	Vars2Text();
	
	///////////////////////////////////////////////////////////////////////////
	// LCD FIRST/TOP line /////////////////////////////////////////////////////
	if(setting.monitor_values)
	{
		ScrollParameters();
	}
	else
	{
		if(message == 0)
		{		// If YES, draw bar
			bar_p = (pressure / 20) + 3;
			// if remained, set MSB flick mode
			for(i=0;i<20;i++)			// Show bar graph
			{
				if(bar_p > i)
					lcd_string[i] = 0xFF;
				else
					lcd_string[i] = ' ';
			}
			
			if(monitor.trigger_flag)
				lcd_string[(monitor.p_trigger / 20) + 3] = '*';
		}
		else	// If NO show status message in top line
		{
			memcpypgm2ram(lcd_string,LCD_MSG[message],20);
		}
	}
	
	///////////////////////////////////////////////////////////////////////////
	// LCD SECOND/BOTTOM line /////////////////////////////////////////////////
	if(message == 1)
	{
		memcpypgm2ram(lcd_string + 20,LCD_MSG[2],20);
	}	
	else
	{
		lcd_string[20] = bpm_str[0];
		lcd_string[21] = bpm_str[1];
		lcd_string[22] = bpm_str[2];
		lcd_string[23] = ' ';
		lcd_string[24] = ' ';
		lcd_string[25] = i_time_str[0];
		lcd_string[26] = i_time_str[1];
		lcd_string[27] = i_time_str[2];
		lcd_string[28] = ' ';
		lcd_string[29] = peep_str[0];
		lcd_string[30] = peep_str[1];
		lcd_string[31] = ' ';
		lcd_string[32] = ' ';
		lcd_string[33] = pip_str[0];
		lcd_string[34] = pip_str[1];
		lcd_string[35] = ' ';
		lcd_string[36] = p_mean_str[0];
		lcd_string[37] = p_mean_str[1];
		lcd_string[38] = p_mean_str[2];
		lcd_string[39] = p_mean_str[3];
	}							// Text string is now prepared
	lcd_refresh(lcd_string);	// Call lcd write routine
}	// end UpdateLCDText()


///////////////////////////////////////////////////////////////////////////////
// Functions:       void Vars2Text(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        Convert values into lcd text strings
// Note:            
///////////////////////////////////////////////////////////////////////////////
void Vars2Text(void)
{
	///////////////////////////////////////////////////////////////////////////	
	// Convert bpm value to text //////////////////////////////////////////////
	if(setting.mode == MODE_CPAP)
	{
		bpm_str[2] = '-';
		bpm_str[1] = '-';
		bpm_str[0] = ' ';
	}
	else
	{
		itoa(setting.bpm,(char*)bpm_str);
		if(bpm_str[1] == 0)
		{
			bpm_str[2] = bpm_str[0];
			bpm_str[1] = ' ';
			bpm_str[0] = ' ';
		}
		else
			if(bpm_str[2] == 0)
			{
				bpm_str[2] = bpm_str[1];
				bpm_str[1] = bpm_str[0];
				bpm_str[0] = ' ';
			}
	}

	///////////////////////////////////////////////////////////////////////////
	// Convert i-Time value into text /////////////////////////////////////////
	if(setting.mode == MODE_CPAP)
	{
		i_time_str[2] = '-';
		i_time_str[1] = '-';
		i_time_str[0] = ' ';
	}
	else
	{	
		itoa(setting.ti,(char*)i_time_str);
		if(i_time_str[1] == 0)
		{
			i_time_str[2] = i_time_str[0];
			i_time_str[1] = '.';
			i_time_str[0] = '0';
		}
		else
		{
			i_time_str[2] = i_time_str[1];
			i_time_str[1] = '.';
		}
	}
	///////////////////////////////////////////////////////////////////////////
	// Convert PEEP value into text ///////////////////////////////////////////
	itoa(setting.peep,(char*)peep_str);
	if(peep_str[1] == 0)
	{
		peep_str[1] = peep_str[0];
		peep_str[0] = ' ';
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Convert PIP value into text ////////////////////////////////////////////
	if(setting.mode == MODE_CPAP)
	{
		pip_str[1] = '-';
		pip_str[0] = '-';
	}
	else
	{
		itoa(setting.pip,(char*)pip_str);
		if(pip_str[1] == 0)
		{
			pip_str[1] = pip_str[0];
			pip_str[0] = ' ';
		}
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Convert Pmean value into text //////////////////////////////////////////
	itoa(monitor.p_mean_display,(char*)p_mean_str);
	if(monitor.p_mean_display < -60)				// Lower range limit?
	{
		p_mean_str[3] = '!';
		p_mean_str[2] = '!';
		p_mean_str[1] = '!';
		p_mean_str[0] = '-';
	}
	else if(monitor.p_mean_display < 0)				// Negative value?
		if(p_mean_str[2] == 0)
		{
			p_mean_str[3] = p_mean_str[1];
			p_mean_str[2] = '.';
			p_mean_str[1] = '0';
			p_mean_str[0] = '-';
		}
		else
		{
			p_mean_str[3] = p_mean_str[2];
			p_mean_str[2] = '.';
			p_mean_str[0] = '-';
		}
	
	else if(monitor.p_mean_display < 100)		// Less than 10 mbar?
	{
		if(p_mean_str[1] == 0)
		{
			p_mean_str[3] = p_mean_str[0];
			p_mean_str[2] = '.';
			p_mean_str[1] = '0';
			p_mean_str[0] = ' ';
		}
		else
		{
			p_mean_str[3] = p_mean_str[1];
			p_mean_str[2] = '.';
			p_mean_str[1] = p_mean_str[0];
			p_mean_str[0] = ' ';
		}
	}
	else						// More than 10 mbar
	{	
		p_mean_str[3] = p_mean_str[2];
		p_mean_str[2] = '.';
	}
}


/////////////////////////////////////////////////////////////////////////////
// Functions:       void ScrollParameters(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        Set up LCD top line with a scrolling text string.
//					Is called every 100mS
/////////////////////////////////////////////////////////////////////////////
void ScrollParameters(void)
{
	static int scroll_position = -1;
	static int scroll_timer;
	static unsigned char scroll_string[80];

	if(scroll_position == -1)	// Is this the first cycle?
	{							// If YES, set up the scroll string
		scroll_position = 0;
		scroll_timer = 30;		// First scroll in 3 sec
		memcpypgm2ram(scroll_string,SCROLL_MSG,78);
		scroll_string[11] = '1';
	}

	// Copy section of scroll string into top line of lcd text
	memcpy(lcd_string,scroll_string + scroll_position,20);	

	// Advance to next scroll position
	if(scroll_timer < 1)
	{
		scroll_timer = 2;		// Scroll again in 0.2 sec
		scroll_position++;
		if(scroll_position > 57)
		{
			scroll_position = -1;
			setting.monitor_values = FALSE;
		}
	}
	else
	{
		scroll_timer--;		// 
	}
}

///////////////////////////////////////////////////////////////////////////////
// Functions:       void InitDisplay(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void InitDisplay(void)
{	
	lcd_initialize();
	message = 1;					// Splash screen
	UpdateLCDText();
}	// end InitDisplay()


/////////////////////////////////////////////////////////////////////////////
// Functions:       void lcd_initialize(void)
//					void lcd_write_8(unsigned char instruction)
//					void lcd_write(unsigned char instruction)
//					void lcd_write_char(unsigned char chr)
//					void lcd_refresh(unsigned char* lcd_text)
// PreCondition:    None
// Input:           lcd_refresh takes pointer to 40 character text string
// Output:          None
// Side Effects:    None
// Overview:        Collection of routines for writing to lcd display
// Note:			Controller is ....
/////////////////////////////////////////////////////////////////////////////
void lcd_initialize(void)
{
	Delay10KTCYx(20);		// Wait 50mS
	lcd_write_8(0b00110000);// Initialise LCD, 8-bit instruction 
	Delay10KTCYx(2);		// Wait 5mS
	lcd_write_8(0b00110000);// Initialise LCD again, 8-bit instruction 
	Delay10TCYx(50);		// Wait 125uS
	lcd_write_8(0b00110000);// Initialise LCD again, 8-bit instruction
	Delay10KTCYx(2);		// Wait 5mS
	lcd_write_8(0b00100000);// Set 4-bit mode
	Delay10KTCYx(1);		// Wait 2mS
	
	// Now in 4-bit mode
	lcd_write_4(0b00101000);// In 4-bit mode, Set 2-lines, 5x8 font
	Delay10KTCYx(1);		// Wait 2mS
	lcd_write_4(0b00001000);// Display off
	Delay10KTCYx(1);		// Wait 2mS
	lcd_write_4(0b00000001);// Clear display
	Delay10KTCYx(1);		// Wait 2mS
	lcd_write_4(0b00000110);// Set entry mode increments on write, no shift
	Delay10KTCYx(1);		// Wait 2mS
	
	lcd_write_4(0b00001100);// Display on; cursor off, blink off
	Delay10KTCYx(1);		// Wait 2mS
}
/////////////////////////////////////////////////////////////////////////////
void lcd_write_8(unsigned char instruction)
{
	LCD_DATA = (instruction >> 2) & 0x3C; // Place data. Only 4 MSB are used
	LCD_RS = 0;							// Prepare LCD for receiving instruction
	LCD_E  = 1;							// Enable read
	Delay10TCYx(4);						// Wait 10uS
	LCD_E  = 0;							// Latch data
}
/////////////////////////////////////////////////////////////////////////////
void lcd_write_4(unsigned char instruction)
{
	LCD_DATA = (instruction >> 2) & 0x3C; // Place data
	LCD_RS = 0;							// Prepare LCD for receiving instruction
	LCD_E  = 1;							// Enable read
	Delay10TCYx(4);						// Wait 10uS
	LCD_E  = 0;							// Latch data
	Nop();								// Allow port to settle
	LCD_DATA = (instruction << 2) & 0x3C; // Place data low nibble
	LCD_RS = 0;							// Prepare LCD for receiving instruction
	LCD_E  = 1;							// Enable read
	Delay10TCYx(4);						// Wait 10uS
	LCD_E  = 0;							// Latch data
}
/////////////////////////////////////////////////////////////////////////////
void lcd_write_char(unsigned char chr)
{
	LCD_DATA = (chr >> 2) & 0x3C;		// Place data high nibble
	LCD_RS = 1;							// Prepare LCD for receiving character
	LCD_E  = 1;							// Enable read
	Delay10TCYx(2);						// Wait 5uS
	LCD_E  = 0;							// Latch data
	Nop();								// Allow port to settle
	LCD_DATA = (chr << 2) & 0x3C;		// Place data low nibble
	LCD_RS = 1;
	LCD_E  = 1;							// Enable read
	Delay10TCYx(2);						// Wait 5uS
	LCD_E  = 0;							// Latch data
	Delay10TCYx(15);					// Wait 38uS
}
/////////////////////////////////////////////////////////////////////////////
void lcd_refresh(unsigned char* lcd_text)
{
	int	i;
	
	for(i=0;i<20;i++)		 // Write first 20 characters into top line
		lcd_write_char(lcd_text[i]);
	for(i=0;i<20;i++)
		lcd_write_char(' '); // Write through to next line (2x40 display driver)
	for(i=20;i<40;i++)		 // Write next 20 characters into bottom line
		lcd_write_char(lcd_text[i]);

	lcd_write_4(0b00000010); // Return home: line 1, position 0, for next call;
							 // there will be >2ms to settle, before next call
}	// end lcd display routines

/*// EOF display.c ////////////////////////////////////////////////////////////

*/
