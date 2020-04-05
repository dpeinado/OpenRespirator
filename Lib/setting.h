///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        setting.h
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.00
///////////////////////////////////////////////////////////////////////////////

#ifndef SETTING_H
#define SETTING_H

/// I N C L U D E S ///////////////////////////////////////////////////////////

/// D E F I N I T I O N S /////////////////////////////////////////////////////
#define BUTTON_MONITOR		PORTAbits.RA4
#define SWITCH_APNOEA		PORTAbits.RA6
#define SWITCH_WAVE			PORTAbits.RA7
#define BUTTON_INSP_HOLD	PORTCbits.RC0
#define SWITCH_STANDBY		PORTCbits.RC3
#define SWITCH_CPAP			PORTCbits.RC4
#define SWITCH_PSV			PORTCbits.RC5
#define SWITCH_SIMV			PORTCbits.RC6
#define SWITCH_CMV			PORTCbits.RC7
#define MODE_STANDBY		0
#define MODE_CPAP			1
#define MODE_PSV			2
#define MODE_SIMV			3
#define MODE_CMV			4

/// S T R U C T U R E S ///////////////////////////////////////////////////////
typedef struct t_setting
{
	int mode;
	int monitor_values;
	int inspiration_hold;
	int apnoea_monitor;
	int soft_slope;
	int bpm;
	int ti;
	int peep;
	int pip;
	int high_ie_flag;
} t_setting;

/// E X T E R N S /////////////////////////////////////////////////////////////
extern t_setting setting;
/*
extern int mode;
extern int bpm;
extern int ti;
extern int peep;
extern int pip;
extern int apnoea_monitor;
extern int scroll_monitor;
extern int soft_slope;
extern int inspiration_hold;
*/
/// P U B L I C  P R O T O T Y P E S //////////////////////////////////////////
void ServiceSettings(void);
void InitSettings(void);

#endif //SETTING_H

