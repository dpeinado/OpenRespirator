///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// Filename:        controls.c
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.00
//
// Services the controls elements of user interface
//
///////////////////////////////////////////////////////////////////////////////

/// I N C L U D E S ///////////////////////////////////////////////////////////
#include <p18f26k20.h>
#include "setting.h"
#include "acquire.h"
#include "controller.h"

/// P R I V A T E  D E F I N I T I O N S //////////////////////////////////////
#define BPM_STEP_DETECT 8
const rom near int BPM_TABLE[126] =	// default is 'near' when in small memory model
   {0,8,16,24,33,41,49,57,65,73,81,90,98,106,114,122,130,138,147,
	155,163,171,179,187,195,204,212,220,228,236,244,252,261,269,
	277,285,293,301,309,318,326,334,342,350,358,366,375,383,391,
	399,407,415,423,432,440,448,456,464,472,480,489,497,505,513,
	521,529,538,546,554,562,570,578,586,595,603,611,619,627,635,
	643,652,660,668,676,684,692,700,709,717,725,733,741,749,757,
	766,774,782,790,798,806,814,823,831,839,847,855,863,871,880,
	888,896,904,912,920,928,937,945,953,961,969,977,985,994,1002,
	1010,1018};

#define TI_STEP_DETECT 34
const rom near int TI_TABLE[31] =
   {0,34,68,102,135,169,203,237,271,305,338,372,406,440,474,508,
	541,575,609,643,677,711,744,778,812,846,880,914,947,981,1015};
   
#define PEEP_STEP_DETECT 65
const rom near int PEEP_TABLE[16] =
	{0,81,146,212,278,343,409,475,540,606,672,737,803,869,934,1000};

#define PIP_STEP_DETECT 25
const rom near int PIP_TABLE[41] =
	{0,35,60,85,110,135,160,185,210,235,260,285,310,335,360,385,
	410,435,460,485,510,535,560,585,610,635,660,685,710,735,760,
	785,810,835,860,885,910,935,960,985,1010};

/// V A R I A B L E S /////////////////////////////////////////////////////////
#pragma udata
t_setting setting;

/// P R I V A T E  P R O T O T Y P E S ////////////////////////////////////////

/// D E C L A R A T I O N S ///////////////////////////////////////////////////
#pragma code
///////////////////////////////////////////////////////////////////////////////
// Functions:       void ServiceSettings(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        Tests for and handles button press and dial encoder changes.
///////////////////////////////////////////////////////////////////////////////
void ServiceSettings(void)
{	
	// Test mode dial switch //////////////////////////////////////////////////
	setting.mode = MODE_STANDBY;		// Default mode
	if(!SWITCH_CPAP)
		setting.mode = MODE_CPAP;
	else if(!SWITCH_PSV)
			setting.mode = MODE_PSV;
		else if(!SWITCH_SIMV)
				setting.mode = MODE_SIMV;
			else if(!SWITCH_CMV)
					setting.mode = MODE_CMV;
	
	// Test for monitor values display button press ///////////////////////////
	if(!BUTTON_MONITOR)
		setting.monitor_values = TRUE;	// Latch up scroll flag. Clears itself at end

	// Test for inspiration hold button press /////////////////////////////////
	if(!BUTTON_INSP_HOLD)
		setting.inspiration_hold = TRUE;
	else
		setting.inspiration_hold = FALSE;
	
	// Test apnoea monitor on/off switch position /////////////////////////////
	if(SWITCH_APNOEA)
		setting.apnoea_monitor = TRUE;
	else
		setting.apnoea_monitor = FALSE;

	// Test soft/hard waveform switch position ////////////////////////////////
	if(SWITCH_WAVE)
		setting.soft_slope = TRUE;
	else
		setting.soft_slope = FALSE;

	// Test for BPM dial movement /////////////////////////////////////////////	
	if(pot_bpm > (BPM_TABLE[setting.bpm] + BPM_STEP_DETECT))
		setting.bpm++;
	else if(pot_bpm < (BPM_TABLE[setting.bpm] - BPM_STEP_DETECT))
		setting.bpm--;
	if(setting.bpm < 1) setting.bpm = 1;	// Lower range limit is 1 bpm

	// Test for Ti dial movement /////////////////////////////////////////////	
	if(pot_ti > (TI_TABLE[setting.ti] + TI_STEP_DETECT))
		setting.ti++;
	else if(pot_ti < (TI_TABLE[setting.ti] - TI_STEP_DETECT))
		setting.ti--;
	if(setting.ti < 2) setting.ti = 2;	// Lower range limit is 0.2 second
	
	if(((6000 / setting.bpm) - (setting.ti * 10)) < (setting.ti * 5))	// Limit I:E to 2:1
		setting.high_ie_flag = TRUE; //setting.ti  = 12000 / setting.bpm;
	else
		setting.high_ie_flag = FALSE;

	// Test for PEEP dial movement ////////////////////////////////////////////
	if(pot_peep > (PEEP_TABLE[setting.peep] + PEEP_STEP_DETECT))
		setting.peep++;
	else if(pot_peep < (PEEP_TABLE[setting.peep] - PEEP_STEP_DETECT))
		setting.peep--;
	
	// Test for PIP dial movement /////////////////////////////////////////////
	if(pot_pip > (PIP_TABLE[setting.pip] + PIP_STEP_DETECT))
		setting.pip++;
	else if(pot_pip < (PIP_TABLE[setting.pip] - PIP_STEP_DETECT))
		setting.pip--;

}	// end ServiceSettings()


///////////////////////////////////////////////////////////////////////////////
// Functions:       void InitSettings(void)
// PreCondition:    None
// Input:           None
// Output:          None
// Side Effects:    None
// Overview:        
///////////////////////////////////////////////////////////////////////////////
void InitSettings(void)
{	
	setting.monitor_values = 0;
	setting.inspiration_hold = 0;
	setting.apnoea_monitor = 0;
	setting.soft_slope = 0;
	setting.bpm = 0;
	setting.ti = 0;
	setting.peep = 0;
	setting.pip = 0;
}	// end InitSettings()


/*// EOF user.c ///////////////////////////////////////////////////////////////

