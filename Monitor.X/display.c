#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include "lcd.h"
#include "alarm.h"
#include "monitor.h"

char msg1[17]="";
char msg2[17]="";

void InitDisplay(void) {
    LCDInit();
}

void ValueDisplay() {
    char msg[16];
    
    int TR =0;
    
    if (TR>100) TR=100;
    if (TR<0)   TR=0;
    
    uint16_t tdi = GetTdi();
    uint16_t tde = GetTde();
    uint16_t ti = GetTi();
    uint16_t te = GetTe();
    uint16_t bp = GetBp();
    uint8_t pi = GetPi();
    uint8_t pe = GetPe();
    uint8_t etdi = tdi/1000;
    uint8_t dtdi = (tdi-((uint16_t) etdi)*1000)/10;
    uint8_t etde = tde/1000;
    uint8_t dtde = (tde-((uint16_t) etde)*1000)/10;
    
    printf("\r%2d%% %d.%02d %d.%02d - %d/%d  %u:%u/%u ms       ", TR, etdi,dtdi, etde, dtde, pi, pe, bp, ti, te);
    //sprintf(msg, "%2d%% %d.%02d %d.%02d       ", TR, etdi, dtdi, etde, dtde);
    sprintf(msg1, "%2d%% %d.%02d %d.%02d", TR, etdi,dtdi, etde, dtde ); 
}
void AlarmDisplay(int type, char *alarm) {
    char msg[16];
    char t = (type==ALARM_HIGH) ? 'H' : ((type==ALARM_MED) ? 'M' : 'L');
    
    sprintf(msg2, "%c %14s", t, alarm);
    printf("\r%c %14s", t, alarm);
}

void DisplayTask(void) {
    if (msg1[0]) {
        LCDMessage1(msg1);
        msg1[0]=0;
    }
    if (msg2[0]) {
        LCDMessage2(msg2);
        msg2[0]=0;
    }
}