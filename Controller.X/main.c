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

void putch(char byte)
{
    while (!UART1_is_tx_ready());
    UART1_Write(byte);
}

///////////////////
// User parameters.
///////////////////
uint8_t BPM=10;
uint8_t IP=30;
uint8_t PEEP=10;

#define PRINTTIME TIME_MS(20)
#define PCTIME TIME_MS(25)
// Internal parameters.
#define IDURATION TIME_S((1*60.0)/(3*BPM))
#define EDURATION (TIME_S((60.0/BPM))-IDURATION)
///////////////////
///////////////////

// pAvgShort assumed to have 16ms tau.
#define PAVGSHORTTAU TIME_MS(20)
inline int16_t rPressurePredict(time_t delay, int16_t pInst, int16_t pAvgShort){
    int32_t intLVal;
    
    // Scale delay.
    intLVal = (delay*256)/(PAVGSHORTTAU);
    intLVal = (intLVal * (pInst - pAvgShort))/256;
    return pInst + ((uint16_t) intLVal);
}

void main(void)
{
    int16_t pInst, pNext, pAvgShort, pExpMin, pExpNorm;
    time_t rCycleTime, rSubCycleTime, rValveActionTime, printTime;
    time_t rSV2ValveDelay;
    time_t rValveDelayStart, rTimeTmp;
    bool initialSubState, valveDelayActive;
    
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

    printf("PCTRL START\n");
    // First wait for startup. 50ms.
    rSubCycleTime = timeGet();
    while (timeElapsedR(&rSubCycleTime, TIME_MS(50)));
    
    // Init control variables.
    rSV2ValveDelay = 0;
    valveDelayActive = false;
    if (0) {
        // Code with constant on-off to debug pressure sensor noise.
        time_t tstamp1;
        tstamp1 = timeGet();
        while (1) {
            if (timeElapsedR(&tstamp1, TIME_MS(100))) {
                if (aCaptGetResult(MainPSensor, &pInst)) {
                    printf("P %d  \r", pInst/MPRESSURE_MBAR(1));
                }

                // DORDBG. SET RA2 TO 1.
                IO_RA2_Toggle();
            }
            // Add your application code
        }
    } else {
        // Normal code. 
        rCycleTime = timeGet();
        printTime = timeGet();
        while (1) {
            /////////////////////////////////////////////
            // Inspiration part of the cycle.
            /////////////////////////////////////////////
            printf("\nIP\n");
            // Set valves initial values.
            OPEN_SV2;
            OPEN_SV3;
            initialSubState = true;
            rValveDelayStart=timeGet();
            valveDelayActive = true;

            while (1) {
                if (timeElapsedR(&rCycleTime, IDURATION)) {
                    // Goto next.
                    break;
                } else {
                    if (initialSubState) {
                        // Inhale.
                        if (aCaptGetResult(MainPSensor, &pInst)) {
                            // Two checks. Instantaneous pressure > IP, OR Prediction of pressure at activation time of valve > IP.
                            // Pressure prediction computed by presPredict function.
                            aCaptGetResult(Flt1PSensor, &pAvgShort);
                            pNext = rPressurePredict(rSV2ValveDelay, pInst, pAvgShort);
                            if ((pNext > MPRESSURE_MBAR(IP)) || (pInst > MPRESSURE_MBAR(IP))) {
                                CLOSE_SV2;
                                initialSubState = false;
                                rValveActionTime = timeGet();
                                printf("PII end T %d - Pi %d Pn %d\n", timeDiff(rValveDelayStart,timeGet()),(10*pInst)/MPRESSURE_MBAR(1), (10*pNext)/MPRESSURE_MBAR(1));
                            }
                            if (valveDelayActive) {
                                // Measure response time of valve.
                                // When detect a change of 1mBar on pInst-pAvgShort.
                                if (pInst > (pAvgShort + MPRESSURE_MBAR(1))) {
                                    valveDelayActive = false;
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
                            if (timeElapsedR(&rSubCycleTime, PCTIME)) {
                                CLOSE_SV2;
                                rValveActionTime = timeGet();
                                aCaptGetResult(MainPSensor, &pInst);
                                printf("PI VC T %d - Pi %d\n", timeDiff(rValveDelayStart,rValveActionTime),(10*pInst)/MPRESSURE_MBAR(1));
                            }
                        } else if ((aCaptGetResult(MainPSensor, &pInst))) {
                            // Meassure only after delay of valve actuation has elapsed, increased by 25%.
                            if (timeElapsed(rValveActionTime, 20*rSV2ValveDelay/16) && (pInst < MPRESSURE_MBAR(IP))) {
                                OPEN_SV2;
                                rSubCycleTime = timeGet();
                                printf("PI VO T %d - Pi %d\n", timeDiff(rValveDelayStart,rSubCycleTime),(10*pInst)/MPRESSURE_MBAR(1));
                            }
                        }
                    }
                }

                if (timeElapsedR(&printTime, PRINTTIME)) {
                    int16_t pinst,pavg;
                    aCaptGetResult(MainPSensor, &pinst);
                    aCaptGetResult(Flt1PSensor, &pavg);
                    printf("T %d - Pu %d Pi %d Pf %d Pd %d. R %d\n", timeDiff(rValveDelayStart,timeGet()),(10*pInst)/MPRESSURE_MBAR(1),(10*pinst)/MPRESSURE_MBAR(1),(10*(2*pinst-pavg))/MPRESSURE_MBAR(1),(10*(pinst-pavg))/MPRESSURE_MBAR(1), rSV2ValveDelay);
                }
            }

            /////////////////////////////////////////////
            // Expiration part of the cycle.
            /////////////////////////////////////////////
            printf("\nEP\n");
            rSubCycleTime = timeGet();
            CLOSE_SV2;
            CLOSE_SV3;
            initialSubState = true;
            valveDelayActive = false;
            while (1) {
                if (timeElapsedR(&rCycleTime, EDURATION)) {
                    // Goto next.
                    break;
                } else {
                    if (initialSubState) {
                        // Exhale.
                        if (aCaptGetResult(MainPSensor, &pInst)) {
                            // Two checks. Instantaneous pressure > IP, OR Prediction of pressure at activation time of valve > IP.
                            // Pressure prediction computed by presPredict function.
                            aCaptGetResult(Flt1PSensor, &pAvgShort);
                            pNext = rPressurePredict(rSV2ValveDelay, pInst, pAvgShort);
                            if ((pNext < MPRESSURE_MBAR(PEEP)) || (pInst < MPRESSURE_MBAR(PEEP))) {
                                OPEN_SV3;
                                initialSubState = 0;
                                rValveActionTime = timeGet();
                                pExpMin = pInst;
                                printf("PEI end T %d - Pi %d Pn %d\n", timeDiff(rValveDelayStart,rValveActionTime),(10*pInst)/MPRESSURE_MBAR(1), (10*pNext)/MPRESSURE_MBAR(1));
                            }
                        }
                    } else {
                        if (SV2ISOPEN) {
                            if (timeElapsedR(&rSubCycleTime, PCTIME)) {
                                CLOSE_SV2;
                                rValveActionTime = timeGet();
                                aCaptGetResult(MainPSensor, &pInst);
                                printf("PE VC T %d - Pi %d\n", timeDiff(rValveDelayStart,timeGet()),(10*pInst)/MPRESSURE_MBAR(1));
                            }
                        } else if (aCaptGetResult(MainPSensor, &pInst)) {
                            // Meassure only after delay of valve actuation has elapsed, increased by 25%.
                            if (timeElapsed(rValveActionTime, 32*rSV2ValveDelay/16) && (pInst < MPRESSURE_MBAR(PEEP))) {
                                OPEN_SV2;
                                rSubCycleTime = timeGet();
                                printf("PE VO T %d - Pi %d\n", timeDiff(rValveDelayStart,rSubCycleTime),(10*pInst)/MPRESSURE_MBAR(1));
                            } else {
                                // Check minimum value reached by pressure.
                                if (pInst < pExpMin) {
                                    pExpMin = pInst;
                                }
                            }
                        }
                    }
                }
                if (timeElapsedR(&printTime, PRINTTIME)) {
                    int16_t pinst,pavg;
                    aCaptGetResult(MainPSensor, &pinst);
                    aCaptGetResult(Flt1PSensor, &pavg);
                    printf("T %d - Pu %d Pi %d Pf %d Pd %d.\n", timeDiff(rValveDelayStart,timeGet()), (10*pInst)/MPRESSURE_MBAR(1),(10*pinst)/MPRESSURE_MBAR(1),(10*(2*pinst-pavg))/MPRESSURE_MBAR(1),(10*(pinst-pavg))/MPRESSURE_MBAR(1));
                }
            }
        }
    }
}
/**
 End of File
*/