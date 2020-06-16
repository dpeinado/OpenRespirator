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
//  Copyright © 2020 Carlos Pardo
//  This file is part of project: OpenRespirator
//
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "buzzer.h"
#include "alarm.h"

// This code generates alarm tones and alarm tones sequences



uint8_t alarmStep;
uint8_t alarmState;

//---------------------- BUZZER STATE
#define BUZZER_OFF 0
#define BUZZER_LOW 1
#define BUZZER_A   2
#define BUZZER_B   3
#define BUZZER_C   4
#define BUZZER_D   5
#define BUZZER_E   6
#define BUZZER_F   7

uint8_t buzzerState;

#define PERIOD_A  0x8D  // 440 Hz
#define PERIOD_B  0xFC  // 246.94 Hz
#define PERIOD_C  0xEE  // 261.63 Hz
#define PERIOD_D  0xD4  // 293.66 Hz
#define PERIOD_E  0xBD  // 329.63 Hz
#define PERIOD_F  0xB2  // 349.23 Hz

#define TIMER_400MS 102
#define TIMER_200MS 51
#define TIMER_100MS 26
#define TIMER_180MS 46
#define TIMER_1S    255

static bool on;
static bool detected;
static bool checkController;
static bool controllerDetected;

void BuzzerSet(uint8_t state) {
    alarmState = state;
    alarmStep = 0;
    BuzzerOff();
    BuzzerTask();
}

void BuzzerClear(void) {
    alarmState = ALARM_NONE;
    BuzzerOff();
    TMR4_Stop();
}

void BuzzerHandler( void) {
//    printf("BH\r\n");
    BuzzerTask();
}

void BuzzerTestHandler( void) {    
//    printf("BTH\r\n");
    if (on) {
        // I come from Buzzer on
        if (detected) {
            // Buzz is detected by micro
            printf("_DB_");
        } else {
            // Not detected
            printf("_EB_");
        }
    }
    BuzzerOff();
}

void BuzzerTest(char note) {
    TMR4_SetInterruptHandler(BuzzerTestHandler);
    switch (note) {
        case 'A':
            BuzzerOn(PERIOD_A);
            break;
        case 'B':
            BuzzerOn(PERIOD_B);
            break;
        case 'C':
            BuzzerOn(PERIOD_C);
            break;
        case 'D':
            BuzzerOn(PERIOD_D);
            break;
        case 'E':
            BuzzerOn(PERIOD_E);
            break;
        case 'F':
            BuzzerOn(PERIOD_F);
            break;            
        default:
            BuzzerOff();
            return;         
    };
    TMR4_Period8BitSet(TIMER_200MS);
    TMR4_Start();
}

// ALARM_NONE 0
// ALARM_LOW  1 // E 200ms LOW 200ms C 200ms LOW 16s
// ALARM_MED  2 // C 200ms LOW 200ms A 200ms LOW 200ms F 200ms LOW 8s
// ALARM_HIGH 4 // C 180ms LOW 100ms A 180ms LOW 100ms F 180ms LOW 400ms A 180ms LOW 100ms F 180ms LOW 1s
                     // ... C 180ms LOW 100ms A 180ms LOW 100ms F 180ms LOW 400ms A 180ms LOW 100ms F 180ms LOW 4s

void BuzzerTask(void) {
    static int noDetections = 0;
    if (on) {
        // I come from Buzzer on
        if (detected) {
            // Buzz is detected by micro
            ClearMonitorFailAlarm(); // Latched should not be here
            noDetections = 0;
            printf("_DB_");
        } else {
            // Not detected
            // we give 3 tries           
            if (noDetections>=2) {
                SetMonitorFailAlarm();
            } else {
                noDetections ++;
            }
            printf("_EB_");
        }
    }
    BuzzerOff();
    TMR4_Stop();
    TMR4_SetInterruptHandler(BuzzerHandler);
    switch(alarmState) {
        case ALARM_LOW:
            switch (alarmStep) {
                case 0:
                    BuzzerOn(PERIOD_E);
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep++;
                    TMR4_Start();
                    break;             
                case 1:
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 2:
                    BuzzerOn(PERIOD_C);
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                    TMR4_Period8BitSet(TIMER_1S);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 18:
                    TMR4_Period8BitSet(TIMER_1S);
                    alarmStep=0;
                    TMR4_Start();
                    break;
                default:
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep=0;
                    TMR4_Start();
                    break;                 
            };
            break;
        case ALARM_MED:
           switch (alarmStep) {
                case 0:
                    BuzzerOn(PERIOD_C);
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep++;
                    TMR4_Start();
                    break;             
                case 1:
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 2:
                    BuzzerOn(PERIOD_A);
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 3:
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 4:
                    BuzzerOn(PERIOD_F);
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                    TMR4_Period8BitSet(TIMER_1S);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 12:
                    TMR4_Period8BitSet(TIMER_1S);
                    alarmStep=0;
                    TMR4_Start();
                    break;
                default:
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep=0;
                    TMR4_Start();
                    break;                 
            };
            break;            
        case ALARM_HIGH:
            switch (alarmStep) {
                case 0:
                    BuzzerOn(PERIOD_C);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;             
                case 1:
                    TMR4_Period8BitSet(TIMER_100MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 2:
                    BuzzerOn(PERIOD_A);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 3:
                    TMR4_Period8BitSet(TIMER_100MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 4:
                    BuzzerOn(PERIOD_F);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                    
                case 5:
                    TMR4_Period8BitSet(TIMER_400MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                    
                case 6:
                    BuzzerOn(PERIOD_A);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                    
                case 7:
                    TMR4_Period8BitSet(TIMER_100MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                    
                case 8:
                    BuzzerOn(PERIOD_F);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                    
                case 9:
                    TMR4_Period8BitSet(TIMER_1S);
                    alarmStep++;
                    TMR4_Start();
                    break;                     
                case 10:
                    BuzzerOn(PERIOD_C);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;             
                case 11:
                    TMR4_Period8BitSet(TIMER_100MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 12:
                    BuzzerOn(PERIOD_A);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 13:
                    TMR4_Period8BitSet(TIMER_100MS);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 14:
                    BuzzerOn(PERIOD_F);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                    
                case 15:
                    TMR4_Period8BitSet(TIMER_400MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                    
                case 16:
                    BuzzerOn(PERIOD_A);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                    
                case 17:
                    TMR4_Period8BitSet(TIMER_100MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                    
                case 18:
                    BuzzerOn(PERIOD_F);
                    TMR4_Period8BitSet(TIMER_180MS);
                    alarmStep++;
                    TMR4_Start();
                    break;                       
                case 19:
                case 20:
                case 21:
                    TMR4_Period8BitSet(TIMER_1S);
                    alarmStep++;
                    TMR4_Start();
                    break;
                case 22:
                    TMR4_Period8BitSet(TIMER_1S);
                    alarmStep=0;
                    TMR4_Start();
                    break;
                default:
                    TMR4_Period8BitSet(TIMER_200MS);
                    alarmStep=0;
                    TMR4_Start();
                    break;                 
            };
            break;            
        case ALARM_NONE:
        default:
            BuzzerOff();
            break;
    };
    
}

void BuzzerCMPHandler( void) {    
    //printf("BCMPH\r\n");
    if (checkController) {
        controllerDetected = true;
    } else  {
        detected = true;
    }
    CMP1_Disable();
    
}

void BuzzerInit (void) {
    // Initialize TIMER 4 handler
    TMR4_SetInterruptHandler(BuzzerHandler);
    CMP1_SetIntHandler(BuzzerCMPHandler);
    BuzzerOff();
    detected = true;
    checkController = false;
    controllerDetected = false;
}

bool GetControllerBuzzerCheck() {
    bool tmp = controllerDetected;
    controllerDetected = false;
    return tmp;
}

void ControllerBuzzerCheck() {
        CMP1_Enable();
        checkController = true;
}

void BuzzerCheck (void) {
    BuzzerOn(PERIOD_A);
    __delay_ms(250);
    BuzzerOff();
    if (detected==false) {
        SetMonitorFailAlarm();
        printf("Error Buzzer\r\n");
    } else {
        printf("Found Buzzer\r\n");
        }
}


void BuzzerOn (uint8_t period) {

    T2PR = period;
    PWM6CON = 0x80;

    on = true;
    detected = false;
    
    CMP1_Enable();

}

void BuzzerOff (void) {
    PWM6CON = 0x00;
    on = false;
    CMP1_Disable();
}