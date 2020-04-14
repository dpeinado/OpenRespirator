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

char msg3[17]="";

bool calibrate;

void InitDisplay(void) {
    LCDInit();
    calibrate=false;
}

void DisplayCalibrate(int16_t pr, int16_t off) {
    calibrate=true;
    sprintf(msg2, "Pr: %2d Of: %4d", pr, off);
}

void UnDisplayCalibrate(void) {
    calibrate=false;
    msg2[0]=0;
};

void ValueDisplay() {
    int TR =0;
    
    if (TR>100) TR=100;
    if (TR<0)   TR=0;
    
    uint16_t tdi = GetTdi();
    uint16_t tde = GetTde();
    uint16_t ti = GetTi();
    uint16_t te = GetTe();
    uint8_t eti = ti/1000;
    uint8_t dti = (ti-((uint16_t) eti)*1000)/10;
    uint8_t ete = te/1000;
    uint8_t dte = (te-((uint16_t) ete)*1000)/10;
    uint16_t bp = GetBp();
    int16_t pi = GetIp();
    int16_t pe = GetEp();
    uint8_t etdi = tdi/1000;
    uint8_t dtdi = (tdi-((uint16_t) etdi)*1000)/10;
    uint8_t etde = tde/1000;
    uint8_t dtde = (tde-((uint16_t) etde)*1000)/10;
    
    printf("\r%2d%% %d.%02d %d.%02d - %d/%d  %u:%u/%u ms       ", TR, etdi,dtdi, etde, dtde, pi, pe, bp, ti, te);
    //sprintf(msg, "%2d%% %d.%02d %d.%02d       ", TR, etdi, dtdi, etde, dtde);
    sprintf(msg1, "%2d%% %d.%02d %d.%02d %s", TR, etdi,dtdi, etde, dtde, calibrate? "Ca" : GetAlarmState() );
//                   1234 5678  9 0123  456  
    sprintf(msg3, "%2d/%2d %d.%02d/%d.%02d ", pi, pe, eti, dti, ete, dte);
//                  123 456 7890  1 2345  6
}   

void AlarmDisplay(int type, char *alarm) {
    char t = (type==ALARM_HIGH) ? 'H' : ((type==ALARM_MED) ? 'M' : 'L');
    if (msg2[0]==0) {
        sprintf(msg2, "%c %14s", t, alarm);
        printf("\r%c %14s", t, alarm);
    }
    ValueDisplay();
}

void DisplayTask(void) {
    if (msg1[0] && msg2[0]) {
        LCDMessage12(msg1,msg2);
        if (calibrate == false) msg2[0]=0;
        msg1[0]=0;
        return;
    }

    if (msg1[0] && msg3[0]) {
        LCDMessage12(msg1,msg3);
        msg1[0]=0;
        msg3[0]=0;
        return;
    }
    if (msg2[0]) {
        LCDMessage2(msg2);
        if (calibrate == false) msg2[0]=0;
        return;
    }
    LCDMessage1(msg1);
}