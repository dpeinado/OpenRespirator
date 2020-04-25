#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mcc_generated_files/mcc.h"

uint8_t controllerMsg[10];
int  cntByte;

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
        if (cntByte<10) controllerMsg[cntByte] = data;
        cntByte++;
        //printf("\r\nCD%02X : %02X %02X = %02X\r\n",addr, stat0, stat1, data);
    }
    //I2C2CON0bits.CSTR = 0;
}

void StopHandler(void) {
    uint8_t stat0,stat1;
    stat0 = I2C2STAT0;
    stat1 = I2C2STAT1;
    //printf("\r\nSTOP: %02X %02X\r\n",stat0, stat1);
    if (cntByte!=0) {
    //    printf("\r\nCNTR: ");
    //    for (int i= 0; i< cntByte; i++) printf ("%02X ", controllerMsg[i]);
    //    printf("\r\n");
    }
    cntByte = 0;
    
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
    printf("ERR: %02X %02X\r\n", I2C2ERR);
    printf("CNT: %02X %02X\r\n", I2C2CNT);
    printf("PIR: %02X %02X\r\n", I2C2PIR);
}
