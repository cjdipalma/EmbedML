#include <avr/io.h>

#ifndef _HELPER_H
#define _HELPER_H

#define XTAL_FRQ 8000000lu


#define SET_BIT(p,i) ((p) |=  (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) &   (1 << (i)))
#define WRT_BYTE(p,b,i) ((p) |=  (b << (i * 8)))
#define GET_BYTE(p,i) ((p) &   (11111111UL << (i * 8)))

#define AVR_WAIT_TIME 3000

void avr_wait(unsigned short cmsec8);

void set_si_port(char value);

// void print_bit(char i);

void print_bit(char i, char target);

void print_byte(unsigned char data);

#endif /* _HELPER_H */