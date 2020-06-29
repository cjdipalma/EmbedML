#include <avr/io.h>
#include "helper.h"

void avr_wait(unsigned short cmsec)
{
	TCCR0B = 2;
	while (cmsec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * 0.00001);
		SET_BIT(TIFR0, TOV0);
		// WDR();
		while (!GET_BIT(TIFR0, TOV0));
	}
	TCCR0B = 0;
}

/*
void print_bit(char i)
{
	if (i != 0)		SET_BIT(PORTA, 1);
	else			CLR_BIT(PORTA, 1);
}
*/

void print_bit(char i, char target)
{
	if (i != 0)		SET_BIT(PORTA, target);
	else			CLR_BIT(PORTA, target);
}

void print_byte(unsigned char data)
{
	for (int i=7; i>=0; --i)
	{
		print_bit(1, 0); // Clock bit on
		print_bit(GET_BIT(data, i), 1); // Data bit
		avr_wait(AVR_WAIT_TIME);
		print_bit(0, 0);
		print_bit(0, 1);
		avr_wait(AVR_WAIT_TIME); // Clear clk, and wait
	}
}