// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA.
//
// Author: David Ortiz
//  This file is part of project: OpenRespirator
//

#include "i2c2_mux.h"
#include "mcc_generated_files/mcc.h"
#include "ORespGlobal.h"

uint8_t      currentTrfAddr;
i2c2_error_t lastI2C2MTrfResponse;
i2c2_error_t lastI2C2LTrfResponse;
i2c2_error_t lastI2C2MAckResponse;
i2c2_error_t lastI2C2LAckResponse;

i2c2_operations_t I2C2_NackCb(void *ptr){
    DEBUG_PRINT(("I2C2 E\n"));
    if (currentTrfAddr == MON_ADDR) {
        lastI2C2MAckResponse = false;
    } else {
        lastI2C2LAckResponse = false;        
    }
    return I2C2_CallbackReturnStop(NULL);
}

void I2C2_MuxInit(void){
    lastI2C2MAckResponse = true;
    lastI2C2LAckResponse = true;
    lastI2C2MTrfResponse = I2C2_NOERR;
    lastI2C2LTrfResponse = I2C2_NOERR;
    currentTrfAddr = 0x0;
}

bool I2C2_MAck(void){
    return lastI2C2MAckResponse;
}
bool I2C2_LAck(void){
    return lastI2C2LAckResponse;
}


i2c2_error_t I2C2_MOpen(void){
    i2c2_error_t trfRsp;

    trfRsp = I2C2_Open(MON_ADDR);
    if (trfRsp != I2C2_BUSY) {
        I2C2_SetAddressNackCallback(I2C2_NackCb,NULL);
        I2C2_SetDataNackCallback(I2C2_NackCb,NULL);

        lastI2C2MAckResponse = true;
        // Assign response to correct slave.
        if (currentTrfAddr == MON_ADDR){
            lastI2C2MTrfResponse = trfRsp;
        } else {
            lastI2C2LTrfResponse = trfRsp;
        }
        currentTrfAddr = MON_ADDR;
        lastI2C2MAckResponse = true;
        // Return last response for Monitor.
        return lastI2C2MTrfResponse;
    } 
    return I2C2_BUSY;
}

i2c2_error_t I2C2_LOpen(void){
    i2c2_error_t trfRsp;

    trfRsp = I2C2_Open(LCD_ADDR);
    if (trfRsp != I2C2_BUSY) {
        I2C2_SetAddressNackCallback(I2C2_NackCb,NULL);
        I2C2_SetDataNackCallback(I2C2_NackCb,NULL);

        lastI2C2LAckResponse = true;
        // Assign response to correct slave.
        if (currentTrfAddr == MON_ADDR){
            lastI2C2MTrfResponse = trfRsp;
        } else {
            lastI2C2LTrfResponse = trfRsp;
        }
        currentTrfAddr = LCD_ADDR;
        lastI2C2LAckResponse = true;
        // Return last response for LCD.
        return lastI2C2LTrfResponse;
    }
    return I2C2_BUSY;
}

i2c2_error_t I2C2_MClose(void){
    i2c2_error_t trfRsp;

    trfRsp = I2C2_Close();
    if (trfRsp != I2C2_BUSY) {
        // Assign response to correct slave.
        if (currentTrfAddr == MON_ADDR){
            lastI2C2MTrfResponse = trfRsp;
        } else {
            lastI2C2LTrfResponse = trfRsp;
        }
        // Return last response for LCD.
        return lastI2C2MTrfResponse;    
    }
    return I2C2_BUSY;
}

i2c2_error_t I2C2_LClose(void){
    i2c2_error_t trfRsp;

    trfRsp = I2C2_Close();
    if (trfRsp != I2C2_BUSY) {
        // Assign response to correct slave.
        if (currentTrfAddr == MON_ADDR){
            lastI2C2MTrfResponse = trfRsp;
        } else {
            lastI2C2LTrfResponse = trfRsp;
        }
        // Return last response for LCD.
        return lastI2C2LTrfResponse;    
    }
    return I2C2_BUSY;
}
