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
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "monitor.h"
#include "display.h"
#include "buzzer.h"
#include "alarm.h"
#include "lcd.h"

struct message {
    uint8_t state;
    uint8_t ip;
    uint8_t ep;
    uint8_t bpm;
    uint8_t pmax;
    uint8_t v;
    uint8_t vlow;
    uint8_t vhigh;
    uint8_t spr;
    uint8_t alarm;
};

//uint8_t controllerMsg[10];

union { struct message cntMsg; uint8_t controllerMsg[10]; } msg;

static int  cntByte;
static bool commandReceived;

#define STATE_INIT 0xE0

#define STATE_STOP 0x0
#define STATE_RUN  0x10
#define STATE_RUNP 0x10
#define STATE_RUNV 0x14
#define STATE_SLEEP 0x8

void StopHandler(void);
static int16_t spr = 0;
int16_t GetSpr(void) { return spr; }
static bool init;
static bool run;
static bool volumeControl;

void UpdateState(void) {
    static uint8_t old;
    //if (msg.cntMsg.state!=old) printf("\r\nNew state: %02X\r\n", msg.cntMsg.state);
    old= msg.cntMsg.state;
    init = (msg.cntMsg.state & STATE_INIT);
    if (init) {
        int initPhase = (msg.cntMsg.state & STATE_INIT)>>5;
        printf("\r\nINIT: %d\r\n", initPhase);
        if (initPhase == 5 || initPhase==3) SetSV1(true);
        if (initPhase == 1 || initPhase==2 || initPhase==4 || initPhase==6 || initPhase==7) SetSV1(false);
        if (initPhase == 2) SetCalibrateState(true); else SetCalibrateState(false);
        
        if (initPhase == 5) {
            //printf("\r\nFlow: %d pa Pressure: %d pa\r\n", GetPressureV_pa(), GetPressure_pa());
        }
        
        // Check of Controller Buzzer
        if (initPhase == 2) { // Clear Check
            GetControllerBuzzerCheck();
        }
         if (initPhase == 3) { // Do Check
            ControllerBuzzerCheck();
         }
         if (initPhase == 4) { // Check result
            if (!GetControllerBuzzerCheck()) {
                printf("\r\n Error in Controller buzzer\r\n");
                SetControlFailAlarm();
            } else {
                printf("\r\n Controller buzzer detected\r\n");
            }
         }
        
        EnableAlarmSV1();
    }
    if (msg.cntMsg.state & STATE_SLEEP) {
        LCDOff();
        ClearAllAlarm();
        MonitorDisable();
        DisplayDisable();
        printf("\r\n SLEEP MODE\r\n");
    } else {
        LCDOn();
    }
    
    run = (msg.cntMsg.state & STATE_RUN) == STATE_RUN;
    //if (run) printf("R\r\n");
    
    if (run) SetSV1(true);
    if (run) MonitorEnable(); else MonitorDisable();
    if (run) DisplayEnable(); else DisplayDisable();
    volumeControl = (msg.cntMsg.state & STATE_RUNV) == STATE_RUNV;
    SetVolumeControl(volumeControl);
    
    SetTarget(msg.cntMsg.pmax & 0x7F, msg.cntMsg.ip & 0x3F, msg.cntMsg.ep & 0x3F, msg.cntMsg.bpm & 0x3F, (msg.cntMsg.vhigh&0x7F)*20, (msg.cntMsg.vlow&0x7F)*20);
    SetMaxPressure(msg.cntMsg.pmax & 0x7F);
    

    spr = msg.cntMsg.spr;
    
    static int cnt=0;
    if (msg.cntMsg.alarm & 0x80) {
        //printf("\r\nControl Alarm: %02X\r\n", msg.cntMsg.alarm);
        
        cnt++;
        if (cnt>10) SetControlFailAlarm();
    } else {
        ClearControlFailAlarm();
        cnt = 0;
    }
}

void ReceiveHandler(void) {
    uint8_t data;
    uint8_t addr,stat0,stat1;
    //I2C2_SendAck();
    //data = I2C2_Read();
    //printf("\r\nD:%02X\r\n", data);
    //return;
        
    stat0 = I2C2STAT0;
    stat1 = I2C2STAT1;
    addr = I2C2ADB0;
    if (!I2C2STAT1bits.RXBF) return;
    while (I2C2STAT1bits.RXBF) {
        //I2C2_SendAck();
        //stat0 = I2C2STAT0;
        //stat1 = I2C2STAT1;
        data = I2C2_Read();
        if (cntByte<10) msg.controllerMsg[cntByte] = data;
        cntByte++;
        //printf("\r\nCD%02X : %02X %02X = %02X\r\n",addr, stat0, stat1, data);
    }
    commandReceived = true;
    //if (cntByte>=10) UpdateState();
}

void StopHandler(void) {
    uint8_t stat0,stat1;
    stat0 = I2C2STAT0;
    stat1 = I2C2STAT1;
    //if (I2C2STAT1bits.RXBF) ReceiveHandler();
    if (cntByte!=10 && cntByte>0) {
        ReceiveHandler();
        printf("I2C: miss %d\r\n", cntByte);
    }
    //printf("\r\nSTOP: %02X %02X C:%d\r\n",stat0, stat1, cntByte);
    if (cntByte!=0) {
    //    printf("\r\nCNTR: ");
    //    for (int i= 0; i< cntByte; i++) printf ("%02X ", mag.controllerMsg[i]);
    //    printf("\r\n");
    }
    
    if (cntByte==10) {
    //    printf("\r\nCNTR: ");
    //    for (int i= 0; i< cntByte; i++) printf ("%02X ", msg.controllerMsg[i]);
    //    printf("\r\n");
        UpdateState();
    }
    cntByte = 0;
    
}

void AddressHandler(void) {
    uint8_t addr;
    addr = I2C2ADB0;
    //printf("\r\nCA%02X\r\n",addr);
}

void ControllerTimerHandler(void) {
    static int errors=0;
    if (!commandReceived) {
        //
        if (run) {
            printf("EC ");
            errors++;
            if (errors>10) SetControlFailAlarm();
        } else {
            ClearControlFailAlarm();
        }
    } else {
        //ClearControlFailAlarm();
        errors = 0;
    }
    static int16_t cnt=0;
    cnt = (cnt+1)%100;
    if (cnt==0) printf("&\r\n");
    commandReceived = false;
}

void ControllerInit(void) {
    //printf("Controller init\r\n");
    commandReceived = false;
    init = false;
    run = false;
            
    // Start I2C
    I2C2_Open();
    // After Open define handlers
    //I2C2_SlaveSetAddrIntHandler( AddressHandler);
    // Set write interrupt handler
    I2C2_SlaveSetReadIntHandler( ReceiveHandler);
    // Set write interrupt handler
    I2C2_SlaveSetStopIntHandler( StopHandler);
    TMR6_SetInterruptHandler(ControllerTimerHandler);
    TMR6_StartTimer();
}

void DumpI2C(void) {
    //printf("\r\nSTAT0/1: %02X %02X\r\n", I2C2STAT0, I2C2STAT1);
    //printf("ERR: %02X\r\n", I2C2ERR);
    //printf("CNT: %02X\r\n", I2C2CNT);
    //printf("PIR: %02X\r\n", I2C2PIR);
    
    //printf("STATE %02X EP %d IP %d\r\n", msg.cntMsg.state, msg.cntMsg.ep&0x3f, msg.cntMsg.ip&0x3f);
    
}
