/**
 Author: David Ortiz
 */

#include "mcc_generated_files/mcc.h"
#include "ORespGlobal.h"
#include "aCapture.h"
#include "time.h"
#include "cmath.h"
#include "vMeasure.h"
#include "keyRead.h"
#include "LiquidCrystal_I2C.h"
#include "menu.h"
#include "i2c2_mux.h"

#define FORCEMONOK 0

#ifdef DEBUG

#define DBGPCONVERT(pValue) ((int16_t) (((2560/MPRESSURE_MBAR(1))*((int24_t) pValue))>>8))

void putch(char byte) {
    while (!UART1_is_tx_ready());
    UART1_Write(byte);
}
#endif


// To avoid problems with control, keep a copy of the values during first part of inspiration and expiration.
int16_t intIP, intMaxP, intPEEP, intIDuration, intEDuration, intMaxV;
vmodeT intVentMode; 

///////////////////
// User parameters.
///////////////////
vmodeT   VentMode = 0; // 0 --> Pressure-control, 1 --> Volume-control.
uint8_t  MaxP = 6;
// Volume is in units of 10ml.
uint8_t  MaxV = 16;
uint8_t  LowVAlarm = 10;
uint8_t  HighVAlarm = 22;
uint8_t BPM = 10;
uint16_t IDuration, EDuration;
uint8_t IP = 4;
uint8_t PEEP = 4;
bool chBPM, chIP, chMaxP, chPEEP, chLowVAlarm, chHighVAlarm, chMaxV, chMaxP, chVentMode;
int16_t  vddValMean;
uint8_t  eBRate;
uint16_t sv2_pwmval;

uint24_t bRateHist;
uint24_t bRatePtr;
uint8_t  bRateCnt = 0;

// CONTROLLER INTERNAL PARAMETERS.
#define BLED_ONTIME TIME_MS(500)
// DO NOT CHANGE. NEEDED TO ENSURE STABILITY.
#define PEEPCTRLMIN   MPRESSURE_MBAR(1)

#define BDTECT_THRL   MPRESSURE_MBAR(0.5)

#define PIPLATEAUMDEL TIME_MS(150)
#define PEPLATEAUMDEL TIME_MS(350)
#define SV2OTIME TIME_MS(15)
#define PRINTTIME TIME_MS(1)
///////////////////
///////////////////

// pAvgShort assumed to have 16ms tau.
#define PAVGSHORTTAU TIME_MS(20)

#if 0
// Not needed in the end implementation.
inline int16_t rPressurePredict(time_t delay, int16_t pInst, int16_t pAvgShort) {
    int32_t intLVal;

    // Scale delay.
    intLVal = (delay * 256) / (PAVGSHORTTAU);
    intLVal = (intLVal * (pInst - pAvgShort)) / 256;
    return pInst + ((uint16_t) intLVal);
}
#endif

ctrlStatusT ctrlStatus;
uint16_t lastCycleVol;
uint16_t freeFlowRateF, freeFlowRateM, freeFlowRateL;

// FUNCTIONS TO COMMUNICATE WITH MONITOR.
// Message to sent to monitor.
#define MONIDX_MODE   0
#define MONIDX_IPV    1
#define MONIDX_EPV    2
#define MONIDX_BPMV   3
#define MONIDX_PMAXV  4
#define MONIDX_VMAXV  5
#define MONIDX_LVAV   6
#define MONIDX_HVAV   7
#define MONIDX_BRRTV  8
#define MONIDX_ALARMV 9

char monitorMsg[10];
char ctrlErrorStatus;

typedef enum {
    MON_VDDE= 0x80,
    MON_SV3E= 0x40,
    MON_IPE = 0x20,
    MON_EPE = 0x10,
    MON_SV2E= 0x08
} monErrorT;

typedef enum {
    MONSTATE_INIT   = 0x20,
    MONSTATE_CALP   = 0x40,
    MONSTATE_SV2CHK = 0x60,
    MONSTATE_SV1CHK = 0x80,
    MONSTATE_CALF   = 0xA0,

    MONSTATE_STOP = 0x00,
    MONSTATE_RUNP = 0x10,
    MONSTATE_RUNV = 0x14,
    MONSTATE_SLEEP = 0x8
} monStateT;

monStateT monState;

monErrorT monError;
time_t    monTstamp;
i2c2_error_t lastI2CMonTrfResponse;

// Keep Rate in last 20 inspirations.
void bRateInit(void) {
    bRateHist = 0;
    bRatePtr = 1;
    eBRate = 0;
    bRateCnt = 0;
}

void bRateUpdt(bool triggered) {
    bRatePtr = bRatePtr << 1;
    bRateCnt++;
    if (bRateCnt > 20){
        bRateCnt = 20;
    }
    if (bRatePtr == (0x1L << 20)) {
        bRatePtr = 1;
    }

    if (bRateHist & bRatePtr) {
        eBRate -= 5;
    }
    if (triggered) {
        eBRate += 5;
        bRateHist |= bRatePtr;
    } else {
        bRateHist &= ~bRatePtr;
    }
}

uint8_t bRateGet(void) {
    return eBRate*20/bRateCnt;
}

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

bool pressureSensorChk(bool offsetCal, uint16_t time) {
    int16_t mPVal, mPValMax, mPValMin;
    int32_t mPValMean, aPValMean;
    int16_t aPVal, aPValMax, aPValMin;
    time_t tstamp;
    bool chkResult;

    chkResult = true;

    // Now capture data.
    mPValMin = 4096;
    aPValMin = 4096;
    mPValMax = 0;
    aPValMax = 0;
    tstamp = timeGet();
    
    while (!timeElapsedR(&tstamp, TIME_MS(time))) {
        if (aCaptGetResult(MainPSensor, &mPVal)) {
            if (mPVal > mPValMax) {
                mPValMax = mPVal;
            }
            if (mPVal < mPValMin) {
                mPValMin = mPVal;
            }
            mPValMean = (31*mPValMean + 32*mPVal)>>5;
        } else {
            ERROR_CONDITION(10);
            chkResult = false;
        }

        if (aCaptGetResult(VolPSensor, &aPVal)) {
            if (aPVal > aPValMax) {
                aPValMax = aPVal;
            }
            if (aPVal < aPValMin) {
                aPValMin = aPVal;
            }
            aPValMean = (31*aPValMean + 32*aPVal)>>5;
        } else {
            chkResult = false;
            ERROR_CONDITION(10);
        }
    }

    // Cal mean values.
    mPValMean = mPValMean>>5;
    aPValMean = aPValMean>>5;
    sprintf(lcdTopRow, "MS: % 3d AS: % 3d ", mPValMean, aPValMean);
    DEBUG_PRINT(("MAIN PRESSURE. Min %d Max %d MEAN %d\n", mPValMin, mPValMax, mPValMean));
    DEBUG_PRINT(("AUX PRESSURE. Min %d Max %d MEAN %d\n", aPValMin, aPValMax, aPValMean));
    setCursor(0, 0);
    printstrblock(lcdTopRow);
    timeDelayMs(1000);

    // Now check pressure sensors working correctly.
    if (offsetCal) {
        if (((aPValMax - aPValMin) > 16) || ((mPValMax - mPValMin) > 16) || (mPValMean > 350) || (mPValMean < 40) || (aPValMean < 600) || (aPValMean > 1030)) {
            // Too noisy, or values outside expected range.
            setCursor(0, 0);
            chkResult = false;
            DEBUG_PRINT(("PRESSURE CALIBRATION ERROR\n"));
        } else {
            // Set cal values.
            aCaptureOffSet(MainPSensor, mPValMean);
            aCaptureOffSet(VolPSensor, aPValMean);
            // Reset filters.
            aCaptRstFlt(Flt0PSensor);
            aCaptRstFlt(Flt1PSensor);
            aCaptRstFlt(Flt2PSensor);
            aCaptRstFlt(Flt3PSensor);
            timeDelayMs(50); // To ensure prev data flushed.
        }
    } else {
        if (((aPValMax - aPValMin) > 16) || ((mPValMax - mPValMin) > 16) || (mPValMean > 10) || (mPValMean < -10) || (aPValMean < -10) || (aPValMean > 10)) {
            DEBUG_PRINT(("VALVE CHECK ERROR\n"));
            chkResult = false;
        }
    }
    return chkResult;
}

// Initialization procedure.
// Self-test.
bool InitProcedure(void) {
    int16_t vddVal, vddValMax, vddValMin;
    int16_t aPVal, aPValMax, aPValMin, aPValMean;
    bool lcdBLight;
    time_t tstamp;
    bool initOk;

    ctrlStatus = CTRL_UNCAL;
    CLOSE_SV2;
    CLOSE_SV3;

    setCursor(0, 0);
    printstrblock("SELF-TEST.      ");
    setCursor(0, 1);
    printstrblock("AIRWAY OPEN     ");
    timeDelayMs(1000);

    vddValMin = 8000;
    vddValMax = 0;
    initOk = true;

    if (!MonitorMsgSendBlock(MONSTATE_INIT)) {
        initOk = false;
        DEBUG_PRINT(("Mon error"));
        setCursor(0, 0);
        printstrblock("MONITOR ERROR");
        timeDelayMs(500);
    }
    
    aCaptureOffSet(MainPSensor, 0);
    aCaptureOffSet(VolPSensor, 0);
    // First check VDD stability.
    tstamp = timeGet();

    while (!timeElapsedR(&tstamp, TIME_MS(500))) {
        if (aCaptGetResult(VddSensor, &vddVal)) {
            // DEBUG_PRINT(("VDD %d\n", vddVal));
            if (vddVal > vddValMax) {
                vddValMax = vddVal;
            }
            if (vddVal < vddValMin) {
                vddValMin = vddVal;
            }
        } else {
            ERROR_CONDITION(10);
            initOk = false;
        }
    }

    if (!initOk) {
        return initOk;
    }

    // Now check within limits.
    vddValMean = (vddValMax + vddValMin) >> 1;
    DEBUG_PRINT(("VDD Min %d Max %d MEAN %d\n", vddValMin, vddValMax, vddValMean));

    if ((vddValMean > 5250) || (vddValMean < 4750)) {
        // Print error message and exit.
        setCursor(0, 0);
        printstrblock("INTERNAL ERROR  ");
        setCursor(0, 1);
        sprintf(lcdTopRow, "VDD OOL: % 3d mV", vddValMean);
        printstrblock(lcdTopRow);
        initOk = false;
    } else if ((vddValMax - vddValMin) > 100) {
        // Print error message and exit.
        setCursor(0, 0);
        printstrblock("INTERNAL ERROR  ");
        setCursor(0, 1);
        printstrblock("VDD NOISY       ");
        initOk = false;
    }

    if (!initOk) {
        return initOk;
    }

    while (ctrlStatus == CTRL_UNCAL) {
        BLED_OFF;
        // First, ensure no key pressed.
        while (keyPeek() != -1)
            DEBUG_PRINT(("KP %d\n", keyPeek()));
            ;
        keyReadInit();

        // Repeat until correct calibration.
        // Now calibrate.
        BLED_ON;
        setCursor(0, 0);
        printstrblock("CAL PRESS BREATH");
        tstamp = timeGet();
        lcdBLight = true;

        tstamp = timeGet();
        while (1) {
            if (timeElapsedR(&tstamp, TIME_MS(500))) {
                setBacklight(lcdBLight);
                lcdBLight = !lcdBLight;
            }
            // WAIT UNTIL BREATH BUTTON IS PRESSED DURING 2 SECONDS.
            // MEANWHILE BLINK LCD and KEEP BREATH PRESSED.
            if (keyReadEC() == ESCAPE_CODE) {
                if (keyPeek() == KEYBREATH) {
                    // Key Breath pressed.
                    setBacklight(true);
                    break;
                }
            }
        }

        BLED_OFF;
        DEBUG_PRINT(("CALP\n"));
        if (!MonitorMsgSendBlock(MONSTATE_CALP)){
            initOk=false;
            DEBUG_PRINT(("Mon error"));
            setCursor(0, 0);
            printstrblock("MONITOR ERROR");
            timeDelayMs(500);
        }

        if (pressureSensorChk(true, 500)) {
            // Success.
            ctrlStatus = CTRL_STOP;
        } else {
            printstrblock("CAL ERROR. RETRY ");
            timeDelayMs(500);
        }
    }
    
    if (!initOk) {
        return initOk;
    }

    // Two extra valve check steps. 
    DEBUG_PRINT(("CHKSV2\n"));
    // First CLOSE SV2, OPEN SV1, check no pressure, no volume detected.
    if (!MonitorMsgSendBlock(MONSTATE_SV2CHK)){
        initOk=false;
            DEBUG_PRINT(("Mon error"));
            setCursor(0, 0);
            printstrblock("MONITOR ERROR");
            timeDelayMs(500);
    }

    CLOSE_SV2;
    timeDelayMs(200);
    if (!pressureSensorChk(false, 100)) {
        initOk=false;
        printstrblock("SV2 Valve error");
        timeDelayMs(500);
    }
    
    // Then CLOSE SV1, OPEN SV2, check no pressure, no volume detected.
    DEBUG_PRINT(("CHKSV1\n"));
    if (!MonitorMsgSendBlock(MONSTATE_SV1CHK)){
        initOk=false;
    }
    timeDelayMs(100);
    OPEN_SV2;
    timeDelayMs(300);
    if (!pressureSensorChk(false, 100)) {
        initOk=false;
        printstrblock("SV1 Valve error");
        timeDelayMs(500);
    }
    
    // Last step, check volume. 
    DEBUG_PRINT(("CHK VOL\n"));
    tstamp = timeGet();

    // Open valve, wait for 250ms, measure flow during 250ms, display result.
    setCursor(0, 0);
    printstrblock("FLOW RATE        ");
    aPValMin = 4096;
    aPValMax = 0;

    if (!MonitorMsgSendBlock(MONSTATE_CALF)){
        initOk=false;
            DEBUG_PRINT(("Mon error"));
            setCursor(0, 0);
            printstrblock("MONITOR ERROR");
            timeDelayMs(500);
    }

    OPEN_SV2LOW;
    OPEN_SV3;
    timeDelayMs(250);
    vMeasureRst();
    tstamp = timeGet();
    while (!timeElapsedR(&tstamp, TIME_MS(250))) {
        aCaptGetResult(VolPSensor, &aPVal);
        if (aPVal > aPValMax) {
            aPValMax = aPVal;
        }
        if (aPVal < aPValMin) {
            aPValMin = aPVal;
        }
    }
    // Open Flow Rate in ml/sec. Do it in 250ms to avoid patient damage even if patient is connected.
    freeFlowRateL = vMeasureGet()<<2;
    if (!MonitorMsgSendBlock(MONSTATE_STOP)){
        initOk=false;
            DEBUG_PRINT(("Mon error"));
            setCursor(0, 0);
            printstrblock("MONITOR ERROR");
            timeDelayMs(500);
    }
    CLOSE_SV2;
    CLOSE_SV3;

    aPValMean=aCaptureOffGet(VolPSensor);
    
    if ((aPValMean + aPValMax)>= (0.95*8192)) {
        // Flow sensor almost saturated. Flow too high.
        setCursor(0, 0);
        printstrblock("FLOW TOO HIGH   ");
        timeDelayMs(1000);
        initOk = false;
    } else {
        sprintf(lcdTopRow, "FLOW: % 3d L/min ", (int16_t) ((uint32_t) ((uint32_t) 60 * freeFlowRateL) / 1000));
        setCursor(0, 0);
        printstrblock(lcdTopRow);
        timeDelayMs(4000);

        if (freeFlowRateL < 120) {
            // Outside limits.
            setCursor(0, 0);
            printstrblock("FLOW TOO LOW    ");
            timeDelayMs(1000);
            initOk = false;
        } else if (freeFlowRateL > 600) {
            // Outside limits.
            setCursor(0, 0);
            printstrblock("FLOW TOO HIGH   ");
            timeDelayMs(1000);
            initOk = false;
        }
    }




    OPEN_SV2MED;
    OPEN_SV3;
    timeDelayMs(250);
    vMeasureRst();
    tstamp = timeGet();
    while (!timeElapsedR(&tstamp, TIME_MS(250))) {
        aCaptGetResult(VolPSensor, &aPVal);
        if (aPVal > aPValMax) {
            aPValMax = aPVal;
        }
        if (aPVal < aPValMin) {
            aPValMin = aPVal;
        }
    }
    // Open Flow Rate in ml/sec. Do it in 250ms to avoid patient damage even if patient is connected.
    freeFlowRateM = vMeasureGet()<<2;
    if (!MonitorMsgSendBlock(MONSTATE_STOP)){
        initOk=false;
            DEBUG_PRINT(("Mon error"));
            setCursor(0, 0);
            printstrblock("MONITOR ERROR");
            timeDelayMs(500);
    }
    CLOSE_SV2;
    CLOSE_SV3;

    if ((aPValMean + aPValMax)>= (0.95*8192)) {
        // Flow sensor almost saturated. Flow too high.
        setCursor(0, 0);
        printstrblock("FLOW TOO HIGH   ");
        timeDelayMs(1000);
    } else {
        sprintf(lcdTopRow, "FLOW: % 3d L/min ", (int16_t) ((uint32_t) ((uint32_t) 60 * freeFlowRateM) / 1000));
        setCursor(0, 0);
        printstrblock(lcdTopRow);
        timeDelayMs(4000);

        if (freeFlowRateM < 200) {
            // Outside limits.
            setCursor(0, 0);
            printstrblock("FLOW TOO LOW    ");
            timeDelayMs(1000);
            initOk = false;
        } else if (freeFlowRateM > 2000) {
            // Outside limits.
            setCursor(0, 0);
            printstrblock("FLOW TOO HIGH   ");
            timeDelayMs(1000);
            initOk = false;
        }
    }
    




    OPEN_SV2;
    OPEN_SV3;
    timeDelayMs(250);
    vMeasureRst();
    tstamp = timeGet();
    while (!timeElapsedR(&tstamp, TIME_MS(250))) {
        aCaptGetResult(VolPSensor, &aPVal);
        if (aPVal > aPValMax) {
            aPValMax = aPVal;
        }
        if (aPVal < aPValMin) {
            aPValMin = aPVal;
        }
    }
    // Open Flow Rate in ml/sec. Do it in 250ms to avoid patient damage even if patient is connected.
    freeFlowRateF = vMeasureGet()<<2;
    if (!MonitorMsgSendBlock(MONSTATE_STOP)){
        initOk=false;
            DEBUG_PRINT(("Mon error"));
            setCursor(0, 0);
            printstrblock("MONITOR ERROR");
            timeDelayMs(500);
    }
    CLOSE_SV2;
    CLOSE_SV3;

    if ((aPValMean + aPValMax) >= (0.95*8192)) {
        // Flow sensor almost saturated. Flow too high.
        setCursor(0, 0);
        printstrblock("FLOW TOO HIGH   ");
        timeDelayMs(1000);
    } else {
        sprintf(lcdTopRow, "FLOW: % 3d L/min ", (int16_t) ((uint32_t) ((uint32_t) 60 * freeFlowRateF) / 1000));
        setCursor(0, 0);
        printstrblock(lcdTopRow);
        timeDelayMs(4000);

        if (freeFlowRateF < 300) {
            // Outside limits.
            setCursor(0, 0);
            printstrblock("FLOW TOO LOW    ");
            timeDelayMs(1000);
            initOk = false;
        } else if (freeFlowRateF > 2500) {
            // Outside limits.
            setCursor(0, 0);
            printstrblock("FLOW TOO HIGH   ");
            timeDelayMs(1000);
            initOk = false;
        }
    }
    
    
    // Wait until key depressed.
    while (keyPeek() != -1)
        ;
    keyReadInit();
        
    return initOk;
}

int16_t pInst, pAvgShort, pAvgUShort;
int16_t bdP1, bdP2;
// Pressure overshoot measurement variables. Both for inspiration and expiration, overshoot measures 
// the difference between the pressure estimation that is used to close the valve (pValveActuation), and the real 
// pressure reached some time after valve is closed (pPlateau).
int16_t pCtrl, pValveActuation, pPlatMax, pPlatInsp, pPlatExp, pPeepActual, pExpOS, pInspOS; // Variables for overshoot measurement.
// Volume overshoot measurement variables. Measure volume difference between valve close time and after valve actuation time.
// Best way to decide when to determine SV2 close time, even in pressure-mode is to measure the volume that produces the desired IP.
// Estimate lung compliance, multiplied by the desired IP it gives the estimated volume for it.
uint16_t lungC;
int16_t pInspOS3, pInspOS2; // Pressure overshoot during inhalation for each intermediate valve setting.
int16_t vValveActuation, vPlateau, vInspOS;
// Volume quanta. Volume injected on each fixed-time SV2 opening after initial subState.
int pInspVSet, pInspOSVSet;
int16_t vQuanta;
int16_t pQuantaInsp, pQuantaExp;
int16_t pAdj, vAdj;
uint16_t tInsp;
uint16_t effectiveFlowRate;
time_t rCycleTime, rValveActuationTstamp, rVHighActuationTstamp, rVMedActuationTstamp, rVLowActuationTstamp;
time_t rSV2ValveDelay, rSV3ValveDelay;
time_t rValveDelayStart, rTimeTmp;
bool initialSubState, valveDelayCheck, OSCheck, QuantaCheck;
bool OSCheckInt;

void inspOSMeasure(void) {
    uint16_t tmpUVal;
    int16_t tmpVal;

    // Valve setting when measuring OS. Normalize OS for both valves open (3x).
    // Then scale down in use for the actual setting. This produces no difference in constant conditions, but would enhance the behaviour in changing conditions.
    tmpVal = (3*(pPlatMax - pValveActuation))/pInspOSVSet;
    if (pInspOS == 0){
        pInspOS = (3*tmpVal)>>2;
    } else {
        pInspOS = (3 * pInspOS + tmpVal) >> 2;
    }
    if (vInspOS < 0) {
        // Volume overshoot should always be > 0.
        vInspOS = 0;
    }
    
    tmpVal = vMeasureGet();
    // Constant is volume at inspiration. Scale for precision.
    if ((pPlatInsp > MPRESSURE_MBAR(4)) && (tmpVal < 2047)) {
        tmpUVal = (pPlatInsp - pPeepActual) >> 1;
        tmpUVal = (((uint24_t) tmpVal) << 6) / tmpUVal;
        // Now IIR.
        if (lungC == 0){
            lungC = (3*tmpUVal)>>2;
        } else {
            lungC = (3 * lungC + tmpUVal) >> 2;
        }
    }

    tmpVal = (3*(tmpVal - vValveActuation))/pInspOSVSet;
    if (vInspOS == 0){
        vInspOS = (3*tmpVal)>>2;
    } else {
        vInspOS = (3 * vInspOS + tmpVal) >> 2;
    }
    DEBUG_PRINT(("\nIPOS LC %d\n", lungC));
}

void main(void) {
    uint16_t tmpUVal;
    int16_t tmpVal;
    int16_t pInspOSScale, vInspOSScale;
#ifdef DEBUG
    time_t printTime;
#endif


    // Initialize the device
    SYSTEM_Initialize();
    // Disable ADC irq.
    PIE1bits.ADIE = 0;
    PIE1bits.ADTIE = 0;

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:
    INTERRUPT_GlobalInterruptEnable();
    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Initialize.
    aCaptureInit();
    timeInit();

    // First wait for startup. 50ms.
    timeDelayMs(50);

    keyReadInit();
    screenInit();
    I2C2_MuxInit();
    MonitorInit();
    vMeasureInit();
    
    while (0) {
        OPEN_SV3;
        OPEN_SV2;
        timeDelayMs(500);
        CLOSE_SV3;
        CLOSE_SV2;
        timeDelayMs(500);
    }
    while (1) {
        char keyTmp;
        while (!InitProcedure())
            ;

        setCursor(0, 0);
        printstrblock("PRESS + TO REPEAT ");
        setCursor(0, 1);
        printstrblock("PRESS - TO EXIT ");

        keyTmp = keyRead();
        while ((keyTmp != KEYMINUS)&&(keyTmp != KEYPLUS)) {
            keyTmp = keyRead();
            MonitorMsgSend(MONSTATE_STOP);
        }
        if (keyTmp == KEYMINUS) {
            break;
        }
    }

    // Clean screen lower line.
    setCursor(0, 1);
    printstrblock("                ");

    while (1) {
        setCursor(0, 1);
        printstrblock("                ");
        timeDelayMs(10);
        setBacklight(true);
        timeDelayMs(10);

        // Now wait until BREATH button pressed.
        lcdPrintTR = true;
        lcdPrintBR = true;
        BLED_ON;
        while (ctrlStatus != CTRL_RUN) {
            // User inputs, screen update.
            MenuMng();
            screenMng();
            MonitorMsgSend(MONSTATE_STOP);

            if (keyReadEC() == ESCAPE_CODE) {
                if (keyPeek() == KEYBREATH) {
                    // Key Breath pressed.
                    BLED_OFF;
                    ctrlStatus = CTRL_RUN;

                    // Wait until key depressed.
                    while (keyPeek() != -1)
                        ;
                    keyReadInit();
                    break;
                }
            }
        }

        // Wait until monitor free to receive new message.
        while(MonitorMsgBusy());
        
        // Init all control variables.
        rSV2ValveDelay = 20;
        rSV3ValveDelay = 40;
        valveDelayCheck = false;
        OSCheck = false;
        QuantaCheck = false;
        pExpOS = 0;
        pInspOS = 0;
        pInspOS3 = 0;
        pInspOS2 = 0;
        vInspOS = 0;
        vQuanta = 0;
        pQuantaInsp = 0;
        pQuantaExp = 0;
        lungC = 0;
        // Flow rate in ml/sec.
        effectiveFlowRate=freeFlowRateF;
        pPeepActual = 0;
        IDuration = ((uint16_t) 60 * 1000) / (3 * BPM);
        EDuration = ((uint16_t) 60 * 1000 / BPM) - IDuration;
        lastCycleVol = 0;
        bRateInit();

        // Start periodic message sent.
        monTstamp = timeGet();
        if (VentMode == VMODE_PRESSURE) {
            MonitorMsgForcedSend(MONSTATE_RUNP);
        } else {
            MonitorMsgForcedSend(MONSTATE_RUNV);
        }

        // Start.
        rCycleTime = timeGet();
        while (ctrlStatus != CTRL_SLEEP) {
            /////////////////////////////////////////////
            // Inspiration part of the cycle.
            /////////////////////////////////////////////
            // Now three different pressure settings are possible. Optimize time expended on each one.
            // Pressure due to lung compliance and lung resistance can be simplified as:
            // P(t) = LC*Integral(Flow(t)) + LR*Flow(t). Being LC the lung compliance, and LR the lung resistance.
            // With fixed flow(t) values, the integral of flow becomes F1*T1+F2*T2+F3*T3 being F1, F2, F3 the flow at the three settings, and T1, T2, T3 the total time at each setting.
            // To minimize the maximum pressure reached, this pressure should be reached the end of each stage (of filling the lung with one of the three settings)
            // If this is done, then T2 and T3 have fixed durations that can be expressed as T2=LR/2LC, T3=LR/LC. T1 would account for the rest of the time.
            // In this expression the time is unrestricted. If time must be minimized, above values would represent max values.
            DEBUG_PRINT(("\nIP\n"));
            intVentMode = VentMode;
            intMaxP = MPRESSURE_MBAR(MaxP);
            intPEEP = MPRESSURE_MBAR(PEEP);
            intIDuration = TIME_MS(IDuration);
            intEDuration = TIME_MS(EDuration);
            if (intVentMode == VMODE_PRESSURE) {
                intIP = MPRESSURE_MBAR(IP);
                // Get target volume to reach desired IP.
                intMaxV = ((((uint24_t) intIP-pPeepActual)*((uint24_t) lungC))>>7);
                // Check do not exceed max volume.
                if (intMaxV > (10*HighVAlarm)) {
                    intMaxV = 10*HighVAlarm;
                }
            } else {
                // In volume mode --> Target volume implies starting from the setted PEEP. If PEEP is higher --> lower the volume to avoid damage.
                intIP = 0;
                if (pPeepActual > ((MPRESSURE_MBAR(1) + intPEEP))) {
                    intMaxV = 10 * ((uint16_t) MaxV) - (((pPeepActual - intPEEP)*lungC)>>7);
                } else  {
                    intMaxV = 10 * ((uint16_t) MaxV);
                }
            }
            DEBUG_PRINT(("\nIP. MaxV %d EBRate %d\n", intMaxV, bRateGet()));
                        
            // Set valves initial value.
            if ((((uint24_t) 3000*intMaxV)/effectiveFlowRate) < INSP_TIME) {
                OPEN_SV2LOW;
                rVLowActuationTstamp = timeGet();
                rVMedActuationTstamp = rCycleTime;
                rVHighActuationTstamp = rCycleTime;
                pInspVSet = 1;
                DEBUG_PRINT(("\nVLow\n"));
            } else if ((((uint24_t) 1500*intMaxV)/effectiveFlowRate) < INSP_TIME) {
                OPEN_SV2MED;
                rVMedActuationTstamp = timeGet();
                rVHighActuationTstamp = rCycleTime;
                pInspVSet = 2;
                DEBUG_PRINT(("\nVMed\n"));
            } else {
                OPEN_SV2;
                pInspVSet = 3;
                rVHighActuationTstamp = timeGet();
            }            
            OPEN_SV3;

            // Init variables for new cycle.
            initialSubState = true;
            valveDelayCheck = true;
            OSCheck = false;
            OSCheckInt = false;
            QuantaCheck = false;
            rValveDelayStart = timeGet();
            vMeasureRst();
            while (1) {
                if (intVentMode == VMODE_PRESSURE) {
                    MonitorMsgSend(MONSTATE_RUNP);
                } else {
                    MonitorMsgSend(MONSTATE_RUNV);
                }

                if (timeElapsedR(&rCycleTime, intIDuration)) {
                    // Goto next.
                    if (initialSubState) {
                        // IP not reached.
                        MonitorErrorSet(MON_IPE);
                        // It is needed to do estimations at least of LungC.
                        pPlatInsp = pInst;
                        pValveActuation = pInst;
                        pPlatMax = pInst;
                        inspOSMeasure();
                    }
                    if (OSCheck) {
                        OSCheck = false;
                        aCaptGetResult(Flt1PSensor, &pAvgUShort);
                        pPlatInsp = pAvgUShort;
                        if (pPlatMax < pAvgUShort) {
                            pPlatMax = pAvgUShort;
                        }
                        inspOSMeasure();
                    }
                    break;
                } else {
                    //TODO
                    // When BPM is very high, IP time can be very short. 
                    // Ensure algorighm is stable in this conditions:
                    // If SV2 is closed before end of IP cycle, and there is time to measure OS, do it.
                    // Otherwise, ensure OS should not used in those conditions...
                    // Do the same in case of EP.
                    if (timeElapsed(rCycleTime, BLED_ONTIME)) {
                        // Turn off led. Can be done here because minimum IP time with fixed 1:2 ratio and 30 BPM is 0.66 seconds.
                        BLED_OFF;
                    }
                    if (initialSubState) {
                        // Inhale.
                        aCaptGetResult(MainPSensor, &pInst);

                        // Valve is closed if:
                        // Pressure goes above IP + Overshoot limit.
                        // Pressure estimated at valve close time, including estimated overshoot > IP + Overshoot limit.
                        // Estimated plateu pressure goes above IP.

                        // Pressure prediction computed by presPredict function.
                        aCaptGetResult(Flt1PSensor, &pAvgShort);
                        // Can assign variables here since there is no ongoing actuation.
                        // The lung will have an extra pressure during inspiration which is proportional to the flow rate,
                        // the higher the flow rate, the higher the extra pressure during inspiration. As in the EP phase, compensate this by using a filtered version of the pressure.
                        pCtrl = pAvgShort;
                        vValveActuation = vMeasureGet();

                        tmpVal = INSP_TIME - timeDiff(rCycleTime, timeGet());
                        
                        // Two reasons for switching to med flow:
                        //   Pressure limit.
                        //   Volume is very low, so possible to fill-in at lower rate.
                        if ((pInspVSet == 3) &&
                            (((pCtrl+pInspOS3) > ((7*intMaxP)>>3)) ||
                            ((((int24_t) 1500*(intMaxV-vValveActuation))/effectiveFlowRate) < tmpVal))) {
                            OPEN_SV2MED;
                            pInspVSet = 2;
                            OSCheckInt = true;
                            pPlatMax = pCtrl;
                            rVMedActuationTstamp = timeGet();
                            rValveActuationTstamp = rVMedActuationTstamp;
                            pValveActuation = pCtrl;
                            DEBUG_PRINT(("PI-MED T %5d - Pi %d Pc %d Vol %3d OS %d RF %d\n", timeDiff(rCycleTime, timeGet()), DBGPCONVERT(pInst), DBGPCONVERT(pCtrl), vValveActuation, DBGPCONVERT(pInspOS3), effectiveFlowRate));
                        } else if (!OSCheckInt && (pInspVSet == 2) &&
                                    (((pCtrl+pInspOS2) > ((7*intMaxP)>>3)) ||
                                    ((((int24_t) 3000*(intMaxV-vValveActuation))/effectiveFlowRate) < tmpVal))) {
                            OPEN_SV2LOW;
                            pInspVSet = 1;
                            OSCheckInt = true;
                            pPlatMax = pCtrl;
                            rVLowActuationTstamp = timeGet();
                            rValveActuationTstamp = rVLowActuationTstamp;                            
                            pValveActuation = pCtrl;                            
                            DEBUG_PRINT(("PI-LOW T %5d - Pi %d Pc %d Vol %3d OS %d\n", timeDiff(rCycleTime, timeGet()), DBGPCONVERT(pInst), DBGPCONVERT(pCtrl), vValveActuation, DBGPCONVERT(pInspOS2)));
                        }
                        
                        if (pPlatMax < pCtrl) {
                            pPlatMax = pCtrl;
                        }

                        if (OSCheckInt && timeElapsed(rValveActuationTstamp, rSV2ValveDelay<<2)) {
                            OSCheckInt=false;
                            if (pInspVSet == 2){
                                pInspOS3 = (3*pInspOS3 + pPlatMax - pValveActuation)>>2;
                            } else if (pInspVSet == 1) {
                                pInspOS2 = (3*pInspOS2 + pPlatMax - pValveActuation)>>2;                                
                            }
                        }
                        
                        if (OSCheckInt){
                            pInspOSScale = ((1+pInspVSet)*pInspOS)/3;
                            vInspOSScale = ((1+pInspVSet)*vInspOS)/3;                            
                        } else {
                            pInspOSScale = (pInspVSet*pInspOS)/3;
                            vInspOSScale = (pInspVSet*vInspOS)/3;
                        }
                        
                        if ((pInst > (intMaxP+MPRESSURE_MBAR(4))) ||
                                (pCtrl > intMaxP) ||
                                ((pCtrl + pInspOSScale) > intMaxP) ||
                                ((intVentMode == VMODE_PRESSURE) && ((pCtrl + pInspOSScale) > intIP) && (vValveActuation + vInspOSScale) >= intMaxV) ||
                                ((intVentMode == VMODE_VOLUME) && ((vValveActuation + vInspOSScale) >= intMaxV))) {
                            CLOSE_SV2;
                            pValveActuation = pCtrl;
                            initialSubState = false;
                            rValveActuationTstamp = timeGet();
                            tInsp = timeDiff(rCycleTime, timeGet());
                            // Here two possibilities exist:
                            // Do like in EP, calculate overshoot respect to instantaneous pressure, and apply to it.
                            // In this case, no need to compute pressure prediction.
                            // The other option is to compute OS over pressure prediction, and apply also over the prediction.
                            // The theoretical advantage of the second case is that the OS calculated will be smaller, so it will be less sensitive to the changing conditions.
                            OSCheck = true;
                            if (OSCheckInt) {
                                pInspOSVSet = pInspVSet+1;
                            } else {
                                pInspOSVSet = pInspVSet;
                            }
                            pPlatMax = pInst;
                            if ((pInst > intMaxP) || ((pCtrl + pInspOSScale) > intMaxP)) {
                                // Should never stop due to pressure, if this is the case --> increment the OS value of the valves.
                                if ((pInspVSet == 2) || (pInspVSet == 3)) {
                                    pInspOS3 += MPRESSURE_MBAR(5);
                                } else {
                                    // In this case two situations might appear: 
                                    // OS2 is too low, then solution is to increase it.
                                    // But it might also happen that the problem is that OS3 is closing too late, and there is no time to put SV2 in low flow.
                                    if (timeElapsed(rVMedActuationTstamp, rSV2ValveDelay<<3)) {
                                        pInspOS2 += MPRESSURE_MBAR(5);
                                    } else {
                                        pInspOS3 += MPRESSURE_MBAR(5);     
                                    }
                                }
                            }

                            if (pInspVSet == 3) {
                                tmpUVal = timeDiff(rVHighActuationTstamp, timeGet());    
                            } else if (pInspVSet == 2){
                                tmpUVal = timeDiff(rVHighActuationTstamp, rVMedActuationTstamp) + (2*timeDiff(rVMedActuationTstamp, timeGet()))/3;    
                            } else if (pInspVSet == 1){
                                tmpUVal = timeDiff(rVHighActuationTstamp, rVMedActuationTstamp) + (2*timeDiff(rVMedActuationTstamp, rVLowActuationTstamp))/3 + timeDiff(rVLowActuationTstamp, timeGet())/3;
                            }

                            if (tmpUVal >= 60) {
                                effectiveFlowRate=(3*effectiveFlowRate+(((uint24_t) 1000*vValveActuation)/tmpUVal))>>2;
                            }   
                            
                            // Avoid too big errors in flow measurement.
                            if (effectiveFlowRate < 100){
                                effectiveFlowRate = 100;
                            } else if (effectiveFlowRate > 2000){
                                effectiveFlowRate = 2000;
                            }
                            
                            DEBUG_PRINT(("PI-END T %d - Pi %d Pc %d Vol %3d PMax %d VL %d VOS %d POS %d FR %d UV %d\n",
                                    timeDiff(rCycleTime, timeGet()),
                                    DBGPCONVERT(pInst), DBGPCONVERT(pValveActuation),
                                    vValveActuation, DBGPCONVERT(intMaxP), intMaxV, vInspOS, DBGPCONVERT(pInspOSScale), effectiveFlowRate, tmpUVal));
                        }
                        if (valveDelayCheck) {
                            // Measure response time of valve.
                            // When detect a change of 1mBar on pInst-pAvgShort.
                            if (pInst > (pAvgShort + MPRESSURE_MBAR(1))) {
                                valveDelayCheck = false;
                                // Minimum reached. Store response time.
                                rTimeTmp = timeDiff(rValveDelayStart, timeGet());
                                if (rTimeTmp < TIME_MS(100)) {
                                    // If value within limits update value, if outside, keep previous value.
                                    rSV2ValveDelay = (rSV2ValveDelay + rTimeTmp) >> 1;
                                    // Clear alarm.
                                    MonitorErrorClr(MON_SV2E);
                                } else {
                                    // Raise alarm.
                                    MonitorErrorSet(MON_SV2E);
                                }
                            }
                        }
                    } else {
                        // Hold IP.
                        aCaptGetResult(MainPSensor, &pInst);
                        MonitorErrorClr(MON_IPE);
                        if (SV2ISOPEN && (timeElapsedR(&rValveActuationTstamp, SV2OTIME) || (pInst > intMaxP))) {
                            CLOSE_SV2;
                            rValveActuationTstamp = timeGet();
                            aCaptGetResult(MainPSensor, &pInst);
                            DEBUG_PRINT(("PI-VC T %d - Pi %d\n", timeDiff(rCycleTime, rValveActuationTstamp), DBGPCONVERT(pInst)));
                        } else {
                            if (OSCheck) {
                                // Keep tracking of pressure, take as plateau max value of pressure during 60ms after valve closing.
                                aCaptGetResult(Flt1PSensor, &pAvgUShort);
                                pPlatInsp = pAvgUShort;
                                if (pPlatMax < pAvgUShort) {
                                    pPlatMax = pAvgUShort;
                                }

                                if (timeElapsed(rValveActuationTstamp, PIPLATEAUMDEL + rSV2ValveDelay)) {
                                    OSCheck = false;
                                    inspOSMeasure();
                                }
                            } else {
                                if (timeElapsed(rValveActuationTstamp, rSV2ValveDelay<<1)) {
                                    if (QuantaCheck) {
                                        QuantaCheck = 0;
                                        tmpVal = vMeasureGet() - vValveActuation;
                                        if (tmpVal > VQUANTA_LIMIT) {
                                            tmpVal = VQUANTA_LIMIT;
                                        }
                                        vQuanta = (3 * tmpVal + vQuanta) / 4;
                                        tmpVal = (pInst - pValveActuation);
                                        if (tmpVal > PQUANTA_LIMIT) {
                                            tmpVal = PQUANTA_LIMIT;
                                        }
                                        pQuantaInsp = (3 * tmpVal + pQuantaInsp) / 4;
                                        if (vQuanta < 0) {
                                            vQuanta = 0;
                                        }
                                        if (pQuantaInsp < 0) {
                                            pQuantaInsp = 0;
                                        }
                                    }
                                    pAdj = pInst; // At this moment variable used to control is instantaneous pressure.
                                    vAdj = vMeasureGet();
                                    // Can assign vars here since there is no ongoing measurement.
                                    vValveActuation = vAdj;
                                    pValveActuation = pAdj;

                                    // Now check if valve can be opened, taking into account the estimated pressure and volume increases during the fixed time the valve will be open.
                                    // Try to do not exceed the set overshoot limits for both pressure and volume.
                                    pAdj = pAdj + (pQuantaInsp >> 1);
                                    vAdj = vAdj + (vQuanta >> 1);

                                    if (((pInst + pQuantaInsp) < intMaxP) &&
                                            (((intVentMode == VMODE_PRESSURE) && (pAdj < intIP)) ||
                                             ((intVentMode == VMODE_VOLUME) && (vAdj < intMaxV  )))) {
                                        OPEN_SV2LOW;
                                        rValveActuationTstamp = timeGet();
                                        QuantaCheck = true;
                                        DEBUG_PRINT(("PI-VO T %d - Pi %d VOL %d\n", timeDiff(rCycleTime, rValveActuationTstamp), DBGPCONVERT(pInst), vMeasureGet()));
                                    }
                                }
                            }
                        }
                    }
                    // User inputs, screen update.
                    screenMng();
                    MenuMng();
                }

                // Partially compensate for SV3 opening time.
                if ((SV3ISOPEN) && (rSV3ValveDelay > TIME_MS(50)) &&
                        ((intIDuration < (rSV3ValveDelay - TIME_MS(50))) || timeElapsed(rCycleTime, intIDuration - (rSV3ValveDelay - TIME_MS(50))))) {
                    valveDelayCheck = true;
                    rValveDelayStart = timeGet();
                    CLOSE_SV3;
                    DEBUG_PRINT(("PI-OSV3 T %d\n", timeDiff(rCycleTime, rValveDelayStart)));
                }

#ifdef DEBUG
                if (timeElapsedR(&printTime, PRINTTIME)) {
                    aCaptGetResult(Flt1PSensor, &pAvgShort);
                        // User dump.
                        DEBUG_PRINT(("PI T %5d - V %3d Pi %3d Pc %3d R %2d PlatMax %3d Plat %3d PQ %d VQ %d.\n",
                                timeDiff(rCycleTime, timeGet()),
                                vMeasureGet(),
                                DBGPCONVERT(pInst),
                                DBGPCONVERT(pCtrl),
                                rSV2ValveDelay,
                                DBGPCONVERT(pPlatMax),
                                DBGPCONVERT(pPlatInsp),
                                DBGPCONVERT(pQuantaInsp),
                                vQuanta));
                        // For octave...
                        // User dump.
#if 0
                        OCTAVE_PRINT(("-: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                                timeGet(),
                                (SV2MEDISOPEN?1:0) + (SV2LOWISOPEN?2:0) + (SV3ISOPEN?4:0),
                                pInst,
                                pAvgShort,
                                pInspOS,
                                pInspOS2,
                                pInspOS3,
                                pPlatInsp,
                                pExpOS,
                                pPlatExp,
                                lungC,
                                effectiveFlowRate,
                                pPlatMax,
                                pQuantaInsp,
                                pQuantaExp,
                                vQuanta,
                                vMeasureGet()));
#else
                        // User dump.
                        OCTAVE_PRINT(("-: %d %d %d\n",
                                timeGet(),
                                (SV2MEDISOPEN?1:0) + (SV2LOWISOPEN?2:0) + (SV3ISOPEN?4:0),
                                pInst));
                }
#endif
#endif
            }

            // Update last cycle inspiration volume. Temporal, should be removed once this is implemented on Monitor.
            lastCycleVol = vMeasureGet();
            sprintf(lcdBtnRRow, "V% 4d", lastCycleVol);
            lcdPrintBRR = true;

            /////////////////////////////////////////////
            // Expiration part of the cycle.
            /////////////////////////////////////////////
            DEBUG_PRINT(("\nEP\n"));
            // Clear filters.
            aCaptRstFlt(Flt0PSensor);
            aCaptRstFlt(Flt1PSensor);
            CLOSE_SV2;
            if (SV3ISOPEN) {
                CLOSE_SV3;
                valveDelayCheck = true;
                rValveDelayStart = timeGet();
            }

            initialSubState = true;
            OSCheck = false;
            QuantaCheck = false;

            while (1) {
                if (intVentMode == VMODE_PRESSURE) {
                    MonitorMsgSend(MONSTATE_RUNP);
                } else {
                    MonitorMsgSend(MONSTATE_RUNV);
                }
                if (timeElapsedR(&rCycleTime, intEDuration)) {
                    // Goto next.
                    bRateUpdt(false);
                    if (initialSubState) {
                        // IP not reached.
                        MonitorErrorSet(MON_EPE);
                    }
                    if (OSCheck) {
                        // Filter-down OS.
                        pExpOS = (3 * pExpOS) >> 2;
                    }
                    break;
                } else {
                    aCaptGetResult(MainPSensor, &pInst);
                    pPeepActual = pInst;
                    if (initialSubState) {
                        // Exhale.
                        // During exhalation, the pressure that is measured is lower than the pressure once SV3 opens,
                        // and the effect is higher the faster the pressure drops during the exhalation.
                        // To compensate for this, and ensure the system converges to the set value, use 
                        // a filtered version of the pressure, instead of the instantaneous pressure.
                        aCaptGetResult(Flt1PSensor, &pAvgShort);
                        // Compensation of pre-measured difference between plateu pressure and pressure at closing of valve.
                        // Due to overshoot after OPEN of SV3.
                        // Ensure comparison value is never 0.
                        tmpVal = intPEEP - pExpOS;
                        if (tmpVal <= PEEPCTRLMIN) {
                            tmpVal = PEEPCTRLMIN;
                        }
                        if (pAvgShort < tmpVal) {
                            OPEN_SV3;
                            initialSubState = 0;
                            rValveActuationTstamp = timeGet();
                            pValveActuation = pInst;
                            OSCheck = true;
                            DEBUG_PRINT(("PEI end T %d - Pi %d OS %d\n",
                                    timeDiff(rCycleTime, rValveActuationTstamp),
                                    DBGPCONVERT(pInst),
                                    DBGPCONVERT(pExpOS)));
                        }
                        if (valveDelayCheck) {
                            // Measure response time of valve.
                            // When detect a change of 1mBar on pInst-pAvgShort.
                            if (pInst < (pAvgShort - MPRESSURE_MBAR(3))) {
                                valveDelayCheck = false;
                                // Minimum reached. Store response time.
                                rTimeTmp = timeDiff(rValveDelayStart, timeGet());
                                if (rTimeTmp < TIME_MS(600)) {
                                    // If value within limits update value, if outside, keep previous value.
                                    rSV3ValveDelay = (rSV3ValveDelay + rTimeTmp) >> 1;
                                    // Clear alarm.
                                    MonitorErrorClr(MON_SV3E);
                                } else {
                                    // Set alarm.
                                    MonitorErrorSet(MON_SV3E);
                                }
                            }
                        }
                    } else {
                        // Hold PEEP.
                        MonitorErrorClr(MON_EPE);
                        if (SV2ISOPEN && (timeElapsedR(&rValveActuationTstamp, SV2OTIME) || (pInst > intMaxP))) {
                            CLOSE_SV2;
                            rValveActuationTstamp = timeGet();
                            aCaptGetResult(MainPSensor, &pInst);
                            DEBUG_PRINT(("PE VC T %d - Pi %d\n", timeDiff(rCycleTime, timeGet()), DBGPCONVERT(pInst)));
                        } else {
                            if (OSCheck) {
                                if (timeElapsed(rValveActuationTstamp, PEPLATEAUMDEL + rSV2ValveDelay)) {
                                    // Take averaged pressure measurement as mean value.
                                    aCaptGetResult(Flt0PSensor, &pAvgUShort);
                                    pPlatExp = pAvgUShort;
                                    // Filtered pExpOS.
                                    tmpVal = pPlatExp - pValveActuation;
                                    pExpOS = (3 * pExpOS + tmpVal) / 4;
                                    // Clear long filters.
                                    aCaptRstFlt(Flt2PSensor);
                                    aCaptRstFlt(Flt3PSensor);
                                    OSCheck = false;
                                    DEBUG_PRINT(("PE OSC T %d - Pi %d P0 %d\n", timeDiff(rCycleTime, timeGet()), DBGPCONVERT(pInst), DBGPCONVERT(pAvgUShort)));
                                }
                            } else {
                                // Breath detection.
                                // Comparison of two filtered versions of the pressure with different time constants.
                                // To avoid interference with the algorithm to keep PEEP controlled, PEEP control uses the 50ms filtered version of the signal.
                                aCaptGetResult(Flt2PSensor, &bdP1);
                                aCaptGetResult(Flt3PSensor, &bdP2);
                                if (((bdP1 + BDTECT_THRL) < bdP2) || (keyPeek() == KEYBREATH)) {
                                    // Detected breath, or pressed breath button
                                    DEBUG_PRINT(("BD VO T %d - Pi %d P50 %d P2000 %d\n", timeDiff(rCycleTime, timeGet()), DBGPCONVERT(pInst), DBGPCONVERT(bdP1), DBGPCONVERT(bdP1)));
                                    BLED_ON;
                                    rCycleTime = timeGet();
                                    bRateUpdt(true);
                                    break;
                                }
                                // Then PEEP level maintenance.
                                // Measure only after delay of valve actuation has elapsed, x2.
                                if (timeElapsed(rValveActuationTstamp, rSV2ValveDelay<<1)) {
                                    if (QuantaCheck) {
                                        QuantaCheck = 0;
                                        tmpVal = (bdP1 - pValveActuation);
                                        if (tmpVal > PQUANTA_LIMIT) {
                                            tmpVal = PQUANTA_LIMIT;
                                        }

                                        pQuantaExp = (3 * tmpVal + pQuantaExp) / 4;
                                        if (pQuantaExp < 0) {
                                            pQuantaExp = 0;
                                        }
                                    }
                                    if ((bdP1 + (pQuantaExp >> 1)) < intPEEP) {
                                        OPEN_SV2LOW;
                                        rValveActuationTstamp = timeGet();
                                        QuantaCheck = true;
                                        pValveActuation = pInst;
                                        DEBUG_PRINT(
                                                
                                                ("PE VO T %d - Pi %d\n", timeDiff(rCycleTime, rValveActuationTstamp), DBGPCONVERT(pInst), DBGPCONVERT(bdP1)));
                                    }
                                }
                            }
                        }
                        // User inputs, screen update.
                        screenMng();
                        MenuMng();
                    }
                }
#ifdef DEBUG
                if (timeElapsedR(&printTime, PRINTTIME)) {
                    aCaptGetResult(MainPSensor, &pInst);
                    aCaptGetResult(Flt1PSensor, &pAvgShort);
                    DEBUG_PRINT(("PE T %d - Pi %d Pd %d. R %d Pep %d POS %d PQ %d\n",
                            timeDiff(rCycleTime, timeGet()),
                            DBGPCONVERT(pInst),
                            DBGPCONVERT(pInst - pAvgShort),
                            rSV3ValveDelay,
                            DBGPCONVERT(pPlatExp),
                            DBGPCONVERT(pExpOS),
                            DBGPCONVERT(pQuantaExp)
                            ));

#if 0
                    // For octave...
                    // User dump.
                    OCTAVE_PRINT(("-: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                                timeGet(),
                                (SV2MEDISOPEN?1:0) + (SV2LOWISOPEN?2:0) + (SV3ISOPEN?4:0),
                                pInst,
                                pAvgShort,
                                pInspOS,
                                pInspOS2,
                                pInspOS3,
                                pPlatInsp,
                                pExpOS,
                                pPlatExp,
                                lungC,
                                effectiveFlowRate,
                                pPlatMax,
                                pQuantaInsp,
                                pQuantaExp,
                                vQuanta,
                                vMeasureGet()));
                    
#else
                    // User dump.
                    OCTAVE_PRINT(("-: %d %d %d\n",
                                 timeGet(),
                                (SV2MEDISOPEN?1:0) + (SV2LOWISOPEN?2:0) + (SV3ISOPEN?4:0),
                                pInst));
#endif
                }
#endif
            }
        }
        // Clean screen lower line.
        setCursor(0, 1);
        printstrblock("                ");
        DEBUG_PRINT(("POWER-OFF\n"));
        // Now ensure at least once to update monitor with new state.
        BUZZER_OFF;
        CLOSE_SV3;
        CLOSE_SV2;
        BLED_OFF;
        // Force screen refresh.
        lcdPrintTR = true;
        screenMng();
        while (PrintStrBusy());
        screenMng();
        timeDelayMs(10);
        // Wait until I2C idle, then send message.
        MonitorMsgSendBlock(MONSTATE_SLEEP);
        timeDelayMs(10);
        setBacklight(false);        
        // Now wait until power button pressed again.
        DEBUG_PRINT(("Wait for key press\n"));
        while (keyRead() != KEYPOWER);
        // Go back to stop.
        ctrlStatus = CTRL_STOP;
        timeDelayMs(10);
        MonitorMsgSendBlock(MONSTATE_STOP);
    }
}
/**
 End of File
 */
