/*
 * LCD Keypad Shield.c
 *
 * Created: 06-02-2020 09:44:23
 * Author : jonas
 */ 

#include <avr/io.h>
#include "lcd162.h"
#define F_CPU 16000000
#include <util/delay.h>

int main(void)
{
    /* Replace with your application code */
	LCDInit();
	
	
	while(1)
	{
		LCDDispChar(readKeys());
		_delay_ms(200);
		LCDClear();
	}
}

