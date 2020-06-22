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
#include <string.h>

#include "mcc_generated_files/mcc.h"
#include "lcd.h"


void LCDWriteCmd(uint8_t data);
void LCDWriteChar(char chr);
void LCDWrite4(uint8_t data);

static bool LED = false;

void LCDTest(void);


void LCDInit (void) {
    
    __delay_ms(50);         // 50 ms after power up
 
    LCDWrite4(0x30);        // Move to 4 bits try 1
    __delay_ms(5);          // 5 ms
    LCDWrite4(0x30);        // Move to 4 bits try 2
    __delay_ms(5);          // 5 ms
    LCDWrite4(0x30);        // Move to 4 bits try 3
    __delay_ms(1);          // 1 ms
    LCDWrite4(0x20);        // Enter in 4 bit mode

    LCDWriteCmd(0x28);      // Select 5x8 double row
    LCDWriteCmd(0x08);      // Display Off
    LCDWriteCmd(0x01);      // Display Clear
    __delay_ms(2);
    LCDWriteCmd(0x06);      // Entry Mode Set
    
    LCDWriteCmd(0x0C);      // Display on; cursor off, blink off
    LCDWriteCmd(0x02);      // Return home
              //1234567890123456
    LCDMessage("Emerg Respirator");
    LCDWriteCmd(0xC0);      // Second Line
    LCDMessage("    OxyVita     ");
    LCDWriteCmd(0x02);
    __delay_ms(750);
    //printf("LCDTest\r\n");
    LCDTest();
    /*
    LCDWriteCmd(0x02);
    __delay_ms(1000);
    LCDWriteCmd(0x01);
    __delay_ms(2);
    LCDMessage("Open Ventilator");
    LCDWriteCmd(0xC0);      // Second Line
    LCDMessage("                ");
    LCDWriteCmd(0x02);
    */
}
void LCDTest(void) {
    char msg1[17];
    char msg2[17];
    
    for (int i=0; i<9; i++) {
        for (int j=0; j<17; j++) {
            msg1[j] = '0' + i; msg2[j] = '0' + i + 1; 
        }
        LCDMessage12(msg1,msg2);
        __delay_ms(500);
    }
    /*   
    LCDMessage12("0000000000000000","1111111111111111");
    __delay_ms(750);
    LCDMessage12("1111111111111111","2222222222222222");
    __delay_ms(750);
    LCDMessage12("2222222222222222","3333333333333333");
    __delay_ms(750);
    LCDMessage12("3333333333333333","4444444444444444");
    __delay_ms(750);
    LCDMessage12("4444444444444444","5555555555555555");
    __delay_ms(750);
    LCDMessage12("5555555555555555","6666666666666666");
    __delay_ms(750);
    LCDMessage12("6666666666666666","7777777777777777");
    __delay_ms(750);
    LCDMessage12("7777777777777777","8888888888888888");
    __delay_ms(750);
    LCDMessage12("8888888888888888","9999999999999999");
    __delay_ms(750);
    LCDMessage12("9999999999999999","0000000000000000");
    __delay_ms(750);
    */
}

void LCDOff(void) {
    LED = false;
    LLED_SetLow();
    //LCDWriteCmd(0x01);
    //__delay_ms(2);
}

void LCDOn(void) {
    LED = true;
    LLED_SetHigh();
}

#define min(x,y) (x)>(y)?(y):(x)
void LCDMessage(char *str) {
    
    int length = min( strlen(str), 16);
    
    for (int i= 0; i<length; i++) {
        LCDWriteChar(str[i]);
    }
    //LCDWriteCmd(0x02);      // Return home
    
    //printf("\rLCD(%02d): %s", length, str);
}
void LCDMessage1(char *str) {
    LCDWriteCmd(0x02);
    if (LED) LCDMessage(str);
}

void LCDMessage12(char *str1, char *str2) {
    LCDWriteCmd(0x02);
    if (LED) LCDMessage(str1);
    LCDWriteCmd(0xC0);      // Second Line
    if (LED) LCDMessage(str2);
}

void LCDMessage2(char *str) {
    LCDWriteCmd(0x02);
    LCDWriteCmd(0xC0);      // Second Line
    if (LED) LCDMessage(str);
}

void LCDWriteChar(char chr)
{
    /*
    I2CSend(1,0,0,chr); //RS RW E D74
    __delay_us(5);
    I2CSend(1,0,1,chr); //RS RW E D74
    __delay_us(10);
    I2CSend(1,0,0,chr); //RS RW E D74
    __delay_us(5);
    I2CSend(1,0,0,chr<<4); //RS RW E D74
    __delay_us(5);
    I2CSend(1,0,1,chr<<4); //RS RW E D74
    __delay_us(10);
    I2CSend(1,0,0,chr<<4); //RS RW E D74
    __delay_us(38);
    */
    
    if (chr&0x80) D7_SetHigh(); else D7_SetLow(); // Place data
    if (chr&0x40) D6_SetHigh(); else D6_SetLow();
    if (chr&0x20) D5_SetHigh(); else D5_SetLow();
    if (chr&0x10) D4_SetHigh(); else D4_SetLow(); 
    
	RS_SetHigh();							// Prepare LCD for receiving instruction
    __delay_us(5);
	E_SetHigh();							// Enable read
	__delay_us(10);                 	// Wait 10uS
	E_SetLow();							// Latch data
	__delay_us(5);								// Allow port to settle
    if (chr&0x8) D7_SetHigh(); else D7_SetLow(); // Place data low nibble
    if (chr&0x4) D6_SetHigh(); else D6_SetLow();
    if (chr&0x2) D5_SetHigh(); else D5_SetLow();
    if (chr&0x1) D4_SetHigh(); else D4_SetLow(); 
	
	RS_SetHigh();							// Prepare LCD for receiving instruction
    __delay_us(5);
	E_SetHigh();							// Enable read
	__delay_us(10);     				// Wait 10uS
	E_SetLow();							// Latch data
    __delay_us(38);

}

void LCDWrite4(uint8_t chr)
{
    /*
    I2CSend(0,0,0,chr); //RS RW E D74
    __delay_us(5);
    I2CSend(0,0,1,chr); //RS RW E D74
    __delay_us(10);
    I2CSend(0,0,0,chr); //RS RW E D74
    __delay_us(38);
    */
    
     
    if (chr&0x80) D7_SetHigh(); else D7_SetLow(); // Place data high nibble
    if (chr&0x40) D6_SetHigh(); else D6_SetLow();
    if (chr&0x20) D5_SetHigh(); else D5_SetLow();
    if (chr&0x10) D4_SetHigh(); else D4_SetLow();
    
	RS_SetLow();							// Prepare LCD for receiving character
    __delay_us(5);
	E_SetHigh();							// Enable read
	__delay_us(10);  					// Wait 5uS
	E_SetLow();							// Latch data
	__delay_us(38);         			// Wait 38uS
    
}

void LCDWriteCmd(uint8_t data) 
{
     /*
    I2CSend(0,0,0,data); //RS RW E D74
    __delay_us(5);
    I2CSend(0,0,1,data); //RS RW E D74
    __delay_us(10);
    I2CSend(0,0,0,data); //RS RW E D74
    __delay_us(5);
    I2CSend(0,0,0,data<<4); //RS RW E D74
    __delay_us(5);
    I2CSend(0,0,1,data<<4); //RS RW E D74
    __delay_us(10);
    I2CSend(0,0,0,data<<4); //RS RW E D74
    __delay_us(38);
    */
    
	if (data&0x80) D7_SetHigh(); else D7_SetLow(); // Place data
    if (data&0x40) D6_SetHigh(); else D6_SetLow();
    if (data&0x20) D5_SetHigh(); else D5_SetLow();
    if (data&0x10) D4_SetHigh(); else D4_SetLow(); 
    
	RS_SetLow();							// Prepare LCD for receiving instruction
    __delay_us(5);
	E_SetHigh();							// Enable read
	__delay_us(10);                 	// Wait 10uS
	E_SetLow();							// Latch data
	__delay_us(5);								// Allow port to settle
    if (data&0x8) D7_SetHigh(); else D7_SetLow(); // Place data low nibble
    if (data&0x4) D6_SetHigh(); else D6_SetLow();
    if (data&0x2) D5_SetHigh(); else D5_SetLow();
    if (data&0x1) D4_SetHigh(); else D4_SetLow(); 
	
	RS_SetLow();							// Prepare LCD for receiving instruction
    __delay_us(5);
	E_SetHigh();							// Enable read
	__delay_us(10);     				// Wait 10uS
	E_SetLow();							// Latch data
    __delay_us(38);
            
}

/*
void I2CSend(bool RS, bool RW, bool E, uint8_t D74) {
        
    static uint8_t writeBuffer[1];
    i2c1_error_t err;
    
    writeBuffer[0] = (D74&0xF0) | (E<<2) | (RW<<1) | RS | LED<<3; // seconds address pointer

    //printf("\r\nI2C Send 0x%0X 0x%0X\r\n", 0x27<<1, writeBuffer[0]);
    
    
    while(!I2C1_Open(0x27));
    //printf("I2C Open\r\n");
    //I2C1_SetDataCompleteCallback(wr0RegCompleteHandler,NULL);
    I2C1_SetDataCompleteCallback(NULL,NULL);
    I2C1_SetBuffer(writeBuffer, 1);
    I2C1_SetAddressNackCallback(NULL,NULL);
    //printf("I2C PreWrite %02X %02X %02X\r\n", I2C1STAT0, I2C1STAT1,I2C1ERR);

    I2C1_MasterWrite();
    //I2C2_MasterOperation(false); // Write
    //printf("I2C Write %02X %02X %02X\r\n", I2C1STAT0, I2C1STAT1,I2C1ERR);
    while(I2C1_BUSY == (err=I2C1_Close()));
 
    
    if (err==I2C1_FAIL) {
        printf("\r\nI2C FAIL 0x%0X 0x%0X\r\n", 0x27<<1, writeBuffer[0]);
        return;
    }
    //printf("0x%0X ", writeBuffer[0]);

}
 */