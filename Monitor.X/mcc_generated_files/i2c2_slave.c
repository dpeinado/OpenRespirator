/**
  I2C2 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    i2c2_slave.c

  @Summary
    This is the generated driver implementation file for the I2C2 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides implementations for driver APIs for I2C2.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.0
        Device            :  PIC18F46K42
        Driver Version    :  1.0.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.10 and above or later
        MPLAB             :  MPLAB X 5.35
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "i2c2_slave.h"
#include <xc.h>
#include <stdio.h>

#define I2C2_SLAVE_ADDRESS      0x50 // 0x27 // 80
#define I2C2_SLAVE_MASK         0

/**
 Section: Global Variables
 */
typedef enum 
{
    I2C2_ADDR, 
    I2C2_TX, 
    I2C2_RX
} i2c2_state_t;

static void I2C2_Isr(void);
static void I2C2_SlaveDefRdInterruptHandler(void);
static void I2C2_SlaveDefWrInterruptHandler(void);
static void I2C2_SlaveDefStopInterruptHandler(void);
static void I2C2_SlaveDefAddrInterruptHandler(void);
static void I2C2_SlaveDefWrColInterruptHandler(void);
static void I2C2_SlaveDefBusColInterruptHandler(void);

static void I2C2_SlaveRdCallBack(void);
static void I2C2_SlaveWrCallBack(void);
static void I2C2_SlaveStopCallBack(void);
static void I2C2_SlaveAddrCallBack(void);
static void I2C2_SlaveWrColCallBack(void);
static void I2C2_SlaveBusColCallBack(void);

static inline bool I2C2_SlaveOpen();
static inline void I2C2_SlaveClose();
static inline void I2C2_SlaveSetSlaveAddr(uint8_t slaveAddr);
static inline void I2C2_SlaveSetSlaveMask(uint8_t maskAddr);
static inline void I2C2_SlaveEnableIrq(void);
static inline bool I2C2_SlaveIsAddr(void);
static inline bool I2C2_SlaveIsRead(void);
static inline void I2C2_SlaveClearAddrFlag(void);
static inline void I2C2_SlaveClearStartFlag(void);
static inline bool I2C2_SlaveIsStop(void);
static inline bool I2C2_SlaveIsForMe(void);
static inline void I2C2_SlaveClearBuff(void);
static inline void I2C2_SlaveClearIrq(void);
static inline void I2C2_SlaveSetCounter(uint8_t counter);
static inline void I2C2_SlaveReleaseClock(void);
static inline bool I2C2_SlaveIsWriteCollision(void);
static inline bool I2C2_SlaveIsTxBufEmpty(void);
static inline bool I2C2_SlaveIsData(void);
static inline bool I2C2_SlaveIsRxBufFull(void);
static inline void I2C2_SlaveSendTxData(uint8_t data);
static inline uint8_t I2C2_SlaveGetRxData(void);
static inline uint8_t I2C2_SlaveGetAddr(void);
static inline void I2C2_SlaveSendAck(void);
static inline void I2C2_SlaveSendNack(void);
static inline bool I2C2_SlaveIsNack(void);
static inline void I2C2_SlaveClearNack(void);

static volatile i2c2_state_t i2c2State = I2C2_ADDR;
volatile uint8_t i2c2WrData;
volatile uint8_t i2c2RdData;
volatile uint8_t i2c2SlaveAddr;

void I2C2_Initialize()
{
    //EN disabled; RSEN disabled; S Cleared by hardware after Start; CSTR Enable clocking; MODE four 7-bit address; 
    I2C2CON0 = 0x00;
    //ACKCNT Acknowledge; ACKDT Acknowledge; ACKSTAT ACK received; ACKT 0; RXO 0; TXU 0; CSD Clock Stretching enabled; 
    I2C2CON1 = 0x00;
    //ACNT disabled; GCEN enabled; FME disabled; ABD enabled; SDAHT 300 ns hold time; BFRET 8 I2C Clock pulses; 
    I2C2CON2 = 0x00;
    //CNT 0; 
    I2C2CNT = 0xFF;
    //I2C2_Open();
}

void I2C2_Open() 
{
    I2C2_SlaveOpen();
    I2C2_SlaveSetSlaveAddr(I2C2_SLAVE_ADDRESS << 1);
    printf("I2C Open %02X\r\n", I2C2ADR0);
    I2C2_SlaveSetSlaveMask(I2C2_SLAVE_MASK);
    I2C2_SlaveSetIsrHandler(I2C2_Isr);
    I2C2_SlaveSetBusColIntHandler(I2C2_SlaveDefBusColInterruptHandler);
    I2C2_SlaveSetWriteIntHandler(I2C2_SlaveDefWrInterruptHandler);
    I2C2_SlaveSetStopIntHandler(I2C2_SlaveDefStopInterruptHandler);
    I2C2_SlaveSetReadIntHandler(I2C2_SlaveDefRdInterruptHandler);
    I2C2_SlaveSetAddrIntHandler(I2C2_SlaveDefAddrInterruptHandler);
    I2C2_SlaveSetWrColIntHandler(I2C2_SlaveDefWrColInterruptHandler);
    I2C2_SlaveEnableIrq();    
}

void I2C2_Close() 
{
    I2C2_SlaveClose();
}

uint8_t I2C2_Read()
{
   return I2C2_SlaveGetRxData();
}

void I2C2_Write(uint8_t data)
{
    I2C2_SlaveSendTxData(data);
}

void I2C2_Enable()
{
    I2C2_Initialize();
}

void I2C2_SendAck()
{
    I2C2_SlaveSendAck();
}

void I2C2_SendNack()
{
    I2C2_SlaveSendNack();
}

static void I2C2_Isr() 
{ 
//   printf("-I2C-");
    if (I2C2_SlaveIsAddr()) 
    {
        //uint8_t addr,stat0,stat1;
        
        //stat0 = I2C2STAT0;
        //stat1 = I2C2STAT1;
        //addr = I2C2ADB0;
        //printf("A%02X\r\n",addr);
        //printf("\r\nI2C addr\r\n");
        if(I2C2_SlaveIsRead())
        {
 //           printf("\r\nI2C Read\r\n");
            i2c2State = I2C2_TX;
        }
        else
        {
 //           printf("\r\nI2C Wr\r\n");
            i2c2State = I2C2_RX;
        }
        I2C2_SlaveClearAddrFlag();
        I2C2_SlaveClearStartFlag();
        
        //uint8_t addr;
        //addr = I2C2ADB0;
        //if (addr == I2C2_SLAVE_ADDRESS<<1) {
        //    I2C2_SlaveAddrCallBack();
        //}
        
        //    I2C2_SlaveClearIrq();
            //printf("I2C Me %02x %02X\r\n", I2C2STAT0, I2C2STAT1);
        //} else {
            
            //uint8_t addr;
            //addr = I2C2ADB0;
            //printf("A%02X\r\n",addr);
            //printf("I2C No\r\n");
            //I2C2_SlaveClearBuff();
            //I2C2_SlaveClearIrq();
            //I2C2_SlaveSetCounter(0xFF);
            //I2C2_SlaveReleaseClock();
            //i2c2State = I2C2_ADDR;           
        //}
    }
    


    if(I2C2_SlaveIsNack())
    {
 //       printf("\r\nI2C NACK\r\n");
        I2C2_SlaveClearNack();
        I2C2_SlaveClearBuff();
        I2C2_SlaveReleaseClock();
        return;
    } 
    
    switch(i2c2State)
    {
        case I2C2_TX:
            if(!I2C2_SlaveIsWriteCollision())
            {    
                if(I2C2_SlaveIsTxBufEmpty())
                {    
                    I2C2_SlaveWrCallBack();
                }    
            }
            else
            {   
                I2C2_SlaveWrColCallBack();
            }
            break;
           
        case I2C2_RX:
        {
            
            //uint8_t data;   
            //data = I2C2RXB;
            //printf("D%02X\r\n",data);
            //uint8_t addr;
            //addr = I2C2ADB0;
            //printf("A%02X\r\n",addr);
            if (I2C2_SlaveIsData()) 
            {

                if(I2C2_SlaveIsRxBufFull())
                { 
                    I2C2_SlaveRdCallBack();
                }    
            } else {

            }
            break;
        }
        default:          
            break;
    }
    
    if(I2C2_SlaveIsStop())
    {
 //       printf("\r\nI2C STOP\r\n");
        if(I2C2_SlaveIsRxBufFull())
        { 
            I2C2_SlaveRdCallBack();
        }    
        I2C2_SlaveClearBuff();
        I2C2_SlaveClearIrq();
        I2C2_SlaveSetCounter(0xFF);
        I2C2_SlaveReleaseClock();
        i2c2State = I2C2_ADDR;
        I2C2_SlaveStopCallBack();
        return;
    }
    
    I2C2_SlaveReleaseClock();
}

// Common Event Interrupt Handlers
void I2C2_SlaveSetIsrHandler(interruptHandler handler)
{
    I2C2_InterruptHandler = handler;
}

// Read Event Interrupt Handlers
void I2C2_SlaveSetReadIntHandler(interruptHandler handler) {
    I2C2_SlaveRdInterruptHandler = handler;
}

static void I2C2_SlaveRdCallBack() {
    // Add your custom callback code here
    if (I2C2_SlaveRdInterruptHandler) 
    {
        I2C2_SlaveRdInterruptHandler();
    }
}

static void I2C2_SlaveDefRdInterruptHandler() {
    i2c2RdData = I2C2_SlaveGetRxData();
}

// Write Event Interrupt Handlers
void I2C2_SlaveSetWriteIntHandler(interruptHandler handler) {
    I2C2_SlaveWrInterruptHandler = handler;
}

static void I2C2_SlaveWrCallBack() {
    // Add your custom callback code here
    if (I2C2_SlaveWrInterruptHandler) 
    {
        I2C2_SlaveWrInterruptHandler();
    }
}

static void I2C2_SlaveDefWrInterruptHandler() {
    I2C2_SlaveSendTxData(i2c2WrData);
}

// Stop Event Interrupt Handlers
void I2C2_SlaveSetStopIntHandler(interruptHandler handler) {
    I2C2_SlaveStopInterruptHandler = handler;
}

static void I2C2_SlaveStopCallBack() {
    // Add your custom callback code here
    if (I2C2_SlaveStopInterruptHandler) 
    {
        I2C2_SlaveStopInterruptHandler();
    }
}

static void I2C2_SlaveDefStopInterruptHandler() {

}
// ADDRESS Event Interrupt Handlers
void I2C2_SlaveSetAddrIntHandler(interruptHandler handler){
    I2C2_SlaveAddrInterruptHandler = handler;
}

static void I2C2_SlaveAddrCallBack() {
    // Add your custom callback code here
    if (I2C2_SlaveAddrInterruptHandler) {
        I2C2_SlaveAddrInterruptHandler();
    }
}

static void I2C2_SlaveDefAddrInterruptHandler() {
    i2c2SlaveAddr = I2C2_SlaveGetAddr();
}

// Write Collision Event Interrupt Handlers
void I2C2_SlaveSetWrColIntHandler(interruptHandler handler){
    I2C2_SlaveWrColInterruptHandler = handler;
}

static void  I2C2_SlaveWrColCallBack() {
    // Add your custom callback code here
    if ( I2C2_SlaveWrColInterruptHandler) 
    {
         I2C2_SlaveWrColInterruptHandler();
    }
}

static void I2C2_SlaveDefWrColInterruptHandler() {
}

// Bus Collision Event Interrupt Handlers
void I2C2_SlaveSetBusColIntHandler(interruptHandler handler){
    I2C2_SlaveBusColInterruptHandler = handler;
}

static void  I2C2_SlaveBusColCallBack() {
    // Add your custom callback code here
    if ( I2C2_SlaveBusColInterruptHandler) 
    {
         I2C2_SlaveBusColInterruptHandler();
    }
}

static void I2C2_SlaveDefBusColInterruptHandler() {
}

static inline bool I2C2_SlaveOpen()
{
    if(!I2C2CON0bits.EN)
    {  
        I2C2CON0bits.EN = 1;
        return true;
    }
    return false;
}

static inline void I2C2_SlaveClose()
{
    I2C2CON0bits.EN = 0;
    I2C2PIR = 0x00;
    I2C2STAT1bits.CLRBF = 1;
}

static inline void I2C2_SlaveSetSlaveAddr(uint8_t slaveAddr)
{
    I2C2ADR0 = slaveAddr;
}

static inline void I2C2_SlaveSetSlaveMask(uint8_t maskAddr)
{
    I2C2ADR1 = ~maskAddr;
}

static inline void I2C2_SlaveEnableIrq()
{
    PIE6bits.I2C2IE    = 1;
    PIE6bits.I2C2EIE   = 1;
    PIE5bits.I2C2RXIE  = 1;
    PIE5bits.I2C2TXIE  = 1;
    I2C2PIEbits.PCIE = 1; 
    I2C2PIEbits.ADRIE = 1; 
    I2C2PIEbits.WRIE = 1; 
    I2C2ERRbits.NACKIE = 1; 


}

static inline bool I2C2_SlaveIsAddr()
{
    return !(I2C2STAT0bits.D); 
}

static inline bool I2C2_SlaveIsRead()
{
    return I2C2STAT0bits.R; 
}

static inline bool I2C2_SlaveIsForMe()
{
    return I2C2STAT0bits.SMA; 
}

static inline void I2C2_SlaveClearAddrFlag()
{
    I2C2PIRbits.ADRIF = 0; 
}

static inline void I2C2_SlaveClearStartFlag()
{
    I2C2PIRbits.SCIF = 0; 
}

static inline bool I2C2_SlaveIsStop()
{
    return I2C2PIRbits.PCIF; 
}

static inline void I2C2_SlaveClearBuff()
{
    I2C2STAT1bits.CLRBF = 1; 
}

static inline void I2C2_SlaveClearIrq()
{
    I2C2PIR = 0x00; 
}

static inline void I2C2_SlaveSetCounter(uint8_t counter)
{
    I2C2CNT = counter;
}

static inline void I2C2_SlaveReleaseClock()
{
    I2C2CON0bits.CSTR = 0;
}

static inline bool I2C2_SlaveIsWriteCollision()
{
    return I2C2ERRbits.BCLIF;
}

static inline bool I2C2_SlaveIsTxBufEmpty()
{
    return I2C2STAT1bits.TXBE;
}

static inline bool I2C2_SlaveIsData()
{
    return I2C2STAT0bits.D;
}

static inline bool I2C2_SlaveIsRxBufFull()
{
    return I2C2STAT1bits.RXBF;
}

static inline void I2C2_SlaveSendTxData(uint8_t data)
{
    I2C2TXB = data;
}

static inline uint8_t I2C2_SlaveGetRxData()
{
    return I2C2RXB;
}

static inline uint8_t I2C2_SlaveGetAddr()
{
    return I2C2ADB0;
}

static inline void I2C2_SlaveSendAck()
{
    I2C2CON1bits.ACKDT = 0;
}


static inline void I2C2_SlaveSendNack()
{
    I2C2CON1bits.ACKDT = 1;
}

static inline bool I2C2_SlaveIsNack()
{
    return I2C2ERRbits.NACKIF;
}

static inline void I2C2_SlaveClearNack()
{
    I2C2ERRbits.NACKIF = 0;
}

