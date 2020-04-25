#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"
#include "monitor.h"

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


#define STATE_INIT 0x80
#define STATE_CALP 0xC0
#define STATE_CALF 0xA0
#define STATE_STOP 0x0
#define STATE_RUN  0x10
#define STATE_RUNP 0x10
#define STATE_RUNV 0x14
#define STATE_SLEEP 0x8


void UpdateState(void) {
    static bool calib = false;
    if (msg.cntMsg.state & STATE_CALP && calib ==false) SetCalibrateState(true);
    if (!(msg.cntMsg.state & STATE_INIT) && calib ==true) SetCalibrateState(false);
    calib = (msg.cntMsg.state & STATE_CALP)==STATE_CALP;
    SetTarget(msg.cntMsg.ip & 0x3F, msg.cntMsg.ep & 0x3F, msg.cntMsg.bpm & 0x3F);
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
}

void StopHandler(void) {
    uint8_t stat0,stat1;
    stat0 = I2C2STAT0;
    stat1 = I2C2STAT1;
    //if (cntByte!=10) printf("LESS THAN 10 BYTES FROM CONTROLLER !!!");
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

void ControllerInit(void) {
    printf("Controller init\r\n");
    // Start I2C
    I2C2_Open();
    // After Open define handlers
    I2C2_SlaveSetAddrIntHandler( AddressHandler);
    // Set write interrupt handler
    I2C2_SlaveSetReadIntHandler( ReceiveHandler);
    // Set write interrupt handler
    I2C2_SlaveSetStopIntHandler( StopHandler);
}

void DumpI2C(void) {
    printf("\r\nSTAT0/1: %02X %02X\r\n", I2C2STAT0, I2C2STAT1);
    printf("ERR: %02X\r\n", I2C2ERR);
    printf("CNT: %02X\r\n", I2C2CNT);
    printf("PIR: %02X\r\n", I2C2PIR);
    
    printf("STATE %02X EP %d IP %d\r\n", msg.cntMsg.state, msg.cntMsg.ep&0x3f, msg.cntMsg.ip&0x3f);
    
}
