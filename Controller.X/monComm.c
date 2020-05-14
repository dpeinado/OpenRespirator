/* 
 * File:   monComm.c
 * Author: David Ortiz
 *
 */

#include "monComm.h"
#include "ORespGlobal.h"
#include "i2c2_mux.h"
#include "menu.h"
#include "brate.h"

time_t    monTstamp;
i2c2_error_t lastI2CMonTrfResponse;

void MonitorInit(void){
    ;
}
void MonitorErrorSet(monErrorT flag){
    ctrlErrorStatus = ctrlErrorStatus|flag;
}

void MonitorErrorClr(monErrorT flag){
    ctrlErrorStatus = ctrlErrorStatus&(~flag);    
}

bool MonitorMsgBusy(void){
    return (I2C2_MClose() == I2C2_BUSY);
}

void MonitorMsgForcedSend (monStateT state){
    i2c2_error_t trfError;
    bool trfAck;
    int16_t vddVal;

    monTstamp = timeGet();
    // Assemble message for monitor and sent it.
    // Ensure previous message was sent.
    trfError = I2C2_MClose();
    trfAck = I2C2_MAck();

    if (!trfAck) {
        // Enable buzzer and display error message on second line.
        sprintf(lcdBtnRRow, " M.ERR");
        lcdPrintBRR = true;
        BUZZER_ON;
        lastI2CMonTrfResponse = true;
        // DEBUG_PRINT(("MON ERROR\n"));
    } else if ((BUZZERISON) && trfAck) {
        // Disable buzzer.
        sprintf(lcdBtnRRow, "      ");
        lcdPrintBRR = true;
        BUZZER_OFF;
        lastI2CMonTrfResponse = true;
        // DEBUG_PRINT(("MON ERROR CLR\n"));
    }
    
    if (trfError != I2C2_BUSY) {
        // First set VDD error flag. No more than 150mV deviation with respect to calibration value.
        aCaptGetResult(VddSensor, &vddVal);
        if (vddVal > vddValMean){
            vddVal = vddVal - vddValMean;
        } else {
            vddVal = vddValMean - vddVal;
        }
        if (vddVal > 150) {
            MonitorErrorSet(MON_VDDE);
        } else {
            MonitorErrorClr(MON_VDDE);        
        }
        
        // Now build message.
        if (VentMode == VMODE_VOLUME) {
            monitorMsg[MONIDX_MODE] = state|0x4;
        } else {
            monitorMsg[MONIDX_MODE] = state;
        }

        monitorMsg[MONIDX_IPV] = IP;    
        if (chIP) {
            monitorMsg[MONIDX_IPV] |= 0x80;    
        }
        
        monitorMsg[MONIDX_EPV] = PEEP;
        if (chPEEP) {
            monitorMsg[MONIDX_EPV] |= 0x80;
        }
        
        monitorMsg[MONIDX_BPMV]  = BPM;
        if (chBPM) {
            monitorMsg[MONIDX_BPMV] |= 0x80;
        }
        
        monitorMsg[MONIDX_PMAXV] = MaxP;
        if (chMaxP){
            monitorMsg[MONIDX_BPMV] |= 0x80;
        }

        monitorMsg[MONIDX_VMAXV] = MaxV>>1;
        if (chMaxV){
            monitorMsg[MONIDX_VMAXV] |= 0x80;
        }
        
        monitorMsg[MONIDX_LVAV]  = LowVAlarm>>1;
        if (chLowVAlarm){
            monitorMsg[MONIDX_LVAV] |= 0x80;
        }
        
        monitorMsg[MONIDX_HVAV]  = HighVAlarm>>1;
        if (chHighVAlarm){
            monitorMsg[MONIDX_HVAV]  |= 0x80;
        }
        
        monitorMsg[MONIDX_BRRTV] = bRateGet();
        monitorMsg[MONIDX_ALARMV] = ctrlErrorStatus;

        trfError = I2C2_MOpen();
        I2C2_SetBuffer(monitorMsg, 10);
        trfError = I2C2_MasterOperation(false);
    } else {
        // TODO: in final implementation, if I2C busy generate an error.
        // Until then, not possible.
    }
}

void MonitorMsgSend(monStateT state) {
    if (timeElapsedR(&monTstamp, TIME_MS(50))) {
        MonitorMsgForcedSend(state);
    }
}

// Blocking message send, returning ack of transfer.
bool MonitorMsgSendBlock(monStateT state) {
    while (MonitorMsgBusy());
    MonitorMsgForcedSend(state);
    while (MonitorMsgBusy());
#if FORCEMONOK
    return true;
#else
    return I2C2_MAck(); 
#endif
}
