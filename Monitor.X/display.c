#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include "lcd.h"
#include "alarm.h"
#include "monitor.h"
#include "controller.h"

char msg1[20]="";
char msg2[20]="";

char msg3[20]="";
char msg4[20]="";

bool calibrate;
bool enable = false;

void InitDisplay(void) {
    LCDInit();
    calibrate=false;
}

void DisplayEnable(void) {
    enable = true;
}

void DisplayDisable(void) {
    //                        1234567890123456   1234567890123456
    if (enable) LCDMessage12("Open Respirator ","    AirVita    ");
    enable = false;
}

void DisplayCalibrate(int16_t pr, int16_t off) {
    calibrate=true;
    sprintf(msg4, "Pr: %2d Of: %4d", pr, off);
}

void UnDisplayCalibrate(void) {
    calibrate=false;
    msg4[0]=0;
};

void ValueDisplay(void) {
    static int cnt=0;
    
    int16_t tdi = GetTdi();
    int16_t tde = GetTde();
    int16_t ti = GetTi();
    int16_t te = GetTe();
    int8_t eti = ti/1000;
    int8_t dti = (ti-((int16_t) eti)*1000)/10;
    int8_t ete = te/1000;
    int8_t dte = (te-((int16_t) ete)*1000)/10;
    int16_t bpm = GetBpm();
    int16_t pi = GetIp();
    int16_t pe = GetEp();
    int8_t etdi = tdi/1000;
    int8_t dtdi = (tdi-((int16_t) etdi)*1000)/10;
    int8_t etde = tde/1000;
    int8_t dtde = (tde-((int16_t) etde)*1000)/10;
    int16_t r = (te)/(ti/10);
    int8_t er = r/10;
    int8_t dr = (r-((int16_t) er)*10);
    int16_t tip = GetTargetIp();
    int16_t tep = GetTargetEp();
    int16_t vol = GetVolume();
    int16_t pmax = GetPmax();
    int16_t spr = GetSpr();
    
    //printf("\rTR:%2d%% TDE:%d %d.%02d TDI:%d %d.%02d EP/IP:%d(%d)/%d(%d)  BPM: %d TE/TI:%d/%d ms       ", TR, tde, etde,dtde, tdi, etdi, dtdi, pe, tep, pi, tip, bpm, te, ti);
    //sprintf(msg, "%2d%% %d.%02d %d.%02d       ", TR, etdi, dtdi, etde, dtde);
    sprintf(msg1, "%2d%% %d.%02d %d.%02d %s", spr, etde,dtde, etdi, dtdi, calibrate? "Ca" : GetAlarmState() );
//                   1234  56 78  90  12  3456  
    sprintf(msg3, "%2d %2d %2d %3d %1d.%1d", pe, pi, pmax, vol, er, dr);
//                  123 456 7890123 456
    cnt++;
    cnt=cnt%10;
}   

void AlarmDisplay(int type, char *alarm) {
    static int cnt = 0;
    char t = (type==ALARM_HIGH) ? 'H' : ((type==ALARM_MED) ? 'M' : 'L');
    //if (msg2[0]==0) {
        sprintf(msg2, "%c %14s", t, alarm);
        //printf("\r\n%1d %c %14s\r\n", cnt, t, alarm);
    //}
    ValueDisplay();
    cnt++;
    cnt = cnt%10;
}

void DisplayTask(void) {
    if (!enable) return;
    
    if (msg1[0] && msg2[0]) {
        //printf("\r\nData+Alarm \"%s\" \"%s\"\r\n", msg1, msg2);
        LCDMessage12(msg1,msg2);
        msg2[0]=0;
        msg1[0]=0;
        return;
    }
    
    if (msg4[0]) {
        //printf("\r\nData+Data \"%s\" \"%s\"\r\n", msg1, msg3);
        LCDMessage1(msg4);
        return;
    }
    
    if (msg2[0]) {
        //printf("\r\nAlarm \"%s\\r\n", msg2);
        LCDMessage2(msg2);
        msg2[0]=0;
        return;
    }
    
    if (msg1[0] && msg3[0]) {
        //printf("\r\nData+Data \"%s\" \"%s\"\r\n", msg1, msg3);
        LCDMessage12(msg1,msg3);
        msg1[0]=0;
        msg3[0]=0;
        return;
    }

    
    if (msg1[0]) {
        //printf("\r\nData \"%s\"\r\n", msg1);
        //printf("\r\nOnly one line\r\n");
        LCDMessage1(msg1);
        msg1[0]=0;
    }
}