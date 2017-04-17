#include "LiquidCrystal.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <Arduino.h>

struct lcd_ops lcd_default_ops = {
	.begin = begin,
	.clear = clear,
	.setCursor = setCursor,
	.print = print,
};

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

void set_ops(struct LiquidCrystal *lcd, struct lcd_ops *ops)
{
	lcd->ops = *ops;
}

void lcd_init(struct LiquidCrystal *lcd, uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
			 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{

  set_ops(lcd, &lcd_default_ops);
  lcd->_rs_pin = rs;
  lcd->_rw_pin = rw;
  lcd->_enable_pin = enable;

  lcd->_data_pins[0] = d0;
  lcd->_data_pins[1] = d1;
  lcd->_data_pins[2] = d2;
  lcd->_data_pins[3] = d3;
  lcd->_data_pins[4] = d4;
  lcd->_data_pins[5] = d5;
  lcd->_data_pins[6] = d6;
  lcd->_data_pins[7] = d7;

  pinMode(lcd->_rs_pin, OUTPUT);
  // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
  if (lcd->_rw_pin != 255) {
    pinMode(lcd->_rw_pin, OUTPUT);
  }
  pinMode(lcd->_enable_pin, OUTPUT);

  if (fourbitmode)
    lcd->_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else
    lcd->_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

  begin(lcd, 16, 1, LCD_5x8DOTS);
}

void begin(struct LiquidCrystal *lcd, uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    lcd->_displayfunction |= LCD_2LINE;
  }
  lcd->_numlines = lines;
  lcd->_currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    lcd->_displayfunction |= LCD_5x10DOTS;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  delayMicroseconds(50000);
  // Now we pull both RS and R/W low to begin commands
  digitalWrite(lcd->_rs_pin, LOW);
  digitalWrite(lcd->_enable_pin, LOW);
  if (lcd->_rw_pin != 255) {
    digitalWrite(lcd->_rw_pin, LOW);
  }

  //put the LCD into 4 bit or 8 bit mode
  if (! (lcd->_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(lcd, 0x03);
    delayMicroseconds(4500); // wait min 4.1ms

    // second try
    write4bits(lcd, 0x03);
    delayMicroseconds(4500); // wait min 4.1ms

    // third go!
    write4bits(lcd, 0x03);
    delayMicroseconds(150);

    // finally, set to 4-bit interface
    write4bits(lcd, 0x02);
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);
    delayMicroseconds(4500);  // wait more than 4.1ms

    // second try
    command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);
    delayMicroseconds(150);

    // third go
    command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);
  }

  // finally, set # lines, font size, etc.
  command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);

  // turn the display on with no cursor or blinking default
  lcd->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display(lcd);

  // clear it off
  clear(lcd);

  // Initialize to default text direction (for romance languages)
  lcd->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);

}

/********** high level commands, for the user! */
void clear(struct LiquidCrystal *lcd)
{
  command(lcd, LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void home(struct LiquidCrystal *lcd)
{
  command(lcd, LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void setCursor(struct LiquidCrystal *lcd, uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row >= lcd->_numlines ) {
    row = lcd->_numlines-1;    // we count rows starting w/0
  }

  command(lcd, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay(struct LiquidCrystal *lcd) {
  lcd->_displaycontrol &= ~LCD_DISPLAYON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}
void display(struct LiquidCrystal *lcd) {
  lcd->_displaycontrol |= LCD_DISPLAYON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

// Turns the underline cursor on/off
void noCursor(struct LiquidCrystal *lcd) {
  lcd->_displaycontrol &= ~LCD_CURSORON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}
void cursor(struct LiquidCrystal *lcd) {
  lcd->_displaycontrol |= LCD_CURSORON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink(struct LiquidCrystal *lcd) {
  lcd->_displaycontrol &= ~LCD_BLINKON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}
void blink(struct LiquidCrystal *lcd) {
  lcd->_displaycontrol |= LCD_BLINKON;
  command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(struct LiquidCrystal *lcd) {
  command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(struct LiquidCrystal *lcd) {
  command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(struct LiquidCrystal *lcd) {
  lcd->_displaymode |= LCD_ENTRYLEFT;
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// This is for text that flows Right to Left
void rightToLeft(struct LiquidCrystal *lcd) {
  lcd->_displaymode &= ~LCD_ENTRYLEFT;
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll(struct LiquidCrystal *lcd) {
  lcd->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll(struct LiquidCrystal *lcd) {
  lcd->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void createChar(struct LiquidCrystal *lcd, uint8_t location, uint8_t charmap[]) {
  int i = 0;
  location &= 0x7; // we only have 8 locations 0-7
  command(lcd, LCD_SETCGRAMADDR | (location << 3));
  for (i=0; i<8; i++) {
    write(lcd, charmap[i]);
  }
}


/************ low level data pushing commands **********/
void pulseEnable(struct LiquidCrystal *lcd) {
	digitalWrite(lcd->_enable_pin, LOW);
	delayMicroseconds(1);
	digitalWrite(lcd->_enable_pin, HIGH);
	delayMicroseconds(1);    // enable pulse must be >450ns
	digitalWrite(lcd->_enable_pin, LOW);
	delayMicroseconds(100);   // commands need > 37us to settle
}

void write4bits(struct LiquidCrystal *lcd, uint8_t value) {
	int i = 0;

	for (i = 0; i < 4; i++) {
		pinMode(lcd->_data_pins[i], OUTPUT);
		digitalWrite(lcd->_data_pins[i], (value >> i) & 0x01);
	}
	pulseEnable(lcd);
}

void write8bits(struct LiquidCrystal *lcd, uint8_t value) {
	int i = 0;

	for (i = 0; i < 8; i++) {
		pinMode(lcd->_data_pins[i], OUTPUT);
		digitalWrite(lcd->_data_pins[i], (value >> i) & 0x01);
	}
	pulseEnable(lcd);
}

/*********** mid level commands, for sending data/cmds */
void command(struct LiquidCrystal *lcd, uint8_t value) {
	send(lcd, value, LOW);
}

size_t write(struct LiquidCrystal *lcd, uint8_t value) {
	send(lcd, value, HIGH);
	return 1; // assume sucess
}

// write either command or data, with automatic 4/8-bit selection
void send(struct LiquidCrystal *lcd, uint8_t value, uint8_t mode)
{
  digitalWrite(lcd->_rs_pin, mode);

  // if there is a RW pin indicated, set it low to Write
  if (lcd->_rw_pin != 255) {
    digitalWrite(lcd->_rw_pin, LOW);
  }

  if (lcd->_displayfunction & LCD_8BITMODE) {
    write8bits(lcd, value);
  } else {
    write4bits(lcd, value>>4);
    write4bits(lcd, value);
  }
}

#ifdef __cplusplus
void print(struct LiquidCrystal *lcd, String str)
{
	int i=0;

	while (str && str[i]!='\0') {
		write(lcd, str[i]);
		i++;
	}
}
#else
void print(struct LiquidCrystal *lcd, char *str)
{
	int i=0;

	while (str && str[i]!='\0') {
		write(lcd, str[i]);
		i++;
	}
}
#endif
