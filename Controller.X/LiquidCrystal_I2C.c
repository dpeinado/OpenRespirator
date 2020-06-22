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
volatile uint8_t _backlightval;
volatile uint8_t i2cBuff[80];


/************ low level data pushing commands **********/
#define LCD_D0 LATAbits.LATA0
#define LCD_D1 LATAbits.LATA1
#define LCD_D2 LATAbits.LATA4
#define LCD_D3 LATAbits.LATA5
#define LCD_E LATAbits.LATA7
#define LCD_RS LATAbits.LATA6
#define LCD_BL LATBbits.LATB5

volatile uint8_t parBuffPtr;
volatile uint8_t parBuffLen;
volatile bool lcdBusy;

void lcdIrqHandler(void){
    uint8_t byte;
    if (parBuffPtr < parBuffLen){
        // Set E=0
        LCD_E=0;
        // Set RS.
        byte = i2cBuff[parBuffPtr];
        LCD_RS=(byte&Rs)?1:0;
        // Raise E.
        LCD_E=(byte&En)?1:0;
        // Now put rest of the data.
        LCD_D0=(byte&0x10)?1:0;
        LCD_D1=(byte&0x20)?1:0;
        LCD_D2=(byte&0x40)?1:0;
        LCD_D3=(byte&0x80)?1:0;
        // Now check if backlight has to be set.
        LCD_BL = (byte&LCD_BACKLIGHT)?1:0;
        parBuffPtr++;
        // Re-enable timer.
        TMR6_Start();
        // Now clear E.
        LCD_E=0;
    } else {
        lcdBusy = false;
    }
}

void send4bits(uint8_t value, uint8_t mode) {
    int idx;
    
    if (lcdBusy){
        PIE9bits.TMR6IE = 0;
        idx = parBuffLen;
    } else {
        idx = 0;
        parBuffPtr=0;
    }
    i2cBuff[idx++]=value | mode | _backlightval;

    parBuffLen=idx;
    if (lcdBusy){
        PIE9bits.TMR6IE = 1;
    } else {
        lcdBusy = true;
        TMR6_Start();
    }
}

// write either command or data
void send(uint8_t value, uint8_t mode) {
    send4bits(value&0xf0, mode);
    send4bits((value<<4)&0xf0, mode);
}

// Only blocking function.
void expanderWriteBlock(uint8_t data) {
    send4bits(data, 0);
}

void write(uint8_t value) {
    send(value,Rs|En);
}

void printstr(const char c[]){
    uint8_t idx;
    
    if (lcdBusy){
        PIE9bits.TMR6IE = 0;
        idx = parBuffLen;
    } else {
        idx = 0;
        parBuffPtr=0;
    }
    while (*c){
        i2cBuff[idx++]=((*c)&0xF0) | Rs | En | _backlightval;
        i2cBuff[idx++]=(((*c)&0x0F)<<4) | Rs | En | _backlightval;
        c++;
        if (idx >=80) {
            break;
        }
    }

    parBuffLen=idx;
    if (lcdBusy){
        PIE9bits.TMR6IE = 1;
    } else {
        lcdBusy = true;
        TMR6_Start();
    }
}

bool PrintStrBusy(void){
    return lcdBusy;
}	

void printstrblock(const char c[]){
    while(PrintStrBusy());
    printstr(c);
    while(PrintStrBusy());
}

/*********** mid level commands, for sending data/cmds */

inline void command(uint8_t value) {
	send(value, En);
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

void LcdI2CInit(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows) {
    _Addr = lcd_Addr;
    _cols = lcd_cols;
    _rows = lcd_rows;
    _backlightval = LCD_BACKLIGHT;
    _numlines = 2;

    _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;

    parBuffPtr = 0;
    parBuffLen = 0;
    lcdBusy = false;
    TMR6_SetInterruptHandler(lcdIrqHandler);

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    timeDelayMs(50);

    // Now we pull both RS and R/W low to begin commands
    expanderWriteBlock(_backlightval);
    timeDelayMs(20);

    //put the LCD into 4 bit mode. First set it in 8-bit mode regardless the current mode.
    send4bits(0x03 << 4, En);
    while (lcdBusy);
    timeDelayMs(20);
    send4bits(0x03 << 4, En);
    while (lcdBusy);
    timeDelayMs(20);
    send4bits(0x03 << 4, En);
    while (lcdBusy);
    timeDelayMs(20);

    // finally, set to 4-bit interface
    send4bits(0x02 << 4, En);
    while (lcdBusy);
    timeDelayMs(20); // wait min 4.1ms

    // set # lines, font size, etc.
    command(LCD_FUNCTIONSET | _displayfunction);
    while (lcdBusy);
    timeDelayMs(20); // wait min 4.1ms

    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    while (lcdBusy);

    // clear it off
    clear();
    while (lcdBusy);

    // Initialize to default text direction (for roman languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    // set the entry mode
    command(LCD_ENTRYMODESET | _displaymode);
    while (lcdBusy);

    home();
    while (lcdBusy);
}

void clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	timeDelayMs(10);  // this command takes a long time!
}

void home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	timeDelayMs(10);  // this command takes a long time!
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
	expanderWriteBlock(0);
}

void backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWriteBlock(0);
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
