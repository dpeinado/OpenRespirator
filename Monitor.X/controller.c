#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "monitor.h"
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

int  cntByte;
bool commandReceived;

#define STATE_INIT 0xE0

#define STATE_STOP 0x0
#define STATE_RUN  0x10
#define STATE_RUNP 0x10
#define STATE_RUNV 0x14
#define STATE_SLEEP 0x8


void UpdateState(void) {
    if (msg.cntMsg.state & STATE_INIT) {
        int init = (msg.cntMsg.state & STATE_INIT)>>5;
        printf("\r\nINIT: %d\r\n", init);
        if (init == 5 || init==3) SetSV1(true);
        if (init == 1 || init==2 || init==4 || init==6 || init==7) SetSV1(false);
        if (init == 2) SetCalibrateState(true);
        else SetCalibrateState(false);
    }
    if (msg.cntMsg.state & STATE_RUN) SetSV1(true);
    if (msg.cntMsg.state & STATE_RUN) MonitorEnable(); else MonitorDisable();

    SetTarget(msg.cntMsg.ip & 0x3F, msg.cntMsg.ep & 0x3F, msg.cntMsg.bpm & 0x3F);
    if (msg.cntMsg.state & STATE_SLEEP) LCDOff(); else LCDOn();
}

void ReceiveHandler(void) {
    uint8_t data;
    uint8_t addr,stat0,stat1;
        
    stat0 = I2C2STAT0;
    stat1 = I2C2STAT1;
    addr = I2C2ADB0;
    if (!I2C2STAT1bits.RXBF) return;
    while (I2C2STAT1bits.RXBF) {
        //stat0 = I2C2STAT0;
        //stat1 = I2C2STAT1;
        data = I2C2_Read();
        if (cntByte<10) msg.controllerMsg[cntByte] = data;
        cntByte++;
        //printf("\r\nCD%02X : %02X %02X = %02X\r\n",addr, stat0, stat1, data);
    }
    commandReceived = true;
}

void StopHandler(void) {
    uint8_t stat0,stat1;
    stat0 = I2C2STAT0;
    stat1 = I2C2STAT1;
    if (cntByte!=10 && cntByte>0) printf("\r\nI2C: miss %d !!!\r\n", cntByte);
    //printf("\r\nSTOP: %02X %02X\r\n",stat0, stat1);
    if (cntByte!=0) {
    //    printf("\r\nCNTR: ");
    //    for (int i= 0; i< cntByte; i++) printf ("%02X ", mag.controllerMsg[i]);
    //    printf("\r\n");
    }
    cntByte = 0;
    UpdateState();
    
}

void AddressHandler(void) {
    uint8_t addr;
    addr = I2C2ADB0;
    printf("\r\nCA%02X\r\n",addr);
}

void ControllerTimerHandler(void) {
    //if (!commandReceived) printf("EC ");
    commandReceived = false;
}

void ControllerInit(void) {
    printf("Controller init\r\n");
    commandReceived = false;
    // Start I2C
    I2C2_Open();
    // After Open define handlers
    I2C2_SlaveSetAddrIntHandler( AddressHandler);
    // Set write interrupt handler
    I2C2_SlaveSetReadIntHandler( ReceiveHandler);
    // Set write interrupt handler
    I2C2_SlaveSetStopIntHandler( StopHandler);
    TMR6_SetInterruptHandler(ControllerTimerHandler);
    TMR6_StartTimer();
}

void DumpI2C(void) {
    printf("\r\nSTAT0/1: %02X %02X\r\n", I2C2STAT0, I2C2STAT1);
    printf("ERR: %02X\r\n", I2C2ERR);
    printf("CNT: %02X\r\n", I2C2CNT);
    printf("PIR: %02X\r\n", I2C2PIR);
    
    printf("STATE %02X EP %d IP %d\r\n", msg.cntMsg.state, msg.cntMsg.ep&0x3f, msg.cntMsg.ip&0x3f);
    
}
