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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "display.h"
#include "lcd.h"
#include "alarm.h"
#include "monitor.h"
#include "controller.h"

char msg1[20]="";
char msg2[20]="";

char msg3[20]="";
char msg4[20]="";

static bool calibrate;
static bool enable = false;

void InitDisplay(void) {
    LCDInit();
    calibrate=false;
    //LCDMessage12("Open Respirator ","    OxyVita    ");
}

void DisplayEnable(void) {   
    if (enable == false) ValueDisplay();
    enable = true;
}

void DisplayDisable(void) {
    if (enable == true) ValueDisplay();
    enable = false;
}

void DisplayCalibrate(int16_t pr, int16_t off, int16_t prv, int16_t offv) {
    calibrate=true;
    sprintf(msg4, "Cal: %4d - %4d", off, offv);
}

void UnDisplayCalibrate(void) {
    calibrate=false;
    msg4[0]=0;
};

void ValueDisplay(void) {
    if (!enable) { //                  1234567890123456
        if (msg1[0]==0) sprintf(msg1, "Open  Respirator");
        if (msg2[0]==0) sprintf(msg2, "    OxyVita     ");
        return;
    }
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
    int16_t r;
    if (ti!=0) r = (te)/(ti/10);
    else r = 0;
    int8_t er = r/10;
    int8_t dr = (r-((int16_t) er)*10);
    int16_t tip = GetTargetIp();
    int16_t tep = GetTargetEp();
    int16_t vol = GetVolume();
    int16_t pmax = GetPmax();
    int16_t spr = GetSpr();
    
    //printf("\rTR:%2d%% TDE:%d %d.%02d TDI:%d %d.%02d EP/IP:%d(%d)/%d(%d)  BPM: %d TE/TI:%d/%d ms       ", TR, tde, etde,dtde, tdi, etdi, dtdi, pe, tep, pi, tip, bpm, te, ti);
    //sprintf(msg, "%2d%% %d.%02d %d.%02d       ", TR, etdi, dtdi, etde, dtde);
    sprintf(msg1, "%2d%% %d.%02d %d.%02d%c%s", spr, etde,dtde, etdi, dtdi, GetDisableAlarmSV1() ? 'd':' ', calibrate? "Ca" : GetAlarmState() );
//                   1234 5678  9 01 23  456  
    sprintf(msg3, "%1d%1d %2d %2d %2d %4d", er, dr, pe, pi, pmax, vol);
//                   12  34 567 890 123456
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
    
    if (msg1[0] && msg2[0]) {
        //printf("\r\nData+Alarm \"%s\" \"%s\"\r\n", msg1, msg2);
        LCDMessage12(msg1,msg2);
        msg2[0]=0;
        msg1[0]=0;
        return;
    }
    
    if (msg4[0]) {
        //printf("\r\nData+Data \"%s\" \"%s\"\r\n", msg1, msg3);
        LCDMessage2(msg4);
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