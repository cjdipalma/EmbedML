/*
 * avrmem.h
 *
 * Created: 3/5/2020 7:07:44 AM
 *  Author: Chris
 
todo: simple checking structure to see if malloc statements can be fit

simple memory algorithm basically
I don't even really need an algorithm for the time being

 */ 

#ifndef _AVRMEM_H
#define _AVRMEM_H

#include "helper.h"
#define RAM_PORT PORTC
#define RAM_PIN PINC
#define RAM_DDR DDRC
#define RCS 0
#define RSI 1
#define RSCK 2
#define RSO 3

#define RAM_MEM_SIZE 721696 // Useful for checking against malloc operations

void ram_init();

long avr_alloc(long sz);

void avr_free(long addr);

void ram_write(long addr, unsigned char data, long nBytes); // equivalent to memset

void ram_write_float(long addr, float data);

unsigned char ram_read(long addr);

float ram_read_float(long addr);

void ram_inc_float(long addr, float data); // += operator

void ram_cpy(long addr, const void* src, long nBytes); // memcpy

void ram_close();

void select_ram();

void deselect_ram();

void set_sck();

void clr_sck();

char get_SO();

#endif /* AVRMEM_H_ */