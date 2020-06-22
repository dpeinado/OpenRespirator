/* 
 * File:   i2c2_mux.h
 * Author: David Ortiz
 *
 * Created on April 21, 2020, 8:15 PM
 */

#ifndef I2C2_MUX_H
#define	I2C2_MUX_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "mcc_generated_files/mcc.h"

// Answer to last I2C2 LCD and Monitor transfers.
extern uint8_t      currentTrfAddr;
extern i2c2_error_t lastI2C2MTrfResponse;
extern i2c2_error_t lastI2C2LTrfResponse;

#define LCD_ADDR 0x27
#define MON_ADDR 0x50

// Wrappers to keep minimum control of transfers, even if not fully enclosed.
// To allow muxing, all open commands to LCD and Monitor should be done using functions below.
// Access to Close function return result from last transfer or each.
void I2C2_MuxInit(void);
bool I2C2_MAck(void);
bool I2C2_LAck(void);
i2c2_error_t I2C2_MOpen(void);
i2c2_error_t I2C2_LOpen(void);
i2c2_error_t I2C2_MClose(void);
i2c2_error_t I2C2_LClose(void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C2_MUX_H */

