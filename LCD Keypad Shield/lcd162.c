/*-------------------------------------------------------------------
  File name: "lcd162.c"

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
---------------------------------------------------------------------*/
#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
// Enabling us to use macro _NOP() to insert the NOP instruction
#include <avr/cpufunc.h>
#include "lcd162.h"

// library function itoa() is needed
#include <stdlib.h>

//*********************** PRIVATE (static) operations *********************
static void waitBusy()
{
  // To be implemented
}

static void wait_ns(uint16_t time)
{
	unsigned char nops = time/63;
	if(time%63)
	{
		nops++;
	}
	for(char i = 0;i<nops;i++)
	{
		_NOP();
	}
}  

static void pulse_E()
{
  // To be implemented
  PORTH |= 0b01000000;
  wait_ns(450);
  PORTH &= ~(0b01000000);
  wait_ns(50);
}

// Sets the display data pins according to the 4 lower bits of data
static void set4DataPins(unsigned char data)
{
  PORTH = (PORTH & 0b11100111) | ((data<<1) & 0b00011000);
  PORTE = (PORTE & 0b11110111) | ((data<<2) & 0b00001000);
  PORTG = (PORTG & 0b11011111) | ((data<<5) & 0b00100000);  
}

static void setPins(unsigned char data8)
{
	set4DataPins(data8>>4);
	pulse_E();
	set4DataPins(data8);
	pulse_E();
}

static void sendInstruction(unsigned char data)
{      
  // To be implemented
  PORTH &= ~(0b00100000);
  wait_ns(40);
  setPins(data);
}

static void sendData(unsigned char data)
{      
  // To be implemented
  PORTH |= 0b00100000;
  wait_ns(40);
  setPins(data);
  _delay_us(37);
}


//*********************** PUBLIC functions *****************************

// Initializes the display, blanks it and sets "current display position"
// at the upper line, leftmost character (cursor invisible)
// Reference: Page 46 in the HD44780 data sheet
void LCDInit()
{
  // Initializing the used port
  DDRH |= 0b01111000;  // Outputs
  DDRE |= 0b00001000;
  DDRG |= 0b00100000;
  
  // Wait 50 ms (min. 15 ms demanded according to the data sheet)
  _delay_ms(50);
  // Function set (still 8 bit interface)
  PORTG |= 0b00100000;
  PORTE |= 0b00001000;
  pulse_E();
  
  // Wait 10 ms (min. 4,1 ms demanded according to the data sheet)
  _delay_ms(10);
  // Function set (still 8 bit interface)
  pulse_E();

  // Wait 10 ms (min. 100 us demanded according to the data sheet)
  _delay_ms(10);
  // Function set (still 8 bit interface)
  pulse_E();

  // Wait 10 ms (min. 100 us demanded according to the data sheet)
  _delay_ms(10);
  // Function set (now selecting 4 bit interface !)
  PORTG &= 0b11011111;
  pulse_E();

  // Function Set : 4 bit interface, 2 line display, 5x8 dots
  sendInstruction( 0b00101000 );
  _delay_us(37);
  // Display, cursor and blinking OFF
  sendInstruction( 0b00001000 );
  _delay_us(37);
  // Clear display and set DDRAM adr = 0	
  sendInstruction( 0b00000001 );
  // By display writes : Increment cursor / no shift
  _delay_us(1520);
  sendInstruction( 0b00000110 );
  _delay_us(37);
  // Display ON, cursor and blinking OFF
  sendInstruction( 0b00001100 );
  _delay_us(37);
}

// Blanks the display and sets "current display position" to
// the upper line, leftmost character
void LCDClear()
{
  sendInstruction( 0b00000001 );
  _delay_us(1520);
}

// Sets DDRAM address to character position x and line number y
void LCDGotoXY( unsigned char x, unsigned char y )
{
  // To be implemented
  if(y>1)
	return;
  unsigned char adr = (y * 0x40) + (x % 16);
  
  unsigned char tmp = 0b10000000 | adr;
  sendInstruction(tmp);
  _delay_us(37);
}

// Display "ch" at "current display position"
void LCDDispChar(char ch)
{
  // To be implemented
  sendData(ch);
  
}

// Displays the string "str" starting at "current display position"
void LCDDispString(char* str)
{
  LCDClear();
  unsigned char i = 0;
  unsigned char flag = 1;
  while(str[i] != '\0')
  {
	LCDDispChar(str[i]);
	i++;
	if (i>15 && flag==1)
	{
		LCDGotoXY(0,1);
		flag = 0;
	}
   }
}

// Displays the value of integer "i" at "current display position"
void LCDDispInteger(int i)
{
  // To be implemented
  unsigned char str[32];
  itoa(i,str,10);
  LCDDispString(str);
}

// Loads one of the 8 user definable characters (UDC) with a dot-pattern,
// pre-defined in an 8 byte array in FLASH memory
void LCDLoadUDC(unsigned char UDCNo, const unsigned char *UDCTab)
{
  unsigned char address = 0x40 + (8*UDCNo);//First address is at 0x40 with 8 bits, next is at 0x48 ... last is 0x78
  sendInstruction(address);
  
  for(unsigned char i = 0; i < 8; ++i)
  {
	  sendData(UDCTab[i]);
  }
  
}

// Selects, if the cursor has to be visible, and if the character at
// the cursor position has to blink.
// "cursor" not 0 => visible cursor.
// "blink" not 0 => the character at the cursor position blinks.
void LCDOnOffControl(unsigned char cursor, unsigned char blink)
{
  unsigned char command = 0b00001100;
  command |= (0b00000010 & (cursor << 1));
  command |= (0b00000001 & blink);
  sendInstruction(command);
}

// Moves the cursor to the left
void LCDCursorLeft()
{
  sendInstruction(0b00010000);
}

// Moves the cursor to the right
void LCDCursorRight()
{
  sendInstruction(0b00010100);
}

// Moves the display text one position to the left
void LCDShiftLeft()
{
  sendInstruction(0b00011000);
}

// Moves the display text one position to the right
void LCDShiftRight()
{
  sendInstruction(0b00011100);
}

// Sets the backlight intensity to "percent" (0-100)
void setBacklight(unsigned char percent)
{
  DDRB |= 0b00010000;//Set to output pin
  TCCR2A &= 0b11111000;
  TCCR2A |= 0b10000001;//Control backlight using PWM, phase corret PWM
  TCCR2B |= 0b00000100;//64 prescaler
  OCR2A = 0xFF * (percent/100.0);
  
}

// Reads the status for the 5 on board keys
// Returns 0, if no key pressed
//enum KEY = {SELECT, LEFT, UP, DOWN, RIGHT};
unsigned char readKeys()
{
  DDRF &= 0b11111110;//PINF,0 = ADC0
  
   ADMUX |= (1<<REFS0);//VCC is ref
   ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);//Enable ADC, set prescaler to 128
   
   ADMUX = (ADMUX & 0xF0) | (0 & 0x0F);//Select channel 0 - buttons
   //single conversion mode
   ADCSRA |= (1<<ADSC);
   // wait until ADC conversion is complete
   while( ADCSRA & (1<<ADSC) );
   
   if(ADC > 1023)
		return 'N';//No keys
   else if(ADC < 1)
		return 'R';//Right key
   else if(ADC == 99 || ACD == 98)
		return 'U';//Up key
   else if(ADC == 256)
		return 'D';//Down key
   else if(ADC == 409)
		return 'L';//Left key
   else if(ADC == 640)
		return 'S';//Select key
   else
		return 'N';
	
}