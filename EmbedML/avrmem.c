/*
 * avrmem.h
 *
 * Created: 3/5/2020 7:07:44 AM
 *  Author: Chris
 
todo: simple checking structure to see if malloc statements can be fit

simple memory algorithm basically
I don't even really need an algorithm for the time being

 */ 

#include "helper.h"
#include "avrmem.h"

#ifndef AVRMEM_H_
#define AVRMEM_H_

//DISCLAIMER: I have no idea what I was going to make this commented function do.
// Set all inputs to default values
// void reset_ram()
// {
//
// }

void set_si_port(char value)
{
	if(value != 0)	SET_BIT(RAM_PORT, RSI);
	else			CLR_BIT(RAM_PORT, RSI);
}

void select_ram()
{
	CLR_BIT(RAM_PORT, RCS);
}

void deselect_ram()
{
	SET_BIT(RAM_PORT, RCS);
}

void set_sck()
{
	SET_BIT(RAM_PORT, RSCK);
}

void clr_sck()
{
	CLR_BIT(RAM_PORT, RSCK);
}

char get_SO()
{
	return GET_BIT(RAM_PIN, RSO);
}

void ram_init()
{
	SET_BIT(RAM_DDR, RCS);
	SET_BIT(RAM_DDR, RSI);
	SET_BIT(RAM_DDR, RSCK);
	CLR_BIT(RAM_DDR, RSO);
	
	deselect_ram();
}

// Replacement for malloc.
long avr_alloc(long sz)
{
	return 0; // Only one thing allocated; the entire mem_arr
}

void avr_free(long addr)
{
	
}

// 24-bit address, 8-bit data
void ram_write(long addr, unsigned char data, long nBytes) // Equivalent to memset
{
	select_ram();
	avr_wait(1);
	
	// Write instruction
	for(int i=7; i>=0; --i)
	{
		if (i == 1)	set_si_port(1);
		else		set_si_port(0);
		set_sck();
		clr_sck();
	}
	
	// Address
	for(int i=23; i>=0; --i)
	{
		set_si_port(GET_BIT(addr, i));
		set_sck();
		clr_sck();
	}
	
	// Data to write
	for(int n=0; n<nBytes; ++n)
	{
		for(int i=7; i>=0; --i)
		{
			set_si_port(GET_BIT(data, i));
			set_sck();
			clr_sck();
		}
	}
	deselect_ram();
}

void ram_write_float(long addr, float data)
{
	unsigned char* p = (unsigned char*) &data;
	
	for(int i=0; i<4; ++i)
	{
		ram_write(addr+i, p[i], 1);
	}
}


// 24-bit address, 8-bit data
unsigned char ram_read(long addr)
{
	select_ram();
	
	// Read instruction
	for(int i=7; i>=0; --i)
	{
		if (i <= 1)	set_si_port(1);
		else		set_si_port(0);
		set_sck();
		clr_sck();
	}
	
	// Address
	for(int i=23; i>=0; --i)
	{
		set_si_port(GET_BIT(addr, i));
		set_sck();
		clr_sck();
	}
	
	unsigned char output = 0;
	
	// Receive data
	for(int i=7; i>=0; --i)
	{
		if(get_SO() != 0)
			SET_BIT(output, i);
		set_sck();
		clr_sck();
	}
	
	deselect_ram();
	return output;
}

float ram_read_float(long addr)
{
	float a = 0;
	unsigned char* ptr = (unsigned char*) &a;
	
	for(int i=0; i<4; ++i)
	ptr[i] = ram_read(addr+i);
	return a;
}

void ram_inc_float(long addr, float data)
{
	float prev = ram_read_float(addr);
	ram_write_float(addr, prev+data);
}

void ram_cpy(long addr, const void* src, long nBytes)
{
	const unsigned char* ptr = (const unsigned char*) src;
	
	for(int i=0; i<nBytes; ++i)
		ram_write(addr+i, ptr[i], 1);
}

void ram_close()
{
	
}

#endif /* AVRMEM_H_ */