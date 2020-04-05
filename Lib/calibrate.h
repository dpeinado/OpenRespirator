///////////////////////////////////////////////////////////////////////////////
//     Primitive Test Ventilator
///////////////////////////////////////////////////////////////////////////////
// FileName:        calibrate.h
// Dependencies:    See INCLUDES section below
// Processor:       PIC18
// Compiler:        C18 3.4
///////////////////////////////////////////////////////////////////////////////

#ifndef CALIBRATE_H
#define CALIBRATE_H

/// I N C L U D E S ///////////////////////////////////////////////////////////

/// D E F I N I T I O N S /////////////////////////////////////////////////////

/// S T R U C T U R E S ///////////////////////////////////////////////////////

/// E X T E R N S /////////////////////////////////////////////////////////////
extern int p_offset;
extern int p_10range;
extern int trigger_sens;

/// P U B L I C  P R O T O T Y P E S //////////////////////////////////////////
void Calibrate(void);
void GetCalValues(void);

#endif //CALIBRATE_H
