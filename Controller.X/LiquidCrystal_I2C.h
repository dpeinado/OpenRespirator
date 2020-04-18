// Porting from Arduino LiquidCrystal_I2C
//YWROBOT

#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h

#include "mcc_generated_files/mcc.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x4  // Enable bit
#define Rw 0x2  // Read/Write bit
#define Rs 0x1  // Register select bit

void LcdI2CInit(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows);
void clear();
void home();
void noDisplay();
void display();
void noBlink();
void blink();
void noCursor();
void cursor();
void scrollDisplayLeft();
void scrollDisplayRight();
void printLeft();
void printRight();
void leftToRight();
void rightToLeft();
void shiftIncrement();
void shiftDecrement();
void noBacklight();
void backlight();
void autoscroll();
void noAutoscroll(); 
void createChar(uint8_t, uint8_t[]);
void setCursor(uint8_t, uint8_t); 
void write(uint8_t);
void init();
void setBacklight(bool new_val);				// alias for backlight() and nobacklight()
void load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar()
void printstr(const char[]);
void printstrblock(const char[]);
bool PrintStrBusy(void);
#endif

