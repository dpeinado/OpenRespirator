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
#include "brate.h"
#include "selftest.h"
#include "monComm.h"
#include "i2c2_mux.h"

#ifdef DEBUG

#define DBGPCONVERT(pValue) ((int16_t) (((2560/MPRESSURE_MBAR(1))*((int24_t) pValue))>>8))

void putch(char byte) {
    while (!UART1_is_tx_ready());
    UART1_Write(byte);
}
#endif

// To avoid problems with control, keep a copy of the values during first part of inspiration and expiration.
int16_t intBdTrig, intIP, intMaxP, intPEEP, intIDuration, intEDuration, intMaxV;
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
uint8_t BdTrig = 2;
bool    sBreath;

// CONTROLLER INTERNAL PARAMETERS.
#define BLED_ONTIME TIME_MS(500)
// DO NOT CHANGE. NEEDED TO ENSURE STABILITY.
#define PEEPCTRLMIN   MPRESSURE_MBAR(1)

#define PIPLATEAUMDEL TIME_MS(150)
#define PEPLATEAUMDEL TIME_MS(350)
#define SV2OTIME TIME_MS(15)
#define PRINTTIME TIME_MS(20)
///////////////////
///////////////////

// pAvgShort assumed to have 16ms tau.
#define PAVGSHORTTAU TIME_MS(20)

ctrlStatusT ctrlStatus;
uint16_t lastCycleVol;

int16_t pInst, pAvgShort, pAvgUShort;
// Variables for breath detect.
int16_t bdP1, bdP2;

// Pressure overshoot measurement variables. Both for inspiration and expiration, overshoot measures 
// the difference between the pressure estimation that is used to close the valve (pValveActuation), and the real 
// pressure reached some time after valve is closed (pPlateau).
int16_t pCtrl, pValveActuation, pPlatMax, pPlatInsp, pPlatExp, pPeepActual, pExpOS, pInspOS; // Variables for overshoot measurement.
// Volume overshoot measurement variables. Measure volume difference between valve close time and after valve actuation time.
// Best way to decide when to determine SV2 close time, even in pressure-mode is to measure the volume that produces the desired IP.
// Estimate lung compliance, multiplied by the desired IP it gives the estimated volume for it.
uint16_t lungC, lungR;
int16_t pInspOS3, pInspOS2; // Pressure overshoot during inhalation for each intermediate valve setting.
int16_t vValveActuation, vPlateau, vInspOS;
// Volume quanta. Volume injected on each fixed-time SV2 opening after initial subState.
int pInspVSet, pInspOSVSet;
int16_t vQuanta;
int16_t pQuantaInsp, pQuantaExp;
int16_t pAdj, vAdj;
uint16_t tInsp;
//uint16_t effectiveFlowRate;
// Inverse of the flow rate, scaled by 2^18, in ms/ml.
uint16_t effectiveFlowRateInv;
time_t rCycleTime, rValveActuationTstamp, rVHighActuationTstamp, rVMedActuationTstamp, rVLowActuationTstamp;
// Response time of the valves. For SV2, measure both close and open actuation time.
time_t rSV2ValveORT, rSV2ValveCRT, rSV3ValveORT;
time_t rValveDelayStart, rTimeTmp;
bool initExpSubState, initInspSubState, sv2DelayCheck, sv3DelayCheck, OSCheck, QuantaCheck;
bool OSCheckInt;

void inspOSMeasure(void) {
    uint16_t tmpUVal;
    int16_t tmpVal;
    int8_t setScale;

    switch (pInspOSVSet) {
        case 1:
            setScale = 3*2;
            break;
        case 2:
            setScale = 3*2/2;
            break;
        case 3:
            setScale = 3*2/3;
            break;
        default:
          setScale = 0;    
    }
    
    // Valve setting when measuring OS. Normalize OS for both valves open (3x).
    // Then scale down in use for the actual setting. This produces no difference in constant conditions, but would enhance the behaviour in changing conditions.
    tmpVal = (setScale*(pPlatMax - pValveActuation))>>1;
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
    
    tmpVal = (setScale*(tmpVal - vValveActuation))>>1;
    if (vInspOS == 0){
        vInspOS = (3*tmpVal)>>2;
    } else {
        vInspOS = (3 * vInspOS + tmpVal) >> 2;
    }

    // Calculate also lungR (mbar/l/min).
    // lungR = (pPlatMax - pPlatInsp)/FlowRate at end of inspiration.
    tmpVal = pPlatMax - pPlatInsp;
    tmpVal = ((uint32_t) setScale*tmpVal*effectiveFlowRateInv)>>9;
    if (tmpVal > (200*MPRESSURE_MBAR(1))){
        tmpVal = 200;
    } else if (tmpVal < (2*MPRESSURE_MBAR(1))){
        tmpVal = 2;
    }
    lungR = (3*lungR + tmpVal)>>2;

    DEBUG_PRINT(("\nIPOS LC %d Plat %d FI %d\n", lungC,DBGPCONVERT(pPlatInsp), effectiveFlowRateInv));
}

// Due to difference between open and close response times of the valves, it is
// needed to delay SV2Low open when switching from SV2Med to SV2Low.
void SV2DelayOpen(void){
    if (initInspSubState) {
        OPEN_SV2LOW;
    }
}

// Power-off
void CtrlPowerOff(void) {
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
    timeDelayMs(50);
    // Wait until I2C idle, then send message.
    MonitorMsgSendBlock(MONSTATE_SLEEP);
    timeDelayMs(50);
    setBacklight(false);
    // Now wait until power button pressed again.
    DEBUG_PRINT(("Wait for key press\n"));
    while (keyRead() != KEYPOWER);
    // Go back to stop.
    ctrlStatus = CTRL_STOP;
    timeDelayMs(50);
    MonitorMsgSendBlock(MONSTATE_STOP);
}

void main(void) {
    uint16_t tmpUVal;
    int16_t tmpVal, tmpVal2;
    int16_t pInspOSScale, vInspOSScale;
#ifdef DEBUG
    time_t printTime;
#endif
    bool tstScreen = true;


    // Initialize the device
    SYSTEM_Initialize();
    // Disable ADC irq.
    PIE1bits.ADIE = 0;
    PIE1bits.ADTIE = 0;

    TMR4_SetInterruptHandler(SV2DelayOpen);

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

    while (1) {
        char keyTmp;
        while (!SelfTest(tstScreen))
            tstScreen = false;

        tstScreen = false;
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

            if (ctrlStatus == CTRL_SLEEP) {
                CtrlPowerOff();
                setCursor(0, 1);
                printstrblock("                ");
                timeDelayMs(10);
                setBacklight(true);
                timeDelayMs(10);
                // Now wait until BREATH button pressed.
                lcdPrintTR = true;
                lcdPrintBR = true;
                BLED_ON;
            }

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
        while (MonitorMsgBusy());

        // Init all control variables.
        rSV2ValveORT = 20;
        rSV2ValveCRT = 50;
        rSV3ValveORT = 40;
        sv2DelayCheck = false;
        OSCheck = false;
        QuantaCheck = false;
        pExpOS = 10;
        pInspOS = 50;
        pInspOS3 = 50;
        pInspOS2 = 50;
        vInspOS = 50;
        vQuanta = 30;
        pQuantaInsp = 20;
        pQuantaExp = 20;
        lungC = 0;
        lungR = 0;
        sBreath = false;
        // Flow rate in ml/sec.
        //        effectiveFlowRate=freeFlowRateF;
        // Inverse effective flow rate. Scaled by 2^18
        if (freeFlowRateF > 50) {
            effectiveFlowRateInv = ((uint24_t) 1 << 18) / freeFlowRateF;
        }
        pPeepActual = 0;
        IDuration = ((uint16_t) 60 * 1000) / (3 * BPM);
        EDuration = ((uint16_t) 60 * 1000 / BPM) - IDuration;
        lastCycleVol = 0;
        bRateInit();

        // Start periodic message sent.
        if (VentMode == VMODE_PRESSURE) {
            MonitorMsgSendBlock(MONSTATE_RUNP);
        } else {
            MonitorMsgSendBlock(MONSTATE_RUNV);
        }

        // Start.
        rCycleTime = timeGet();
        while (ctrlStatus != CTRL_SLEEP) {
            /////////////////////////////////////////////
            // Inspiration part of the cycle.
            /////////////////////////////////////////////

            intVentMode = VentMode;
            intMaxP = MPRESSURE_MBAR(MaxP);
            intPEEP = MPRESSURE_MBAR(PEEP) + PEEP_COMP;
            intBdTrig = MPRESSURE_MBAR(BdTrig);
            intIDuration = TIME_MS(IDuration);
            if (sBreath) {
                intEDuration = TIME_MS(EDuration + 1000);
            } else {
                intEDuration = TIME_MS(EDuration);
            }

            if (intVentMode == VMODE_PRESSURE) {
                intIP = MPRESSURE_MBAR(IP);
                // Get target volume to reach desired IP.
                intMaxV = ((((uint24_t) intIP - pPeepActual)*((uint24_t) lungC)) >> 7);
                // Check do not exceed max volume.
                if (intMaxV > (10 * HighVAlarm)) {
                    intMaxV = 10 * HighVAlarm;
                }
            } else {
                // In volume mode --> Target volume implies starting from the setted PEEP. If PEEP is higher --> lower the volume to avoid damage.
                intIP = 0;
                if (pPeepActual > ((MPRESSURE_MBAR(1) + intPEEP))) {
                    intMaxV = 10 * ((uint16_t) MaxV) - (((pPeepActual - intPEEP) * lungC) >> 7);
                } else {
                    intMaxV = 10 * ((uint16_t) MaxV);
                }
            }
            DEBUG_PRINT(("\nIP. MaxV %d EBRate %d SV2RO %d SV2RC %d PQ %d VQ %d.\n", intMaxV, bRateGet(), rSV2ValveORT, rSV2ValveCRT, DBGPCONVERT(pQuantaInsp), vQuanta));
            // Print debug info of 'static' variables.

            // Set valves initial value.
            //            if ((((uint24_t) 3000*intMaxV)/effectiveFlowRate) < INSP_TIME) {
            tmpVal2 = ((uint24_t) 3 * effectiveFlowRateInv * intMaxV) >> 8;
            if (tmpVal2 < INSP_TIME) {
                OPEN_SV2LOW;
                rVLowActuationTstamp = timeGet();
                rVMedActuationTstamp = rCycleTime;
                rVHighActuationTstamp = rCycleTime;
                pInspVSet = 1;
                DEBUG_PRINT(("\nVLow\n"));
                // } else if (((uint24_t) 1500*intMaxV)/effectiveFlowRate) < INSP_TIME) {
            } else if ((tmpVal2 >> 1) < INSP_TIME) {
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
            initInspSubState = true;
            sv2DelayCheck = true;
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
                    if (initInspSubState) {
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
                    initInspSubState = false;
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
                    if (initInspSubState) {
                        // Inhale.
                        aCaptGetResult(MainPSensor, &pInst);

                        if (sv2DelayCheck) {
                            // Measure open response time of valve by check of possitive flow.
                            aCaptGetResult(VolPSensor, &tmpVal);
                            if (tmpVal > (ZVOLTH << 2)) {
                                sv2DelayCheck = false;
                                // Minimum reached. Store response time.
                                rTimeTmp = timeDiff(rValveDelayStart, timeGet());
                                if (rTimeTmp < TIME_MS(100)) {
                                    // If value within limits update value, if outside, keep previous value.
                                    rSV2ValveORT = (rSV2ValveORT + rTimeTmp) >> 1;
                                    // Clear alarm.
                                    MonitorErrorClr(MON_SV2E);
                                } else {
                                    // Raise alarm.
                                    MonitorErrorSet(MON_SV2E);
                                }
                            }
                        }

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
                        tmpVal2 = ((uint24_t) 3 * effectiveFlowRateInv * (intMaxV - vValveActuation)) >> 8;
                        if ((pInspVSet == 3) &&
                                (((pCtrl + pInspOS3 + pInspOS2) > ((7 * intMaxP) >> 3)) ||
                                //                             ((((int24_t) 1500*(intMaxV-vValveActuation))/effectiveFlowRate) < tmpVal))) {
                                ((tmpVal2 >> 1) < tmpVal))) {
                            OPEN_SV2MED;
                            pInspVSet = 2;
                            OSCheckInt = true;
                            pPlatMax = pCtrl;
                            rVMedActuationTstamp = timeGet();
                            rValveActuationTstamp = rVMedActuationTstamp;
                            pValveActuation = pCtrl;
                            DEBUG_PRINT(("PI-MED T %d - Pi %d Pc %d Vol %3x OS %d RF %d\n", timeDiff(rCycleTime, timeGet()), DBGPCONVERT(pInst), DBGPCONVERT(pCtrl), vValveActuation, DBGPCONVERT(pInspOS3), effectiveFlowRateInv));
                        } else if ((pInspVSet == 2) &&
                                (((pCtrl + pInspOS2) > ((7 * intMaxP) >> 3)) ||
                                ((tmpVal2) < tmpVal))) {
                            //                                    ((((int24_t) 3000*(intMaxV-vValveActuation))/effectiveFlowRate) < tmpVal))) {
                            if (rSV2ValveCRT > rSV2ValveORT) {
                                CLOSE_SV2;
                                T4PR = 4 + rSV2ValveCRT - rSV2ValveORT;
                                TMR4_Start();
                            } else {
                                OPEN_SV2LOW;
                            }
                            if (OSCheckInt && (pInspOS3 < intMaxP)) {
                                // Should not happen.
                                pInspOS3 = pInspOS3 + MPRESSURE_MBAR(1);
                            }
                            pInspVSet = 1;
                            OSCheckInt = true;
                            pPlatMax = pCtrl;
                            rVLowActuationTstamp = timeGet();
                            rValveActuationTstamp = rVLowActuationTstamp;
                            pValveActuation = pCtrl;
                            DEBUG_PRINT(("PI-LOW T %d - Pi %d Pc %d Vol %d OS %d\n", timeDiff(rCycleTime, timeGet()), DBGPCONVERT(pInst), DBGPCONVERT(pCtrl), vValveActuation, DBGPCONVERT(pInspOS2)));
                        }

                        if (pPlatMax < pCtrl) {
                            pPlatMax = pCtrl;
                        }

                        // Measure overshoot, and also assign overshoot value to use for finishing ihnale.
                        if (OSCheckInt) {
                            if (timeElapsed(rValveActuationTstamp, rSV2ValveCRT + TIME_MS(15))) {
                                OSCheckInt = false;
                                if (pInspVSet == 2) {
                                    // Check pressure overshoot when switching from 3 to 2.
                                    pInspOS3 = (3 * pInspOS3 + pPlatMax - pValveActuation) >> 2;
                                } else if (pInspVSet == 1) {
                                    // Check pressure overshoot when switching from 2 to 1.
                                    pInspOS2 = (3 * pInspOS2 + pPlatMax - pValveActuation) >> 2;
                                }
                            }
                            // Pressure overshoot already considered for reducing flow, do not consider again to finish inhale.
                            pInspOSScale = 0;
                            vInspOSScale = (5 * (1 + pInspVSet) * vInspOS) >> 4;
                        } else {
                            if (pInspVSet == 3) {
                                pInspOSScale = pInspOS3;
                            } else if (pInspVSet == 2) {
                                pInspOSScale = pInspOS2;
                            } else {
                                pInspOSScale = (5 * pInspVSet * pInspOS) >> 4;
                            }
                            vInspOSScale = (5 * pInspVSet * vInspOS) >> 4;
                        }

                        // Finish inhale if pressure or volume conditions, but also if inspiration is about to finish, so SV2 close response time can be measured.
                        if (timeElapsed(rCycleTime, intIDuration - rSV2ValveCRT - TIME_MS(20)) ||
                                (pInst > (intMaxP + MPRESSURE_MBAR(2))) || // Should never happen.
                                (pCtrl > intMaxP) ||
                                ((pCtrl + pInspOSScale) > intMaxP) ||
                                ((intVentMode == VMODE_PRESSURE) && ((pCtrl + pInspOSScale) > intIP) && (vValveActuation + vInspOSScale) >= intMaxV) ||
                                ((intVentMode == VMODE_VOLUME) && ((vValveActuation + vInspOSScale) >= intMaxV))) {
                            CLOSE_SV2;
                            sv2DelayCheck = true;
                            pValveActuation = pCtrl;
                            initInspSubState = false;
                            rValveActuationTstamp = timeGet();
                            rValveDelayStart = rValveActuationTstamp;
                            tInsp = timeDiff(rCycleTime, rValveActuationTstamp);
                            // Here two possibilities exist:
                            // Do like in EP, calculate overshoot respect to instantaneous pressure, and apply to it.
                            // In this case, no need to compute pressure prediction.
                            // The other option is to compute OS over pressure prediction, and apply also over the prediction.
                            // The theoretical advantage of the second case is that the OS calculated will be smaller, so it will be less sensitive to the changing conditions.
                            OSCheck = true;
                            pPlatMax = pInst;
                            if (((pInspVSet != 1) || OSCheckInt) && ((pInst > intMaxP) || ((pCtrl + pInspOSScale) > intMaxP))) {
                                // Should never stop due to pressure, unless already in lowest volume setting from 'long time' ago.
                                // If this is the case --> increment the OS value of the valves.
                                if ((pInspOS3 < intMaxP) && ((pInspVSet == 2) || (pInspVSet == 3))) {
                                    pInspOS3 += MPRESSURE_MBAR(5);
                                } else if (pInspOS2 < intMaxP) {
                                    pInspOS2 += MPRESSURE_MBAR(5);
                                }
                            }

                            if (OSCheckInt) {
                                pInspOSVSet = pInspVSet + 1;
                            } else {
                                pInspOSVSet = pInspVSet;
                            }

                            if (pInspVSet == 3) {
                                tmpUVal = timeDiff(rVHighActuationTstamp, timeGet());
                            } else if (pInspVSet == 2) {
                                tmpUVal = timeDiff(rVHighActuationTstamp, rVMedActuationTstamp) + ((2 * 5 * timeDiff(rVMedActuationTstamp, timeGet())) >> 4);
                            } else if (pInspVSet == 1) {
                                tmpUVal = timeDiff(rVHighActuationTstamp, rVMedActuationTstamp) + ((5 * ((2 * timeDiff(rVMedActuationTstamp, rVLowActuationTstamp)) + timeDiff(rVLowActuationTstamp, timeGet()))) >> 4);
                            }

                            if ((tmpUVal > 50)&&(vValveActuation > 50)) {
                                //                                effectiveFlowRate=(3*effectiveFlowRate+(((uint24_t) 1000*vValveActuation)/tmpUVal))>>2;
                                effectiveFlowRateInv = (3 * effectiveFlowRateInv + ((((uint24_t) tmpUVal) << 8) / vValveActuation)) >> 2;
                            }

                            // Avoid too big errors in flow measurement.
                            //                            if (effectiveFlowRate < 100){
                            //    effectiveFlowRate = 100;
                            //} else if (effectiveFlowRate > 2000){
                            //    effectiveFlowRate = 2000;
                            //}
                            if (effectiveFlowRateInv > 2621) {
                                effectiveFlowRateInv = 2621;
                            } else if (effectiveFlowRateInv < 131) {
                                effectiveFlowRateInv = 131;
                            }

                            DEBUG_PRINT(("PI-END T %d - Pi %d Pc %d Vol %d PMax %d VL %d VOS %d POS %d FR %d UV %d\n",
                                    timeDiff(rCycleTime, timeGet()),
                                    DBGPCONVERT(pInst), DBGPCONVERT(pValveActuation),
                                    vValveActuation, DBGPCONVERT(intMaxP), intMaxV, vInspOS, DBGPCONVERT(pInspOSScale), effectiveFlowRateInv, tmpUVal));
                        }
                    } else {
                        // Hold IP.
                        if (sv2DelayCheck) {
                            aCaptGetResult(VolPSensor, &tmpVal);
                            if (tmpVal < (ZVOLTH << 1)) {
                                sv2DelayCheck = false;
                                // Minimum reached. Store response time.
                                rTimeTmp = timeDiff(rValveDelayStart, timeGet());
                                if (rTimeTmp < TIME_MS(150)) {
                                    // If value within limits update value, if outside, keep previous value.
                                    rSV2ValveCRT = (rSV2ValveCRT + rTimeTmp) >> 1;
                                    // Clear alarm.
                                    MonitorErrorClr(MON_SV2E);
                                } else {
                                    // Raise alarm.
                                    MonitorErrorSet(MON_SV2E);
                                }
                            }
                        }

                        aCaptGetResult(MainPSensor, &pInst);
                        MonitorErrorClr(MON_IPE);
                        if (SV2ISOPEN && (timeElapsed(rValveActuationTstamp, SV2OTIME) || (pInst > intMaxP))) {
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

                                if (timeElapsed(rValveActuationTstamp, PIPLATEAUMDEL + rSV2ValveCRT)) {
                                    OSCheck = false;
                                    inspOSMeasure();
                                }
                            } else {
                                if (QuantaCheck) {
                                    aCaptGetResult(Flt1PSensor, &pAvgUShort);
                                    if (pPlatMax < pAvgUShort) {
                                        pPlatMax = pAvgUShort;
                                    }
                                }
                                if (timeElapsed(rValveActuationTstamp, rSV2ValveCRT << 2)) {
                                    if (QuantaCheck) {
                                        QuantaCheck = 0;
                                        tmpVal = vMeasureGet() - vValveActuation;
                                        if (tmpVal > VQUANTA_LIMIT) {
                                            tmpVal = VQUANTA_LIMIT;
                                        }
                                        vQuanta = (3 * tmpVal + vQuanta) / 4;
                                        tmpVal = (pPlatMax - pValveActuation);
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

                                    // Leaks detected by comparing pressure with plateau pressure, not with the set pressure or volume, so common for both PC-SIMV and VC-SIMV.
                                    if (((pInst + pQuantaInsp) < intMaxP) &&
                                         (pAdj < (pPlatInsp - MPRESSURE_MBAR(2)))) {
                                        OPEN_SV2LOW;
                                        rValveActuationTstamp = timeGet();
                                        QuantaCheck = true;
                                        pPlatMax = pAvgUShort;
                                        DEBUG_PRINT(("PI-VO T %d - Pi %d Pq %d VOL %d\n", timeDiff(rCycleTime, rValveActuationTstamp), DBGPCONVERT(pInst), DBGPCONVERT(pQuantaInsp), vMeasureGet()));
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
                if ((SV3ISOPEN) && (rSV3ValveORT > TIME_MS(50)) &&
                        ((intIDuration < (rSV3ValveORT - TIME_MS(50))) || timeElapsed(rCycleTime, intIDuration - (rSV3ValveORT - TIME_MS(50))))) {
                    sv3DelayCheck = true;
                    rValveDelayStart = timeGet();
                    CLOSE_SV3;
                    DEBUG_PRINT(("PI-OSV3 T %d\n", timeDiff(rCycleTime, rValveDelayStart)));
                }

                if (timeElapsedR(&printTime, PRINTTIME)) {
                    DEBUG_PRINT(("-: %d %d %d %d\n",
                            timeGet(),
                            (SV2MEDISOPEN ? 1 : 0) + (SV2LOWISOPEN ? 2 : 0) + (SV3ISOPEN ? 4 : 0),
                            pInst,
                            pAvgShort, vMeasureGet()));
                }
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
                sv3DelayCheck = true;
                rValveDelayStart = timeGet();
            }

            initExpSubState = true;
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
                    sBreath = false;
                    bRateUpdt(false);
                    if (initExpSubState) {
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
                    if (initExpSubState) {
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
                            initExpSubState = 0;
                            rValveActuationTstamp = timeGet();
                            pValveActuation = pInst;
                            OSCheck = true;
                            DEBUG_PRINT(("PEI end T %d - Pi %d OS %d\n",
                                    timeDiff(rCycleTime, rValveActuationTstamp),
                                    DBGPCONVERT(pInst),
                                    DBGPCONVERT(pExpOS)));
                        }
                        if (sv3DelayCheck) {
                            // Measure response time of valve.
                            // When detect a change of 1mBar on pInst-pAvgShort.
                            if ((pInst <= ((intPEEP + pPlatInsp) >> 1)) || (pInst < (pAvgShort - MPRESSURE_MBAR(3)))) {
                                sv3DelayCheck = false;
                                // Minimum reached. Store response time.
                                rTimeTmp = timeDiff(rValveDelayStart, timeGet());
                                if (rTimeTmp < TIME_MS(600)) {
                                    // If value within limits update value, if outside, keep previous value.
                                    rSV3ValveORT = (rSV3ValveORT + rTimeTmp) >> 1;
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
                        if (SV2ISOPEN && (timeElapsed(rValveActuationTstamp, SV2OTIME) || (pInst > intMaxP))) {
                            CLOSE_SV2;
                            rValveActuationTstamp = timeGet();
                            aCaptGetResult(MainPSensor, &pInst);
                            DEBUG_PRINT(("PE VC T %d - Pi %d\n", timeDiff(rCycleTime, timeGet()), DBGPCONVERT(pInst)));
                        } else {
                            if (OSCheck) {
                                if (timeElapsed(rValveActuationTstamp, PEPLATEAUMDEL + (rSV2ValveCRT << 1))) {
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
                                if (((bdP1 + intBdTrig) < bdP2) || (keyPeek() == KEYBREATH)) {
                                    // Detected breath, or pressed breath button
                                    DEBUG_PRINT(("BD VO T %d - Pi %d P50 %d P2000 %d\n", timeDiff(rCycleTime, timeGet()), DBGPCONVERT(pInst), DBGPCONVERT(bdP1), DBGPCONVERT(bdP1)));
                                    BLED_ON;
                                    if ((bdP1 + intBdTrig) < bdP2) {
                                        sBreath = true;
                                        rCycleTime = timeGet();
                                        bRateUpdt(true);
                                    }
                                    break;
                                }
                                // Then PEEP level maintenance.
                                // Measure only after close delay of valve actuation has elapsed, x4.
                                if (timeElapsed(rValveActuationTstamp, rSV2ValveCRT << 2)) {
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
                                    if ((bdP1 + (pQuantaExp >> 1)) < (intPEEP - MPRESSURE_MBAR(1))) {
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
                            rSV3ValveORT,
                            DBGPCONVERT(pPlatExp),
                            DBGPCONVERT(pExpOS),
                            DBGPCONVERT(pQuantaExp)
                            ));

#if 0
                    // For octave...
                    // User dump.
                    OCTAVE_PRINT(("-: x %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                            timeGet(),
                            (SV2MEDISOPEN ? 1 : 0) + (SV2LOWISOPEN ? 2 : 0) + (SV3ISOPEN ? 4 : 0),
                            pInst,
                            pAvgShort,
                            pInspOS,
                            pInspOS2,
                            pInspOS3,
                            pPlatInsp,
                            pExpOS,
                            pPlatExp,
                            lungC,
                            effectiveFlowRateInv,
                            pPlatMax,
                            pQuantaInsp,
                            pQuantaExp,
                            vQuanta,
                            vMeasureGet()));

#else
                    // User dump.
                    OCTAVE_PRINT(("-: x %d %d\n",
                            timeGet(),
                            (SV2MEDISOPEN ? 1 : 0) + (SV2LOWISOPEN ? 2 : 0) + (SV3ISOPEN ? 4 : 0),
                            pInst));
#endif
                }
#endif
            }
        }

        CtrlPowerOff();
    }
}
/**
 End of File
 */
