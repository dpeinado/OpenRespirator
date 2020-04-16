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
    
    int16_t tdi = GetTdi();
    int16_t tde = GetTde();
    int16_t ti = GetTi();
    int16_t te = GetTe();
    int8_t eti = ti/1000;
    int8_t dti = (ti-((uint16_t) eti)*1000)/10;
    int8_t ete = te/1000;
    int8_t dte = (te-((uint16_t) ete)*1000)/10;
    int16_t bpm = GetBpm();
    int16_t pi = GetIp();
    int16_t pe = GetEp();
    int8_t etdi = tdi/1000;
    int8_t dtdi = (tdi-((uint16_t) etdi)*1000)/10;
    int8_t etde = tde/1000;
    int8_t dtde = (tde-((uint16_t) etde)*1000)/10;
    
    printf("\r%2d%% %d %d.%02d %d %d.%02d - %d/%d  %u:%u/%u ms       ", TR, tde, etde,dtde, tdi, etdi, dtdi, pe, pi, bpm, te, ti);
    //sprintf(msg, "%2d%% %d.%02d %d.%02d       ", TR, etdi, dtdi, etde, dtde);
    sprintf(msg1, "%2d%% %d.%02d %d.%02d %s", TR, etde,dtde, etdi, dtdi, calibrate? "Ca" : GetAlarmState() );
//                   1234 5678  9 0123  456  
    sprintf(msg3, "%2d %2d %2d    600 ", bpm, pe, pi);
//                  123 456 7890123456
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