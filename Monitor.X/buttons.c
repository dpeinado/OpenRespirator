#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "alarm.h"
#include "buttons.h"
#include "monitor.h"


#define PUSHMASK 0xF0
#define PUSHLONGMASK 0xFF

static uint8_t muteButton=0;
static uint8_t histButton=0;

void ButtonInit(void) {
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
        printf("\r\nDISABLE SV1 ALARM!!!!!\r\n");
    }
    
}

void SetAlarmLED(void) {
    LED_SetHigh();
}

void ClearAlarmLED(void) {
    LED_SetLow();
}

void ToggleAlarmLED(void) {
    LED_Toggle();
}
