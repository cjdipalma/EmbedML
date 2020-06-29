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
//#include "mnist-avr.c"

int main(void)
{
	// LED-based debugging
	DDRA = 0b00000011;
	
	//float a = 0.6f;
	//unsigned char const* const p = (unsigned char const*)&a;
	//for(int i=0; i<4; ++i)
	//{
	//	print_byte(p[i]);
	//	avr_wait(AVR_WAIT_TIME);
	//}
	//return 0;
	
	// MNIST test
	{
		// Start the RAM
		ram_init();

		// Allocate memory space
		// In translation to AVR, m becomes a long indicating the base address of the array
		//long m = avr_alloc(test_memory_size());

		// Initialize memory space
		//test_initialize();

		float* x = malloc(784 * sizeof(float));
		for(int i=0; i<784; ++i)
			x[i] = (i % 256) / 255.0;
		
		// ram_cpy for 627200 byte writes
		for(unsigned long i=0; i<200; ++i)
			ram_cpy(i * 10000UL, x, 784 * sizeof (float));
		
		// ram_read_float for 50000 float reads (200000 bytes)
		for(unsigned long i=0; i<50000UL; ++i)
			ram_read_float(i * 4);
		
		// ram_write_float for 50000 float writes (200000 bytes)
		for(unsigned long i=0; i<50000UL; ++i)
			ram_write_float(i * 4, 0.6f);
		
		// ram_write for 500000 byte writes
		ram_write(95, 185, 500000UL);
		
		print_bit(1, 0);
		
		//free(y);
	
		//avr_free(m);
		/*
		free(train_y);
		free(train_x);
		free(test_y);
		free(test_x);
		*/
		ram_close();
	}
	//volatile int i, j;
	//for(i=0; i<10; ++i)
	//{
	//	for(j=0; j<0x186A0UL; ++j) {}
	//}
	return 0;
	
	/*
	char res = disk_initialize();
	print_bit(1, 0);
	
	if(res != 0) return 0;
	
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
	
	//*
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
	
	/* Testing RAM
	long address = 21;
	
	// Reset RAM
	ram_init();
	
	avr_wait(8000);
	//ram_write(address, 5, 6);
	//ram_write(address+1, 3, 2);
	float a = 0.6f;
	unsigned char* src = (unsigned char*)&a;
	ram_cpy(address, src, 4);
	avr_wait(5000);
	//unsigned char data = ram_read(address);
	//unsigned char data2 = ram_read(address+1);
	
	//*
	// Testing uses two LEDs to represent binary
	//float b = ram_read_float(address);
	//print_byte(b * 10.0 + 0.0001);
	
	for(int i=0; i<5; ++i)
	{
		print_byte(ram_read(address+i));
		avr_wait(AVR_WAIT_TIME);
	}
	
	ram_close();
	//*/
}

