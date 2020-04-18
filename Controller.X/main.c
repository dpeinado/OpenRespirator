/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.80.0
        Device            :  PIC18F46K42
        Driver Version    :  2.00
 */

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
 */

#include "mcc_generated_files/mcc.h"
#include "ORespGlobal.h"
#include "aCapture.h"
#include "time.h"
#include "cmath.h"
#include "vMeasure.h"
#include "keyRead.h"
#include "LiquidCrystal_I2C.h"

#ifdef DEBUG

void putch(char byte) {
    while (!UART1_is_tx_ready());
    UART1_Write(byte);
}
#endif

#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif


// To avoid problems with control, keep a copy of the values during first part of inspiration and expiration.
uint16_t intIP, intPEEP;

// User parameter limits.
#define BPM_MIN 10
#define BPM_MAX 30
#define IP_MIN 4
#define IP_MAX 35
#define PEEP_MIN 4
#define PEEP_MAX 25

///////////////////
// User parameters.
///////////////////
uint8_t BPM = 10;
uint8_t IP = 4;
uint8_t PEEP = 4;

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
// Internal parameters.
#define IDURATION TIME_S((1*60.0)/(3*BPM))
#define EDURATION (TIME_S((60.0/BPM))-IDURATION)
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

#define MENU_TOUT TIME_S(5)

enum menuStatusT {
    CFG_IDLE,
    CFG_IP,
    CFG_BPM,
    CFG_PEEP
} menuStatus;

enum ctrlStatusT {
    CTRL_UNCAL,
    CTRL_STOP,
    CTRL_RUN
} ctrlStatus;


time_t menuTstamp;
uint8_t menuVal;
char lcdTopRow[20];
char lcdBtnRow[20];
bool lcdPrint, lcdBlink;
uint16_t lastCycleVol;
uint16_t openFlowRate;

void MenuInit(void) {
    menuStatus = CFG_IDLE;
}

void MenuMng(void) {
    // Manage menu.
    int8_t keyPress;
    keyPress = keyRead();
    if (keyPress >= 0) {
        DEBUG_PRINT(("KEY! %d\n", keyPress));
        lcdPrint = true;

        switch (keyPress) {
            case KEYIP:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_IP;
                    menuVal = IP;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_IP) {
                    // Accept change and exit.
                    IP = menuVal;
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYBPM:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_BPM;
                    menuVal = BPM;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_BPM) {
                    // Accept change and exit.
                    BPM = menuVal;
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYPEEP:
                if (menuStatus == CFG_IDLE) {
                    menuStatus = CFG_PEEP;
                    menuVal = PEEP;
                    menuTstamp = timeGet();
                } else if (menuStatus == CFG_PEEP) {
                    // Accept change and exit.
                    PEEP = menuVal;
                    menuStatus = CFG_IDLE;
                } else {
                    // Any other case, abort setting.
                    menuStatus = CFG_IDLE;
                }
                break;
            case KEYPLUS:
                if (menuStatus != CFG_IDLE) {
                    menuTstamp = timeGet();
                    menuVal = menuVal + 1;
                    // Should also check limits.
                    switch (menuStatus) {
                        case CFG_IP:
                            if (menuVal > IP_MAX) {
                                menuVal = IP_MAX;
                            }
                            break;
                        case CFG_PEEP:
                            if (menuVal > PEEP_MAX) {
                                menuVal = PEEP_MAX;
                            }
                            break;
                        case CFG_BPM:
                            if (menuVal > BPM_MAX) {
                                menuVal = BPM_MAX;
                            }
                            break;
                        default:
                            // No processing.
                            break;
                    }
                }
                break;
            case KEYMINUS:
                if (menuStatus != CFG_IDLE) {
                    menuTstamp = timeGet();
                    menuVal = menuVal - 1;
                    // Should also check limits.
                    switch (menuStatus) {
                        case CFG_IP:
                            if (menuVal < IP_MIN) {
                                menuVal = IP_MIN;
                            }
                            break;
                        case CFG_PEEP:
                            if (menuVal < PEEP_MIN) {
                                menuVal = PEEP_MIN;
                            }
                            break;
                        case CFG_BPM:
                            if (menuVal < BPM_MIN) {
                                menuVal = BPM_MIN;
                            }
                            break;
                        default:
                            // No processing.
                            break;
                    }
                }
                break;
        }
    } else {
        if ((menuStatus != CFG_IDLE) && (timeElapsed(menuTstamp, MENU_TOUT))) {
            // Silently exit menu.
            lcdPrint = true;
            menuStatus = CFG_IDLE;
        }
        return;
    }
}

void screenInit(void) {
    LcdI2CInit(0x27, 16, 2);
    setCursor(0, 0);
    printstrblock("EMERG.RESPIRATOR");
    setCursor(0, 1);
    printstrblock("  CONTROLLER   ");
    timeDelayMs(TIME_MS(2000));
    clear();
    lcdPrint = true;
    blink();
}

void screenMng(void) {
    // Check if something to print.
    if (lcdPrint && !PrintStrBusy()) {
        if (menuStatus == CFG_IDLE) {
            sprintf(lcdTopRow, "% 2d %2d % 2d IV: % 3d", BPM, PEEP, IP, lastCycleVol);
        } else if (menuStatus == CFG_BPM) {
            sprintf(lcdTopRow, "% 2d %2d % 2d IV: % 3d", menuVal, PEEP, IP, lastCycleVol);
        } else if (menuStatus == CFG_PEEP) {
            sprintf(lcdTopRow, "% 2d %2d % 2d IV: % 3d", BPM, menuVal, IP, lastCycleVol);
        } else if (menuStatus == CFG_IP) {
            sprintf(lcdTopRow, "% 2d %2d % 2d IV: % 3d", BPM, PEEP, menuVal, lastCycleVol);
        }
        DEBUG_PRINT((lcdTopRow));
        lcdPrint = false;
        setCursor(0, 0);
        printstr(lcdTopRow);
        if (menuStatus != CFG_IDLE) {
            lcdBlink = true;
        }
    } else if (lcdBlink && !PrintStrBusy()) {
        lcdBlink = false;
        switch (menuStatus) {
            case CFG_BPM:
                setCursor(1, 0);
                break;
            case CFG_PEEP:
                setCursor(4, 0);
                break;
            case CFG_IP:
                setCursor(7, 0);
                break;
            default:
                // No processing.
                break;
        }
    }
}

// Initialization procedure.
// Self-test.

bool InitProcedure(void) {
    int16_t vddVal, vddValMax, vddValMin, vddValMean;
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

    aCaptureSetOff(MainPSensor, 0);
    aCaptureSetOff(AuxPSensor, 0);
    // First check VDD stability.
    tstamp = timeGet();

    while (!timeElapsedR(&tstamp, TIME_MS(500))) {
        if (aCaptGetResult(VddSensor, &vddVal)) {
            DEBUG_PRINT(("VDD %d\n", vddVal));
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
        // Repeat until correct calibration.
        // Now calibrate.
        BLED_ON;
        setCursor(0, 0);
        printstrblock("CAL PRESS BREATH");
        tstamp = timeGet();
        lcdBLight = true;
        tstamp = timeGet();

        // First, ensure no key pressed.
        while (keyPeek() != -1)
            ;
        keyRead();

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

    // Open valve, wait for 100ms, measure flow during 500ms, display result.
    setCursor(0, 0);
    printstrblock("FLOW RATE        ");

    OPEN_SV2;
    OPEN_SV3;
    timeDelayMs(250);
    vMeasureRst();
    timeDelayMs(250);
    // Open Flow Rate in ml/sec. Do it in 250ms to avoid patient damage even if patient is connected.
    openFlowRate = vMeasureGet()<<2;
    CLOSE_SV2;
    CLOSE_SV3;

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
    
    // Wait until key depressed.
    while (keyPeek() != -1)
        ;
    keyRead();
        
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
    time_t rSV2ValveDelay;
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
    valveDelayCheck = false;
    OSCheck = false;
    pExpOS = 0;
    pInspOS = 0;
    vMeasureInit();
    lastCycleVol = 0;
    keyReadInit();
    screenInit();

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
    lcdPrint=true;
    setCursor(0, 1);
    printstrblock("BREATH TO START ");
    while (ctrlStatus != CTRL_RUN) {
        BLED_ON;
        // User inputs, screen update.
        MenuMng();
        screenMng();

        
        if (keyPeek() == KEYBREATH) {
            if (keyReadEC() == ESCAPE_CODE) {
                // Key Breath pressed.
                BLED_OFF;
                ctrlStatus = CTRL_RUN;
    
                // Wait until key depressed.
                while (keyPeek() != -1)
                ;
                keyRead();
                break;
            }
        }
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

    while (1) {
        /////////////////////////////////////////////
        // Inspiration part of the cycle.
        /////////////////////////////////////////////
        DEBUG_PRINT(("\nIP\n"));
        intIP = MPRESSURE_MBAR(IP);

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
            if (timeElapsedR(&rCycleTime, IDURATION)) {
                // Goto next.
                break;
            } else {
                //TODO
                // When BPM is very high, i time very short. 
                // Ensure algorighm is stable in this conditions:
                // If SV2 is closed before end of IP cycle, and there is time to measure OS, do it.
                // Otherwise, ensure OS is not used in those conditions...
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
                        if ((pNext > pTmp) || (pInst > pTmp)) {
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
                            DEBUG_PRINT(("PII end T %d - Pi %d Pn %d\n", timeDiff(rValveDelayStart, timeGet()), (10 * pInst) / MPRESSURE_MBAR(1), (10 * pNext) / MPRESSURE_MBAR(1)));
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
                                    rSV2ValveDelay = rTimeTmp;
                                } else {
                                    // Consider adding a warning/alarm.
                                    //                                
                                }
                            }
                        }
                    }
                } else {
                    if (SV2ISOPEN) {
                        if (timeElapsedR(&rSubCycleTime, SV2OTIME)) {
                            CLOSE_SV2;
                            rValveAcuationTstamp = timeGet();
                            aCaptGetResult(MainPSensor, &pInst);
                            DEBUG_PRINT(("PI VC T %d - Pi %d\n", timeDiff(rValveDelayStart, rValveAcuationTstamp), (10 * pInst) / MPRESSURE_MBAR(1)));
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
                                DEBUG_PRINT(("PIOSE VO T %d - Pi %d\n", timeDiff(rValveDelayStart, rSubCycleTime), (10 * pInst) / MPRESSURE_MBAR(1)));
                            }
                        }
                        // Measure only after delay of valve actuation has elapsed, x2.
                        if ((!OSCheck) && timeElapsed(rValveAcuationTstamp, 32 * rSV2ValveDelay / 16) && (pInst < (intIP - FINECTRLHIST))) {
                            OPEN_SV2;
                            rSubCycleTime = timeGet();
                            DEBUG_PRINT(("PI VO T %d - Pi %d\n", timeDiff(rValveDelayStart, rSubCycleTime), (10 * pInst) / MPRESSURE_MBAR(1)));
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
                        timeDiff(rValveDelayStart, timeGet()),
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

        // Update last cycle inspiration volume.
        lastCycleVol = vMeasureGet();
        lcdPrint = true;
        /////////////////////////////////////////////
        // Expiration part of the cycle.
        /////////////////////////////////////////////
        DEBUG_PRINT(("\nEP\n"));
        intPEEP = MPRESSURE_MBAR(PEEP);
        rSubCycleTime = timeGet();
        CLOSE_SV2;
        CLOSE_SV3;
        initialSubState = true;
        valveDelayCheck = false;

        while (1) {
            if (timeElapsedR(&rCycleTime, EDURATION)) {
                // Goto next.
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
                                    timeDiff(rValveDelayStart, rValveAcuationTstamp),
                                    (10 * pInst) / MPRESSURE_MBAR(1),
                                    (10 * pExpOS) / MPRESSURE_MBAR(1)));
                        }
                    }
                } else {
                    if (SV2ISOPEN) {
                        if (timeElapsedR(&rSubCycleTime, SV2OTIME)) {
                            CLOSE_SV2;
                            rValveAcuationTstamp = timeGet();
                            aCaptGetResult(MainPSensor, &pInst);
                            DEBUG_PRINT(("PE VC T %d - Pi %d\n", timeDiff(rValveDelayStart, timeGet()), (10 * pInst) / MPRESSURE_MBAR(1)));
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
                                DEBUG_PRINT(("PE OSC T %d - Pi %d P0 %d\n", timeDiff(rValveDelayStart, timeGet()), (10 * pInst) / MPRESSURE_MBAR(1), (10 * pAvgUShort) / MPRESSURE_MBAR(1)));
                            }
                        } else {
                            // Breath detection.
                            // Comparison of two filtered versions of the pressure with different time constants.
                            // To avoid interference with the algorithm to keep PEEP controlled, PEEP control uses the 50ms filtered version of the signal.
                            aCaptGetResult(Flt2PSensor, &bdP1);
                            aCaptGetResult(Flt3PSensor, &bdP2);
                            if (((bdP1 + BDTECT_THRL) < bdP2) || (keyPeek() == KEYBREATH)) {
                                // Detected breath, or pressed breath button
                                DEBUG_PRINT(("BD VO T %d - Pi %d P50 %d P2000 %d\n", timeDiff(rValveDelayStart, rSubCycleTime), (10 * pInst) / MPRESSURE_MBAR(1), (10 * bdP1) / MPRESSURE_MBAR(1), (10 * bdP1) / MPRESSURE_MBAR(1)));
                                BLED_ON;
                                rCycleTime = timeGet();
                                break;
                            }
                            // Then PEEP level maintenance.
                            // Measure only after delay of valve actuation has elapsed, x2.
                            if (timeElapsed(rValveAcuationTstamp, 32 * rSV2ValveDelay / 16) && (bdP1 < (intPEEP - FINECTRLHIST))) {
                                OPEN_SV2;
                                rSubCycleTime = timeGet();
                                DEBUG_PRINT(("PE VO T %d - Pi %d\n", timeDiff(rValveDelayStart, rSubCycleTime), (10 * pInst) / MPRESSURE_MBAR(1), (10 * bdP1) / MPRESSURE_MBAR(1)));
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
                DEBUG_PRINT(("PE T %d - Pi %d Pn %d Pd %d. Pep %d OS %d\n",
                        timeDiff(rValveDelayStart, timeGet()),
                        (10 * pInst) / MPRESSURE_MBAR(1),
                        (10 * (pNext)) / MPRESSURE_MBAR(1),
                        (10 * (pInst - pAvgShort)) / MPRESSURE_MBAR(1),
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