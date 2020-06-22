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
#include "mcc_generated_files/mcc.h"
#include "alarm.h"
#include "buttons.h"
#include "monitor.h"


#define PUSHMASK 0xF0
#define PUSHLONGMASK 0xFF

static uint8_t muteButton;
static uint8_t histButton;
static int ledCnt;
static uint8_t ledType;

void ButtonInit(void) {
    ledCnt=0;
    ledType=ALARM_NONE;
    muteButton=0;
    histButton=0;
    TMR1_SetInterruptHandler(ButtonTask);
    TMR1_StartTimer();
}

void ButtonTask(void) {
    static int cnt = 0;
    cnt++;
    //if (cnt==10) { cnt = 0; printf(".\r\n"); }
    muteButton = !MUTE_GetValue() | (muteButton << 1);
    histButton = !HIST_GetValue() | (histButton << 1);
    if ((muteButton) == PUSHMASK) {
        MuteAlarm();
        //printf("\r\nMUTE\r\n");
        muteButton = 0;
    }
    if ((histButton) == PUSHMASK) {
        HistAlarm(); 
        //printf("\r\nHIST\r\n");
        histButton = 0;
    }
    if (muteButton == PUSHLONGMASK && histButton == PUSHLONGMASK) {
        DisableAlarmSV1();
        VALVE_SetHigh();
        printf("\r\nDIS SV1 ALARM!\r\n");
    }
    if (ledType!=ALARM_NONE) {
        if (ledCnt == 0) LED_SetHigh();
        
        if ((ledCnt == 2 && ledType==ALARM_LOW) || 
            (ledCnt == 15 && ledType== ALARM_MED) ||
            (ledCnt == 15 && ledType == ALARM_HIGH) )
        {
            LED_SetLow();
        }
        if ((ledCnt == 75 && ledType==ALARM_LOW) || 
            (ledCnt == 75 && ledType== ALARM_MED) ||
            (ledCnt == 30 && ledType == ALARM_HIGH) )
        {
            ledCnt = 0;
        } else {
            ledCnt++;
        }
    } else {
        ledCnt=0;
        LED_SetLow();
    }
    
}

void SetAlarmLED(uint8_t type) {
    ledType = type;
    LED_SetHigh();
}

void ClearAlarmLED(void) {
    ledType = ALARM_NONE;
    LED_SetLow();
}

void ToggleAlarmLED(void) {
    LED_Toggle();
}
