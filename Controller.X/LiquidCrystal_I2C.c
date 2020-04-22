// Porting from Arduino LiquidCrystal_I2C
// Based on the work by DFRobot

#include "LiquidCrystal_I2C.h"
#include "time.h"
#include "i2c2_mux.h"

uint8_t _Addr;
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines;
uint8_t _cols;
uint8_t _rows;
uint8_t _backlightval;
uint8_t i2cBuff[80];


/************ low level data pushing commands **********/

void expanderWrite(uint8_t data) {
        uint8_t i2cBuff[2];
        i2cBuff[0]=data | _backlightval;
        I2C2_LOpen();
        I2C2_SetBuffer(i2cBuff,1);
        I2C2_MasterWrite();
        while(I2C2_BUSY == I2C2_LClose()); // sit here until finished.
}
#if 1
void write4bits(uint8_t value) {
        uint8_t i2cBuff[2];
        
        I2C2_LOpen();
        i2cBuff[0]=value  | _backlightval;
        I2C2_SetBuffer(i2cBuff,1);
        I2C2_MasterWrite();
        while(I2C2_BUSY == I2C2_LClose()); // sit here until finished.

        I2C2_LOpen();
        i2cBuff[0]=value | En | _backlightval;
        I2C2_SetBuffer(i2cBuff,1);
        I2C2_MasterWrite();
        while(I2C2_BUSY == I2C2_LClose()); // sit here until finished.
        
        I2C2_LOpen();
        i2cBuff[0]=value | ~En | _backlightval;
        I2C2_SetBuffer(i2cBuff,1);
        I2C2_MasterWrite();
        while(I2C2_BUSY == I2C2_LClose()); // sit here until finished.
}
#else
void write4bits(uint8_t value) {
        uint8_t i2cBuff[3];
        
        I2C2_LOpen(_Addr);
        i2cBuff[0]=value | _backlightval;
        i2cBuff[1]=value | En | _backlightval;
        i2cBuff[2]=value | ~En | _backlightval;
        I2C2_SetBuffer(i2cBuff,3);
        I2C2_MasterWrite();
        while(I2C2_BUSY == I2C2_LClose()); // sit here until finished.
}
#endif

// write either command or data
void send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    write4bits((highnib)|mode);
	write4bits((lownib)|mode); 
}

void write(uint8_t value) {
    uint8_t i2cBuff[4];
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    i2cBuff[0]=highnib | Rs | En | _backlightval;
    i2cBuff[1]=highnib | Rs | ~En | _backlightval;
    i2cBuff[2]=lownib | Rs | En | _backlightval;
    i2cBuff[3]=lownib | Rs | ~En | _backlightval;
    I2C2_LOpen();
    I2C2_SetBuffer(i2cBuff,4);
    I2C2_MasterWrite();
    while(I2C2_BUSY == I2C2_LClose()); // sit here until finished.
}

void printstr(const char c[]){
    uint8_t idx;
    

    idx=0;
    while (*c){
        i2cBuff[idx++]=((*c)&0xF0) | Rs | En | _backlightval;
        i2cBuff[idx++]=((*c)&0xF0) | Rs | ~En | _backlightval;
        i2cBuff[idx++]=(((*c)&0x0F)<<4) | Rs | En | _backlightval;
        i2cBuff[idx++]=(((*c)&0x0F)<<4) | Rs | ~En | _backlightval;
        c++;
        if (idx >=80) {
            break;
        }
    }
    I2C2_LOpen();
    I2C2_SetBuffer(i2cBuff,idx);
    I2C2_MasterWrite();
}

bool PrintStrBusy(void){
    return (I2C2_BUSY == I2C2_LClose());
}	

void printstrblock(const char c[]){
    printstr(c);
    while(PrintStrBusy());
}

/*********** mid level commands, for sending data/cmds */

inline void command(uint8_t value) {
	send(value, 0);
}

/********** high level commands, for the user! */
// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

void LcdI2CInit(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows){
  _Addr = lcd_Addr;
  _cols = lcd_cols;
  _rows = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
  _numlines = 2;

  _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	timeDelayMs(50); 
  
	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	timeDelayMs(10);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	  // we start in 8bit mode, try to set 4 bit mode
   write4bits(0x03 << 4);
   timeDelayMs(5); // wait min 4.1ms
   
   // second try
   write4bits(0x03 << 4);
   timeDelayMs(5); // wait min 4.1ms
   
   // third go!
   write4bits(0x03 << 4); 
   
   // finally, set to 4-bit interface
   write4bits(0x02 << 4); 

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	
	// clear it off
	clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
	
	home();
  
}

void clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	timeDelayMs(2);  // this command takes a long time!
}

void home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	timeDelayMs(2);  // this command takes a long time!
}

void setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}

void load_custom_character(uint8_t char_num, uint8_t *rows){
		createChar(char_num, rows);
}

void setBacklight(bool new_val){
	if(new_val){
		backlight();		// turn backlight on
	}else{
		noBacklight();		// turn backlight off
	}
}
