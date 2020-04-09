#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "alarm.h"
#include "buttons.h"


#define PUSHMASK 0xF0

uint8_t muteButton=0;
uint8_t histButton=0;

void ButtonInit(void) {
    TMR1_SetInterruptHandler(ButtonTask);
    TMR1_StartTimer();
}

void ButtonTask(void) {
    muteButton = !PORTDbits.RD6 | (muteButton << 1);
    histButton = !PORTDbits.RD5 | (histButton << 1);
    if ((muteButton) == PUSHMASK) {
        MuteAlarm();
        printf("\r\nMUTE\r\n");
        muteButton = 0;
    }
    if ((histButton) == PUSHMASK) {
        HistAlarm(); 
        printf("\r\nHIST\r\n");
        histButton = 0;
    }
}

void SetAlarmLED(void) {
    PORTDbits.RD7 = 1;
}

void ClearAlarmLED(void) {
    PORTDbits.RD7 = 0;
}

void ToggleAlarmLED(void) {
    PORTDbits.RD7 = !PORTDbits.RD7;
}