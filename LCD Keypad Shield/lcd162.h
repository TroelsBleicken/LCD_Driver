/*--------------------------------------------------------------
  File name: "lcd162.h"

  Driver for "LCD Keypad Shield" alphanumeric display.
  Display controller = HD44780U (LCD-II).
  
  Max. uC clock frequency = 16 MHz (Tclk = 62,5 ns)

  Connection : PORTx (4 bit mode) :
  [LCD]        [Portx]
  RS   ------  PH 5
  RW   ------  GND
  E    ------  PH 6
  DB4  ------  PG 5
  DB5  ------  PE 3
  DB6  ------  PH 3
  DB7  ------  PH 4

  Henning Hargaard, February 5, 2018
--------------------------------------------------------------*/
void LCDInit();
void LCDClear();
void LCDGotoXY(unsigned char x, unsigned char y);
void LCDDispChar(char ch);
void LCDDispString(char *str);
void LCDDispInteger(int i);
void LCDLoadUDC(unsigned char UDCNo, const unsigned char *UDCTab);
void LCDOnOffControl(unsigned char cursor, unsigned char blink);
void LCDCursorLeft();
void LCDCursorRight();
void LCDShiftLeft();
void LCDShiftRight();
void setBacklight(unsigned char percent);
unsigned char readKeys();
//-------------------------------------------------------------