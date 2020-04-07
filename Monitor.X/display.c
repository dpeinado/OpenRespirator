#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include "lcd.h"
#include "alarm.h"
#include "monitor.h"


void InitDisplay(void) {
    LCDInit();
}

void ValueDisplay() {
    char msg[16];
    
    int TR =0;
    
    if (TR>100) TR=100;
    if (TR<0)   TR=0;
    
    uint32_t tdi = GetTdi();
    uint32_t tde = GetTde();
    
    sprintf(msg, "%2d%% %lu %lu", TR, tdi, tde);
    LCDMessage(msg);
}
void AlarmDisplay(int type, char *alarm) {
    char msg[16];
    char t = (type==ALARM_HIGH) ? 'H' : ((type==ALARM_MED) ? 'M' : 'L');
    
    sprintf(msg, "%c %14s", t, alarm);
    LCDMessage(msg);
}