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
uint8_t IP=24;
uint8_t PEEP=10;

#define PCTIME TIME_MS(50)
// Internal parameters.
#define IDURATION TIME_S((2*60.0)/(3*BPM))
#define EDURATION (TIME_S((60.0/BPM))-IDURATION)
///////////////////
///////////////////

void main(void)
{
    int16_t mainPressure;
    time_t rCycleTime, rSubCycleTime, printTime;
    bool initialSubState;
    
    // Initialize the device
    SYSTEM_Initialize();
    // Disable ADC irq.
    PIE1bits.ADIE = 0;

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

    if (1) {
        // Code with constant on-off to debug pressure sensor noise.
        time_t tstamp1;
        tstamp1 = timeGet();
        while (1) {
            if (timeElapsedR(&tstamp1, TIME_MS(100))) {
                if (aCaptGetResult(MainPSensor, &mainPressure)) {
                    printf("P %d  \r", mainPressure);
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
            // Inspiration part of the cycle.
            printf("\nIP\n");
            // Set valves initial values.
            OPEN_SV2;
            OPEN_SV3;
            initialSubState = 1;
            while (1) {
                if (timeElapsedR(&rCycleTime, IDURATION)) {
                    // Goto next.
                    break;
                } else {
                    if (initialSubState) {
                        // Inhale.
                        if (aCaptGetResult(MainPSensor, &mainPressure)) {
                            if (mainPressure > PRESSURE_MBAR(IP)) {
                                CLOSE_SV2;
                                initialSubState = 0;
                            }
                        }
                    } else {
                        if (SV2ISOPEN) {
                            if (timeElapsedR(&rSubCycleTime, PCTIME)) {
                                CLOSE_SV2;
                            }
                        } else if (aCaptGetResult(MainPSensor, &mainPressure)) {
                            if (mainPressure < PRESSURE_MBAR(IP)) {
                                OPEN_SV2;
                                rSubCycleTime = timeGet();
                            }
                        }
                    }
                }

                if (timeElapsedR(&printTime, TIME_MS(20))) {
                    printf("P %d\n", mainPressure);
                }
            }

            // Expiration part of the cycle.
            printf("\nEP\n");
            rSubCycleTime = timeGet();
            CLOSE_SV2;
            CLOSE_SV3;
            initialSubState = 1;
            while (1) {
                if (timeElapsedR(&rCycleTime, EDURATION)) {
                    // Goto next.
                    break;
                } else {
                    if (initialSubState) {
                        // Exhale.
                        if (aCaptGetResult(MainPSensor, &mainPressure)) {
                            if (mainPressure < PRESSURE_MBAR(PEEP)) {
                                OPEN_SV3;
                                initialSubState = 0;
                            }
                        }
                    } else {
                        if (SV2ISOPEN) {
                            if (timeElapsedR(&rSubCycleTime, PCTIME)) {
                                CLOSE_SV2;
                            }
                        } else if (aCaptGetResult(MainPSensor, &mainPressure)) {
                            if (mainPressure < PRESSURE_MBAR(PEEP)) {
                                OPEN_SV2;
                                rSubCycleTime = timeGet();
                            }
                        }
                    }
                }
                if (timeElapsedR(&printTime, TIME_MS(20))) {
                    printf("P %d\n", mainPressure);
                }
            }
        }
    }
}
/**
 End of File
*/