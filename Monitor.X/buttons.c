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
            (ledCnt == 75/2 && ledType== ALARM_MED) ||
            (ledCnt == 15 && ledType == ALARM_HIGH) )
        {
            LED_SetLow();
        }
        if ((ledCnt == 4 && ledType==ALARM_LOW) || 
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
