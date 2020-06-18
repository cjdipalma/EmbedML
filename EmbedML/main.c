/*
 * EmbedML.c
 *
 * Created: 1/13/2020 6:13:54 PM
 * Author : Chris
 */ 

#include "avrmem.h"
#include "diskio.h"
#include "helper.h"
#include <stdlib.h>

int main(void)
{
	// Advanced print-based debugging
	DDRA = 0b00000011;
	
	/*
	disk_initialize();
	print_bit(1, 0);
	
	avr_wait(30000);
	
	print_bit(0, 0);
	
	avr_wait(15000);
	
	// Was testing code
	BYTE* a = malloc(sizeof(BYTE) * 9);
	disk_readp(a, 0, 0, 9);
	
	//for(int i=0; i<8; ++i)
	//	print_byte(a[i]);
	
	print_bit(1, 0);
	return 0;
	
	//FATFS* fs = malloc(sizeof(FATFS));
	
	//FRESULT fr = pf_mount(fs);
	
	/*
	// OK
	print_bit(1, 0);
	if (fr == FR_OK) print_bit(1, 1);
	
	avr_wait(1500);
	print_bit(0, 0);
	print_bit(0, 1);
	
	avr_wait(1000);
	
	// NR
	print_bit(1, 0);
	if (fr == FR_NOT_READY) print_bit(1, 1);
	
	avr_wait(1500);
	print_bit(0, 0);
	print_bit(0, 1);
	
	avr_wait(1000);
	
	// ERR
	print_bit(1, 0);
	if (fr == FR_DISK_ERR) print_bit(1, 1);
	
	avr_wait(1500);
	print_bit(0, 0);
	print_bit(0, 1);
	
	avr_wait(1000);
	
	// NF
	print_bit(1, 0);
	if (fr == FR_NO_FILESYSTEM) print_bit(1, 1);
	
	avr_wait(1500);
	print_bit(0, 0);
	print_bit(0, 1);
	
	avr_wait(1000);
	
	print_bit(1, 0);
	return 0;
	//*/
	
	//* Testing RAM
	long address = 21;
	
	// Reset RAM
	ram_init();
	DDRC = 0b00010111;
	PORTC = 0b00010001;
	
	avr_wait(8000);
	ram_write(address, 5, 6);
	ram_write(address+1, 3, 2);
	avr_wait(5000);
	unsigned char data = ram_read(address);
	unsigned char data2 = ram_read(address+1);
	
	// Testing uses two LEDs to represent binary
	for(int i=0; i<6; ++i)
	{
		print_byte(ram_read(address+i));
		avr_wait(AVR_WAIT_TIME * 2);
	}
	
	ram_close();
}

