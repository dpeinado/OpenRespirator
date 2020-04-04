#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "alarm.h"


#define PUSHMASK 0x3F

uint8_t muteButton=0;
uint8_t histButton=0;

void ButtonTask(void) {
    muteButton = PORTDbits.RD6 | (muteButton << 1);
    histButton = PORTDbits.RD5 | (histButton << 1);
    if (muteButton & PUSHMASK == PUSHMASK) muteAlarm();
    if (histButton & PUSHMASK == PUSHMASK) histAlarm();   
}

void SetAlarmLED(void) {
    PORTDbits.RD7 = 1;
}

void ClearAlarmLED(void) {
    PORTDbits.RD7 = 0;
}