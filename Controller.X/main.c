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

#ifdef DEBUG

void putch(char byte) {
    while (!UART1_is_tx_ready());
    UART1_Write(byte);
}
#endif


// To avoid problems with control, keep a copy of the values during first part of inspiration and expiration.
uint16_t intIP, intPEEP, intIDuration, intEDuration, intMaxV;
vmodeT intVentMode; 

///////////////////
// User parameters.
///////////////////
vmodeT   VentMode = 0; // 0 --> Pressure-control, 1 --> Volume-control.
uint8_t  MaxP = 4;
uint8_t  MaxV = 100;
uint8_t  LowVAlarm = 100;
uint8_t  HighVAlarm = 100;
uint8_t BPM = 10;
uint16_t IDuration, EDuration;
uint8_t IP = 4;
uint8_t PEEP = 4;
bool chBPM, chIP, chMaxP, chPEEP, chLowVAlarm, chHighVAlarm, chMaxV, chMaxP, chVentMode;
int16_t  vddValMean;
uint8_t  eBRate;

uint24_t bRateHist;
uint24_t bRatePtr;

// CONTROLLER INTERNAL PARAMETERS.
#define BLED_ONTIME TIME_MS(500)
// DO NOT CHANGE. NEEDED TO ENSURE STABILITY.
#define PEEPCTRLMIN   MPRESSURE_MBAR(1)
// Minimum actuation time of the valve has a big effect on pressure, so avoid acting too soon.
#define FINECTRLHIST  MPRESSURE_MBAR(2.5)

#define BDTECT_THRL   MPRESSURE_MBAR(0.5)

#define PIPLATEAUMDEL TIME_MS(150)
#define PEPLATEAUMDEL TIME_MS(350)
#define PRINTTIME TIME_MS(20)
#define SV2OTIME TIME_MS(15)
///////////////////
///////////////////

// pAvgShort assumed to have 16ms tau.
#define PAVGSHORTTAU TIME_MS(20)

inline int16_t rPressurePredict(time_t delay, int16_t pInst, int16_t pAvgShort) {
    int32_t intLVal;

    // Scale delay.
    intLVal = (delay * 256) / (PAVGSHORTTAU);
    intLVal = (intLVal * (pInst - pAvgShort)) / 256;
    return pInst + ((uint16_t) intLVal);
}

enum ctrlStatusT {
    CTRL_UNCAL,
    CTRL_STOP,
    CTRL_RUN,
    CTRL_SLEEP
} ctrlStatus;


uint16_t lastCycleVol;
uint16_t openFlowRate;

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
    MONSTATE_INIT = 0x80,
    MONSTATE_CALP = 0xC0,
    MONSTATE_CALF = 0xA0,
    MONSTATE_STOP = 0x0,
    MONSTATE_RUNP = 0x10,
    MONSTATE_RUNV = 0x14,
    MONSTATE_SLEEP = 0x8
} monStateT;

monStateT monState;

monErrorT monError;
time_t    monTstamp;

void MonitorInit(void){
    ;
}
void MonitorErrorSet(monErrorT flag){
    ctrlErrorStatus = ctrlErrorStatus|flag;
}

void MonitorErrorClr(monErrorT flag){
    ctrlErrorStatus = ctrlErrorStatus&(~flag);    
}

void MonitorMsgForcedSend (monStateT state){
#if 0
    i2c1_error_t trfError;
    int16_t vddVal;
    
    // Assemble message for monitor and sent it.
    // Ensure previous message was sent.
    trfError = I2C1_Close();
    
    DEBUG_PRINT(("I2C1 Close: %d\n", trfError));

    if (trfError == I2C1_FAIL) {
        // Enable buzzer and display error message on second line.
        sprintf(lcdBtnRow, "        M. ERROR");
        lcdPrintBR = true;
        BUZZER_ON;
        DEBUG_PRINT(("MON ERROR"));
    } else if ((BUZZERISON) && (trfError == I2C1_NOERR)) {
        // Disable buzzer.
        sprintf(lcdBtnRow, "                ");
        lcdPrintBR = true;
        BUZZER_OFF;
        DEBUG_PRINT(("MON ERROR CLR"));
    }
    
    if (trfError != I2C1_BUSY) {
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

        monitorMsg[MONIDX_VMAXV] = MaxV;
        if (chMaxV){
            monitorMsg[MONIDX_VMAXV] |= 0x80;
        }
        
        monitorMsg[MONIDX_LVAV]  = LowVAlarm;
        if (chLowVAlarm){
            monitorMsg[MONIDX_LVAV] |= 0x80;
        }
        
        monitorMsg[MONIDX_HVAV]  = HighVAlarm;
        if (chHighVAlarm){
            monitorMsg[MONIDX_HVAV]  |= 0x80;
        }
        
        monitorMsg[MONIDX_BRRTV] = eBRate;        
        monitorMsg[MONIDX_ALARMV] = ctrlErrorStatus;

        trfError = I2C1_Open(0x50);
        DEBUG_PRINT(("I2C1 Open: %d\n", trfError));    
        I2C1_SetBuffer(monitorMsg,10);
        trfError = I2C1_MasterWrite();
        DEBUG_PRINT(("I2C1 Open: %d\n", trfError));    
    }
#endif
}

void MonitorMsgSend (monStateT state){
    if (timeElapsedR(&monTstamp, TIME_MS(50))) {
         MonitorMsgForcedSend(state);     
    }
}

// Keep Rate in last 20 inspirations.
void bRateInit(void){
    bRateHist = 0;
    bRatePtr  = 1;
    eBRate = 0;
}

void bRateUpdt(bool triggered){
    bRatePtr = bRatePtr<<1;
    if (bRatePtr == 0x1L<<20){
        bRatePtr = 1;
    }

    if (bRateHist && bRatePtr) {
        eBRate -= 5;
    }
    if (triggered) {
        bRateHist |= bRatePtr;
        eBRate += 5;
    } else {
        bRateHist &= ~bRatePtr;        
    }
}

// Initialization procedure.
// Self-test.
bool InitProcedure(void) {
    int16_t vddVal, vddValMax, vddValMin;
    int16_t mPVal, mPValMax, mPValMin, mPValMean;
    int16_t aPVal, aPValMax, aPValMin, aPValMean;
    bool lcdBLight;
    time_t tstamp;
    bool initOk;

    ctrlStatus = CTRL_UNCAL;

    setCursor(0, 0);
    printstrblock("SELF-TEST.      ");
    setCursor(0, 1);
    printstrblock("AIRWAY OPEN     ");
    timeDelayMs(1000);

    vddValMin = 8000;
    vddValMax = 0;
    initOk = true;
    MonitorMsgForcedSend (MONSTATE_INIT);
        
    aCaptureSetOff(MainPSensor, 0);
    aCaptureSetOff(AuxPSensor, 0);
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

        MonitorMsgForcedSend (MONSTATE_CALP);

        // Now capture data.
        mPValMin = 4096;
        aPValMin = 4096;
        mPValMax = 0;
        aPValMax = 0;

        while (!timeElapsedR(&tstamp, TIME_MS(500))) {
            if (aCaptGetResult(MainPSensor, &mPVal)) {
                if (mPVal > mPValMax) {
                    mPValMax = mPVal;
                }
                if (mPVal < mPValMin) {
                    mPValMin = mPVal;
                }
            } else {
                ERROR_CONDITION(10);
                initOk = false;
            }

            if (aCaptGetResult(AuxPSensor, &aPVal)) {
                if (aPVal > aPValMax) {
                    aPValMax = aPVal;
                }
                if (aPVal < aPValMin) {
                    aPValMin = aPVal;
                }
            } else {
                initOk = false;
                ERROR_CONDITION(10);
            }
        }

        // Cal mean values.
        mPValMean = (mPValMax + mPValMin) >> 1;
        aPValMean = (aPValMax + aPValMin) >> 1;
        BLED_OFF;
        sprintf(lcdTopRow, "MS: % 3d AS: % 3d ", mPValMean, aPValMean);
        DEBUG_PRINT(("MAIN PRESSURE. Min %d Max %d MEAN %d\n", mPValMin, mPValMax, mPValMean));
        DEBUG_PRINT(("AUX PRESSURE. Min %d Max %d MEAN %d\n", aPValMin, aPValMax, aPValMean));
        setCursor(0, 0);
        printstrblock(lcdTopRow);
        timeDelayMs(1000);

        // Now check pressure sensors working correctly.
        if (((aPValMax - aPValMin) > 16) || ((mPValMax - mPValMin) > 16) || (mPValMean > 350) || (mPValMean > 350) || (aPValMean < 600) || (aPValMean > 1030)) {
            // Too noisy, or values outside expected range.
            setCursor(0, 0);
            printstrblock("CAL ERROR. RETRY ");
            timeDelayMs(500);
        } else {
            // Set cal values.
            aCaptureSetOff(MainPSensor, mPValMean);
            aCaptureSetOff(AuxPSensor, aPValMean);
            // Reset filters.
            aCaptRstFlt(Flt0PSensor);
            aCaptRstFlt(Flt1PSensor);
            aCaptRstFlt(Flt2PSensor);
            aCaptRstFlt(Flt3PSensor);
            DEBUG_PRINT(("MAIN PRESSURE. Min %d Max %d MEAN %d\n", mPValMin, mPValMax, mPValMean));
            DEBUG_PRINT(("AUX PRESSURE. Min %d Max %d MEAN %d\n", aPValMin, aPValMax, aPValMean));
            timeDelayMs(50);
            ctrlStatus = CTRL_STOP;
        }
    }

    if (!initOk) {
        return initOk;
    }

    // Last step, check volume. 
    tstamp = timeGet();

    // Open valve, wait for 250ms, measure flow during 250ms, display result.
    setCursor(0, 0);
    printstrblock("FLOW RATE        ");

    aPValMin = 4096;
    aPValMax = 0;

    MonitorMsgForcedSend (MONSTATE_CALF);

    OPEN_SV2;
    OPEN_SV3;
    timeDelayMs(250);
    vMeasureRst();
    tstamp = timeGet();
    while (!timeElapsedR(&tstamp, TIME_MS(250))) {
        aCaptGetResult(AuxPSensor, &aPVal);
        if (aPVal > aPValMax) {
            aPValMax = aPVal;
        }
        if (aPVal < aPValMin) {
            aPValMin = aPVal;
        }
    }
    // Open Flow Rate in ml/sec. Do it in 250ms to avoid patient damage even if patient is connected.
    openFlowRate = vMeasureGet()<<2;
    MonitorMsgForcedSend (MONSTATE_STOP);
    CLOSE_SV2;
    CLOSE_SV3;

    if (((aPValMean<<1) + aPValMax+aPValMin)>= (0.95*8192)) {
        // Flow sensor almost saturated. Flow too high.
        setCursor(0, 0);
        printstrblock("FLOW TOO HIGH   ");
        timeDelayMs(1000);
    } else {
        sprintf(lcdTopRow, "FLOW: % 3d L/min ", (int16_t) ((uint32_t) ((uint32_t) 60 * openFlowRate) / 1000));
        setCursor(0, 0);
        printstrblock(lcdTopRow);
        timeDelayMs(4000);

        if (openFlowRate < 200) {
            // Outside limits.
            setCursor(0, 0);
            printstrblock("FLOW TOO LOW    ");
            timeDelayMs(1000);
            initOk = false;
        } else if (openFlowRate > 2500) {
            // Outside limits.
            setCursor(0, 0);
            printstrblock("FLOW TOO HIGH   ");
            timeDelayMs(1000);
            initOk = false;
        }
    }
    
    // TODO: Check flow is 0 with valves closed.
    
    // Wait until key depressed.
    while (keyPeek() != -1)
        ;
    keyReadInit();
        
    return initOk;
}

void main(void) {
#ifdef DEBUG
    time_t printTime;
#endif

    int16_t pInst, pNext, pAvgShort, pAvgUShort;
    int16_t bdP1, bdP2;
    int16_t pValveActuation, pPlateau, pExpOS, pInspOS; // Variables for overshoot measurement.
    int16_t pTmp;
    time_t rCycleTime, rSubCycleTime, rValveAcuationTstamp;
    time_t rSV2ValveDelay, rSV3ValveDelay;
    time_t rValveDelayStart, rTimeTmp;
    bool initialSubState, valveDelayCheck, OSCheck;
    
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

    // Init all control variables.
    rSV2ValveDelay = 20;
    rSV3ValveDelay = 40;
    valveDelayCheck = false;
    OSCheck = false;
    pExpOS = 0;
    pInspOS = 0;
    IDuration = ((uint16_t) 60*1000)/(3*BPM);
    EDuration = ((uint16_t) 60*1000/BPM) - IDuration;
    vMeasureInit();
    lastCycleVol = 0;
    keyReadInit();
    screenInit();
    bRateInit();
    MonitorInit();

    while (1) {
        char keyTmp;
        while (!InitProcedure())
            ;

        setCursor(0, 0);
        printstrblock("PRESS + TO REPEAT ");
        setCursor(0, 1);
        printstrblock("PRESS - TO EXIT ");

        keyTmp=keyRead();
        while ((keyTmp != KEYMINUS)&&(keyTmp != KEYPLUS)) {
            keyTmp=keyRead();
        }
        if (keyTmp == KEYMINUS){
            break;
        }
    }
    
    // Now wait until BREATH button pressed.
    lcdPrintTR=true;
    setCursor(0, 1);
    printstrblock("BREATH TO START ");
    BLED_ON;
    while (ctrlStatus != CTRL_RUN) {
        // User inputs, screen update.
        MenuMng();
        screenMng();
        
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

    // Start periodic message sent.
    monTstamp = timeGet();
    if (VentMode == VMODE_PRESSURE){
        MonitorMsgForcedSend(MONSTATE_RUNP);
    } else {
        MonitorMsgForcedSend(MONSTATE_RUNV);        
    }

#if 0
    if (0) {
        time_t tstamp1, tstamp2, tstamp3;
        tstamp1 = timeGet();
        printTime = timeGet();
        uint16_t v1, v2;
        int16_t auxP;
        uint32_t volume;

        volume = 0;

        // Flow sensor calibration.
        tstamp3 = timeGet();
        while (1) {
            if (timeElapsedR(&tstamp1, TIME_MS(2))) {
                aCaptGetResult(AuxPSensor, &auxP);
                if (auxP > 0) {
                    v1 = auxP << 4;
                    v2 = isqrt(v1);
                } else {
                    v1 = 0;
                    v2 = 0;
                }
                if (v2 > 20) {
                    volume = volume + v2;
                }
                tstamp2 = timeGet();
            }

            if (timeElapsedR(&printTime, TIME_MS(100))) {
                // Scale up for maximum resolution.
                DEBUG_PRINT(("T %d VP %d SVP %d, SQRT %d. VolumeIrq %ld Volume %ld Del %d\n",
                        timeDiff(tstamp3, timeGet()), auxP, v1, v2, vMeasureGet(), volume, timeDiff(tstamp1, tstamp2)));
            }
        }
        // Code with constant on-off to debug pressure sensor noise.
        while (1) {
            if (timeElapsedR(&tstamp1, TIME_MS(100))) {
                if (aCaptGetResult(MainPSensor, &pInst)) {
                    DEBUG_PRINT(("P %d  \r", pInst / MPRESSURE_MBAR(1)));
                }

                // DORDBG. SET RA2 TO 1.
                IO_RA2_Toggle();
            }
        }
    } else {
        // Normal code. 
        rCycleTime = timeGet();
#ifdef DEBUG
        printTime = timeGet();
#endif
    }
#endif

    // Start.
    rCycleTime = timeGet();

    while (1) {
        /////////////////////////////////////////////
        // Inspiration part of the cycle.
        /////////////////////////////////////////////
        DEBUG_PRINT(("\nIP\n"));
        intVentMode = VentMode;
        if (intVentMode == VMODE_PRESSURE) {
            intIP = MPRESSURE_MBAR(IP);             
            intMaxV = MaxV;
        } else {
            intIP = MPRESSURE_MBAR(MaxP);
        }
        intPEEP = MPRESSURE_MBAR(PEEP);
        intIDuration = TIME_MS(IDuration);
        intEDuration = TIME_MS(EDuration);
        // Set valves initial values.
        OPEN_SV2;
        OPEN_SV3;

        // Init variables for new cycle.
        initialSubState = true;
        valveDelayCheck = true;
        OSCheck = false;
        rValveDelayStart = timeGet();
        vMeasureRst();
        while (1) {
            if (intVentMode == VMODE_PRESSURE){
                MonitorMsgSend(MONSTATE_RUNP);
            } else {
                MonitorMsgSend(MONSTATE_RUNV);        
            }

            if (timeElapsedR(&rCycleTime, intIDuration)) {
                // Goto next.
                if (initialSubState){
                    // IP not reached.
                    MonitorErrorSet(MON_IPE);                    
                }
                if (OSCheck) {
                    OSCheck = false;
                    aCaptGetResult(Flt1PSensor, &pAvgUShort);
                    pTmp = pAvgUShort - pValveActuation;
                    if (pTmp > 0){
                        pInspOS = (3*pInspOS + pTmp)>>2;
                    }
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
                    if (aCaptGetResult(MainPSensor, &pInst)) {
                        // Two checks. Instantaneous pressure > IP, OR Prediction of pressure at activation time of valve > IP.
                        // Moreover consider also OS even with those settings
                        if ((pInspOS < MPRESSURE_MBAR(5)) && (pInspOS > MPRESSURE_MBAR(-5))) {
                            pTmp = intIP - pInspOS;
                        } else {
                            pTmp = intIP;
                        }

                        // Pressure prediction computed by presPredict function.
                        aCaptGetResult(Flt1PSensor, &pAvgShort);
                        pNext = rPressurePredict(rSV2ValveDelay, pInst, pAvgShort);
                        if (((pNext > pTmp) || (pInst > pTmp)) && ((VentMode == VMODE_PRESSURE) || (vMeasureGet()<MaxV))) {
                            CLOSE_SV2;
                            initialSubState = false;
                            rValveAcuationTstamp = timeGet();
                            // Here two possibilities exist:
                            // Do like in EP, calculate overshoot respect to instantaneous pressure, and apply to it.
                            // In this case, no need to compute pressure prediction.
                            // The other option is to compute OS over pressure prediction, and apply also over the prediction.
                            // The theoretical advantage of the second case is that the OS calculated will be smaller, so it will be less sensitive to the changing conditions.
                            pValveActuation = pNext;
                            OSCheck = true;
                            pPlateau = 0;
                            DEBUG_PRINT(("PII end T %d - Pi %d Pn %d\n", timeDiff(rCycleTime, timeGet()), (10 * pInst) / MPRESSURE_MBAR(1), (10 * pNext) / MPRESSURE_MBAR(1)));
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
                                    rSV2ValveDelay = (rSV2ValveDelay + rTimeTmp)>>1;
                                    // Clear alarm.
                                    MonitorErrorClr(MON_SV2E);
                                } else {
                                    // Raise alarm.
                                    MonitorErrorSet(MON_SV2E);
                                }
                            }
                        }
                    }
                } else {
                    MonitorErrorClr(MON_IPE);
                    if (SV2ISOPEN) {
                        if (timeElapsedR(&rSubCycleTime, SV2OTIME)) {
                            CLOSE_SV2;
                            rValveAcuationTstamp = timeGet();
                            aCaptGetResult(MainPSensor, &pInst);
                            DEBUG_PRINT(("PI VC T %d - Pi %d\n", timeDiff(rCycleTime, rValveAcuationTstamp), (10 * pInst) / MPRESSURE_MBAR(1)));
                        }
                    } else if ((aCaptGetResult(MainPSensor, &pInst))) {
                        if (OSCheck) {
                            // Keep tracking of pressure, take as plateau max value of pressure during 60ms after valve closing.
                            aCaptGetResult(Flt1PSensor, &pAvgUShort);
                            if (pPlateau < pAvgUShort) {
                                pPlateau = pAvgUShort;
                            }

                            if (timeElapsed(rValveAcuationTstamp, PIPLATEAUMDEL + rSV2ValveDelay)) {
                                // Filtered pExpOS.
                                pTmp = pPlateau - pValveActuation;
                                pInspOS = (3*pInspOS + pTmp)>>2;
                                OSCheck = false;
                                DEBUG_PRINT(("PIOSE VO T %d - Pi %d\n", timeDiff(rCycleTime, timeGet()), (10 * pInst) / MPRESSURE_MBAR(1)));
                            }
                        } else {
                            // Partially compensate for SV3 opening time, once inhalation part has been reached.
                            if ((SV3ISOPEN) && (rSV3ValveDelay > TIME_MS(50)) && ((intIDuration < (rSV3ValveDelay - TIME_MS(50))) || timeElapsed(rCycleTime, intIDuration - (rSV3ValveDelay - TIME_MS(50))))) {
                                valveDelayCheck = true;
                                rValveDelayStart = timeGet();
                                CLOSE_SV3;
                                DEBUG_PRINT(("PI VO T %d OSV3\n", timeDiff(rCycleTime, rValveDelayStart)));
                            }
                            if ((timeElapsed(rValveAcuationTstamp, 32 * rSV2ValveDelay / 16) && (pInst < (intIP - FINECTRLHIST))) && ((VentMode == VMODE_PRESSURE) || (vMeasureGet()<MaxV))) {
                                // Measure only after delay of valve actuation has elapsed, x2.
                                OPEN_SV2;
                                rSubCycleTime = timeGet();
                                DEBUG_PRINT(("PI VO T %d - Pi %d\n", timeDiff(rCycleTime, rSubCycleTime), (10 * pInst) / MPRESSURE_MBAR(1)));
                            }
                        }
                    }
                }
                // User inputs, screen update.
                screenMng();
                MenuMng();
            }

#ifdef DEBUG
            if (timeElapsedR(&printTime, PRINTTIME)) {
                aCaptGetResult(MainPSensor, &pInst);
                aCaptGetResult(Flt1PSensor, &pAvgShort);
                pNext = rPressurePredict(rSV2ValveDelay, pInst, pAvgShort);
                DEBUG_PRINT(("PI T %d - Vol %d Pi %d Pn %d Pd %d. R %d Pip %d OS %d.\n",
                        timeDiff(rCycleTime, timeGet()),
                        vMeasureGet(),
                        (10 * pInst) / MPRESSURE_MBAR(1),
                        (10 * (pNext)) / MPRESSURE_MBAR(1),
                        (10 * (pInst - pAvgShort)) / MPRESSURE_MBAR(1),
                        rSV2ValveDelay,
                        (10 * pPlateau) / MPRESSURE_MBAR(1),
                        (10 * pInspOS) / MPRESSURE_MBAR(1)));
            }
#endif
        }

        // Update last cycle inspiration volume. Temporal, should be removed once this is implemented on Monitor.
        lastCycleVol = vMeasureGet();
        sprintf(lcdBtnRow, "CV: % 3d ", lastCycleVol);
        lcdPrintBR = true;

        /////////////////////////////////////////////
        // Expiration part of the cycle.
        /////////////////////////////////////////////
        DEBUG_PRINT(("\nEP\n"));
        // Clear filters.
        aCaptRstFlt(Flt0PSensor);
        aCaptRstFlt(Flt1PSensor);

        rSubCycleTime = timeGet();
        CLOSE_SV2;
        if (SV3ISOPEN){
            CLOSE_SV3;
            valveDelayCheck = true;
            rValveDelayStart = timeGet();
        }
        
        initialSubState = true;
        OSCheck = false;

        while (1) {
            if (intVentMode == VMODE_PRESSURE){
                MonitorMsgSend(MONSTATE_RUNP);
            } else {
                MonitorMsgSend(MONSTATE_RUNV);        
            }
            if (timeElapsedR(&rCycleTime, intEDuration)) {
                // Goto next.
                bRateUpdt(false);
                if (initialSubState){
                    // IP not reached.
                    MonitorErrorSet(MON_EPE);
                }
                if (OSCheck) {
                    pExpOS = (3*pExpOS)>>2;
                }
                break;
            } else {
                if (initialSubState) {
                    // Exhale.
                    // During exhalation, the pressure that is measured is lower than the pressure once SV3 opens,
                    // and the effect is higher the faster the pressure drops during the exhalation.
                    // To compensate for this, and ensure the system converges to the set value, use 
                    // a filtered version of the pressure, instead of the instantaneous pressure.
                    if (aCaptGetResult(MainPSensor, &pInst)) {
                        aCaptGetResult(Flt1PSensor, &pAvgShort);
                        // Compensation of pre-measured difference between plateu pressure and pressure at closing of valve.
                        // Due to overshoot after OPEN of SV3.
                        // Ensure comparison value is never 0.
                        pTmp = intPEEP - pExpOS;
                        if (pTmp <= PEEPCTRLMIN) {
                            pTmp = PEEPCTRLMIN;
                        }
                        if (pAvgShort < pTmp) {
                            OPEN_SV3;
                            initialSubState = 0;
                            rValveAcuationTstamp = timeGet();
                            pValveActuation = pInst;
                            OSCheck = true;
                            DEBUG_PRINT(("PEI end T %d - Pi %d OS %d\n",
                                    timeDiff(rCycleTime, rValveAcuationTstamp),
                                    (10 * pInst) / MPRESSURE_MBAR(1),
                                    (10 * pExpOS) / MPRESSURE_MBAR(1)));
                        }
                        if (valveDelayCheck) {
                            // Measure response time of valve.
                            // When detect a change of 1mBar on pInst-pAvgShort.
                            if (pInst < (pAvgShort - MPRESSURE_MBAR(1))) {
                                valveDelayCheck = false;
                                // Minimum reached. Store response time.
                                rTimeTmp = timeDiff(rValveDelayStart, timeGet());
                                 if (rTimeTmp < TIME_MS(600)) {
                                    // If value within limits update value, if outside, keep previous value.
                                    rSV3ValveDelay = (rSV3ValveDelay + rTimeTmp)>>1;
                                    // Clear alarm.
                                    MonitorErrorClr(MON_SV3E);
                                } else {
                                    // Clear alarm.
                                    MonitorErrorSet(MON_SV3E);
                                }
                           }
                        }
                    }
                } else {
                    MonitorErrorClr(MON_EPE);
                    if (SV2ISOPEN) {
                        if (timeElapsedR(&rSubCycleTime, SV2OTIME)) {
                            CLOSE_SV2;
                            rValveAcuationTstamp = timeGet();
                            aCaptGetResult(MainPSensor, &pInst);
                            DEBUG_PRINT(("PE VC T %d - Pi %d\n", timeDiff(rCycleTime, timeGet()), (10 * pInst) / MPRESSURE_MBAR(1)));
                        }
                    } else if (aCaptGetResult(MainPSensor, &pInst)) {
                        if (OSCheck) {
                            if (timeElapsed(rValveAcuationTstamp, PEPLATEAUMDEL + rSV2ValveDelay)) {
                                // Take averaged pressure measurement as mean value.
                                aCaptGetResult(Flt0PSensor, &pAvgUShort);
                                pPlateau = pAvgUShort;
                                // Filtered pExpOS.
                                pTmp = pPlateau - pValveActuation;
                                pExpOS = (3 * pExpOS + pTmp) / 4;
                                // Clear long filters.
                                aCaptRstFlt(Flt2PSensor);
                                aCaptRstFlt(Flt3PSensor);
                                OSCheck = false;
                                DEBUG_PRINT(("PE OSC T %d - Pi %d P0 %d\n", timeDiff(rCycleTime, timeGet()), (10 * pInst) / MPRESSURE_MBAR(1), (10 * pAvgUShort) / MPRESSURE_MBAR(1)));
                            }
                        } else {
                            // Breath detection.
                            // Comparison of two filtered versions of the pressure with different time constants.
                            // To avoid interference with the algorithm to keep PEEP controlled, PEEP control uses the 50ms filtered version of the signal.
                            aCaptGetResult(Flt2PSensor, &bdP1);
                            aCaptGetResult(Flt3PSensor, &bdP2);
                            if (((bdP1 + BDTECT_THRL) < bdP2) || (keyPeek() == KEYBREATH)) {
                                // Detected breath, or pressed breath button
                                DEBUG_PRINT(("BD VO T %d - Pi %d P50 %d P2000 %d\n", timeDiff(rCycleTime, timeGet()), (10 * pInst) / MPRESSURE_MBAR(1), (10 * bdP1) / MPRESSURE_MBAR(1), (10 * bdP1) / MPRESSURE_MBAR(1)));
                                BLED_ON;
                                rCycleTime = timeGet();
                                bRateUpdt(true);
                                break;
                            }
                            // Then PEEP level maintenance.
                            // Measure only after delay of valve actuation has elapsed, x2.
                            if (timeElapsed(rValveAcuationTstamp, 32 * rSV2ValveDelay / 16) && (bdP1 < (intPEEP - FINECTRLHIST))) {
                                OPEN_SV2;
                                rSubCycleTime = timeGet();
                                DEBUG_PRINT(("PE VO T %d - Pi %d\n", timeDiff(rCycleTime, rSubCycleTime), (10 * pInst) / MPRESSURE_MBAR(1), (10 * bdP1) / MPRESSURE_MBAR(1)));
                            }
                        }
                    }
                }
                // User inputs, screen update.
                screenMng();
                MenuMng();
            }
#ifdef DEBUG
            if (timeElapsedR(&printTime, PRINTTIME)) {
                aCaptGetResult(MainPSensor, &pInst);
                aCaptGetResult(Flt1PSensor, &pAvgShort);
                pNext = rPressurePredict(rSV2ValveDelay, pInst, pAvgShort);
                DEBUG_PRINT(("PE T %d - Pi %d Pn %d Pd %d. R %d Pep %d OS %d\n",
                        timeDiff(rCycleTime, timeGet()),
                        (10 * pInst) / MPRESSURE_MBAR(1),
                        (10 * (pNext)) / MPRESSURE_MBAR(1),
                        (10 * (pInst - pAvgShort)) / MPRESSURE_MBAR(1),
                        rSV3ValveDelay,
                        (10 * pPlateau) / MPRESSURE_MBAR(1),
                        (10 * pExpOS) / MPRESSURE_MBAR(1)));
            }
#endif
        }
    }
}
/**
 End of File
 */