
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "mcc_generated_files/mcc.h"
#include "lcd.h"

#define LCD_DATA		PORTB
#define LCD_E			PORTBbits.RB1
#define LCD_RS			PORTBbits.RB0
#define LCD_DLY         100                 // 10 us DELAY TBD

void LCDWriteCmd(uint8_t data);
void LCDWriteChar(char chr);
void LCDWrite4(uint8_t data);


void LCDInit (void) {
    DAC1_SetOutput(31);
    
    _delay(LCD_DLY*200);   // 15 ms after power up
    _delay(LCD_DLY*200);
    _delay(LCD_DLY*200);
    _delay(LCD_DLY*200);
    _delay(LCD_DLY*200);
    _delay(LCD_DLY*200);
    _delay(LCD_DLY*200);
    _delay(LCD_DLY*100);
    
    LCDWrite4(0x30);
    _delay(LCD_DLY*200);    // 5 ms
    _delay(LCD_DLY*200);
    _delay(LCD_DLY*100);
    LCDWrite4(0x30);
    _delay(LCD_DLY*10);     // 100 us
    LCDWrite4(0x30);
    
    LCDWrite4(0x20);        // Enter in 4 bit mode

    LCDWriteCmd(0x20);      // Select 5x8 single row
    LCDWriteCmd(0x08);      // Display Off
    LCDWriteCmd(0x01);      // Display Clear
    LCDWriteCmd(0x06);      // Increment and now shift
    LCDWriteCmd(0x12);      // Display on; cursor off, blink off
    LCDWriteCmd(0x02);      // Return home
    
    LCDMessage("Open Ventilator");

}

#define min(x,y) (x)>(y)?(y):(x)
void LCDMessage(char *str) {
    int length = min( strlen(str), 16);
    for (int i= 0; i<length; i++) {
        LCDWriteChar(str[i]);
    }
    LCDWriteCmd(0x02);      // Return home
    printf("\rLCD: %s", str);
}

void LCDWriteChar(char chr)
{
	LCD_DATA = (chr >> 2) & 0x3C;		// Place data high nibble
	LCD_RS = 1;							// Prepare LCD for receiving character
	LCD_E  = 1;							// Enable read
	_delay(LCD_DLY/2);					// Wait 5uS
	LCD_E  = 0;							// Latch data
	Nop();								// Allow port to settle
	LCD_DATA = (chr << 2) & 0x3C;		// Place data low nibble
	LCD_RS = 1;
	LCD_E  = 1;							// Enable read
	_delay(LCD_DLY/2);					// Wait 5uS
	LCD_E  = 0;							// Latch data
	_delay(LCD_DLY*4);					// Wait 38uS
}

void LCDWrite4(uint8_t chr)
{
	LCD_DATA = (chr >> 2) & 0x3C;		// Place data high nibble
	LCD_RS = 0;							// Prepare LCD for receiving character
	LCD_E  = 1;							// Enable read
	_delay(LCD_DLY/2);					// Wait 5uS
	LCD_E  = 0;							// Latch data
	_delay(LCD_DLY*4);					// Wait 38uS
}

void LCDWriteCmd(uint8_t data) {
    
	LCD_DATA = (data >> 2) & 0x3C; // Place data
	LCD_RS = 0;							// Prepare LCD for receiving instruction
	LCD_E  = 1;							// Enable read
	_delay(LCD_DLY);					// Wait 10uS
	LCD_E  = 0;							// Latch data
	Nop();								// Allow port to settle
	LCD_DATA = (data << 2) & 0x3C; // Place data low nibble
	LCD_RS = 0;							// Prepare LCD for receiving instruction
	LCD_E  = 1;							// Enable read
	_delay(LCD_DLY);					// Wait 10uS
	LCD_E  = 0;							// Latch data
            
}