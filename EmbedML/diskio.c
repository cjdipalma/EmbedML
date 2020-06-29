/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "helper.h"

void set_di(char value)
{
	if(value != 0)	SET_BIT(SD_PORT, DI);
	else			CLR_BIT(SD_PORT, DI);
}

void spi_init()
{
	/* Old code */
	SET_BIT(SD_DDR, DI);
	CLR_BIT(SD_DDR, DO);
	SET_BIT(SD_DDR, SCK);
	SET_BIT(SD_DDR, CS);
	
	SET_BIT(SD_PORT, DO); // Pull-up
	
//	/* USART code from ATmega128 manual */
//	UBRR1 = 0;
//	/* Setting the XCKn port pin direction as output, enables master mode. */
//	SET_BIT(DDRD, 4);
//	/* Set MSPI mode of operation and SPI data mode 0. */
//	UCSR1C = (1<<UMSEL11)|(1<<UMSEL10)|(0<<UCSZ10)|(0<<UCPOL1);
//	/* Enable receiver and transmitter. */
//	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
//	/* Set baud rate. 
//	   IMPORTANT: The Baud Rate must be set after the transmitter is enabled */
//
//	UBRR1 = ubrr_v;
}

void write_spi(unsigned char data)
{
	for(int i=7; i>=0; --i)
	{
		set_di(GET_BIT(data, i));
		SET_BIT(SD_PORT, SCK);
		avr_wait(1);
		CLR_BIT(SD_PORT, SCK);
		avr_wait(1);
	}
}

unsigned char read_spi()
{
	CLR_BIT(SD_PORT, CS);
	char data = 0x00;
	
	for(int i=7; i>=0; --i)
	{
		SET_BIT(SD_PORT, SCK);
		avr_wait(1);
		if(GET_BIT(SD_PIN, DO) != 0)
			SET_BIT(data, i);
		CLR_BIT(SD_PORT, SCK);
		avr_wait(1);
	}
	
	return data;
}

void dummy_clocks(int n)
{
	SET_BIT(SD_PORT, CS);
	for(int i=0; i<n; ++i)
	{
		write_spi(0xFF);
	}
}

void proceed()
{
	write_spi(0xFF);
}

void command(char cmd, unsigned long arg, char crcbits)
{
	CLR_BIT(SD_PORT, CS);
	write_spi(0xFF);
	write_spi(cmd);
	write_spi((unsigned char) (arg >> 24));
	write_spi((unsigned char) (arg >> 16));
	write_spi((unsigned char) (arg >> 8));
	write_spi((unsigned char) arg);
	write_spi(crcbits);
	
	SET_BIT(SD_PORT, DI);
}


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	DSTATUS stat;

	// Put your code here
	avr_wait(AVR_WAIT_TIME);
	spi_init();
	
	// Following comment is from when I was trying to use USART. Irrelevant now but leaving it for reference.
	// Baud rate 100000, so UBRR1 = 4

	char buff;
	int count = 0;
	
	// CMD0
	do 
	{
		SET_BIT(SD_PORT, CS);
		dummy_clocks(12);
		command(0x40, 0x00000000, 0x95);
		proceed();
		
		do
		{
			buff = read_spi();
			// print_byte(buff);
			++count;
		} while (buff != 0x01 && (count < 10));
		count = 0;
	} while (buff != 0x01);
	
	write_spi(0xFF);
	SET_BIT(SD_PORT, CS);
	
	// Check voltage (CMD8)
	char a[10] = "qwertyuiop";
	
	dummy_clocks(5);
	command(0x48, 0x000001AA, 0x87);
	proceed();
	 
	for(int i=0; i<7; ++i)
	{
		a[i] = read_spi();
	}
	SET_BIT(SD_PORT, CS);
	
	/* Print the voltage check pattern (passed 6/15/20)
	for(int i=0; i<5; ++i)
	{
		print_byte(a[i]);
		avr_wait(AVR_WAIT_TIME*4/5);
	}
	
	//return 0;
	//*/
	
	// Check OCR (CMD58)
	dummy_clocks(5);
	command(0x7A, 0x00000000, 0xFD);
	proceed();
	
	for(int i=0; i<7; ++i)
	{
		a[i] = read_spi();
	}
	SET_BIT(SD_PORT, CS);
	
	//* Check contents of OCR (seems to pass 6/16/20)
	for(int i=0; i<5; ++i)
	{
		print_byte(a[i]);
		avr_wait(AVR_WAIT_TIME*4/5);
	}
	avr_wait(AVR_WAIT_TIME);
	
	//return 0;
	//*/
	
	// Loops which have the potential to be infinite make me very sad :(
	// Passed 6/16/20, praise be to Jake Herman
	do 
	{
		// CMD55
		dummy_clocks(8);
		command(0x77, 0x00000000, 0x65);
		//proceed();
		
		do
		{
			buff = read_spi();
			// print_byte(buff);
			++count;
		} while (buff != 0x01 && (count < 10));
		count = 0;
		//*
		write_spi(0xFF);
		for (int i=0; i<7; ++i)
			read_spi();
		SET_BIT(SD_PORT, CS);
		//*/
	
		// ACMD41
		SET_BIT(SD_PORT, CS);
		dummy_clocks(10);
		command(0x69, 0x40000000, 0x77);
		//proceed();
		
		do
		{
			buff = read_spi();
			// print_byte(buff);
			++count;
		} while (!(buff == 0x00 || buff == 0x01 || buff == 0x05) && (count < 10));
		count = 0;
		if (!(buff == 0x00 || buff == 0x01 || buff == 0x05 || buff == 0xFF))
		{
			print_byte(buff);
			return STA_NOINIT; // Something wrong with SD card
		}
		
		write_spi(0xFF);
		for (int i=0; i<7; ++i)
			read_spi();
		SET_BIT(SD_PORT, CS);
	} while (buff != 0x00); // Return to CMD55 if command returned idle
	
	// Check OCR (CMD58)
	dummy_clocks(5);
	command(0x7A, 0x00000000, 0xFD);
	proceed();
	
	for(int i=0; i<7; ++i)
	{
		a[i] = read_spi();
	}
	SET_BIT(SD_PORT, CS);
	
	//* Check contents of OCR (seems to pass 6/16/20)
	for(int i=0; i<5; ++i)
	{
		print_byte(a[i]);
		avr_wait(AVR_WAIT_TIME*4/5);
	}
	avr_wait(AVR_WAIT_TIME);
	//*/
	
	// Set block size (CMD50)
	// For SDHC, block size is always 512 bytes, so this command is optional
	/*
	dummy_clocks(5);
	command(0x50, 0x00000200, 0xFF);
	
	for(int i=0; i<8; ++i)
	{
		a[i] = read_spi();
	}
	SET_BIT(SD_PORT, CS);
	
	// Check that the block size command accepts (passed 6/15/20)
	//*
	for(int i=0; i<5; ++i)
	{
		print_byte(a[i]);
		avr_wait(AVR_WAIT_TIME*4/5);
	}
	//*/
	
	// Deselect SD card before leaving init
	SET_BIT(SD_PORT, CS);

	return 0x00;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
)
{
	DRESULT res;

	// Put your code here
	//print_byte(sector);
	//avr_wait(AVR_WAIT_TIME*4/5);
	//print_byte(offset);
	//avr_wait(AVR_WAIT_TIME*4/5); 
	//print_byte(count);
	//avr_wait(AVR_WAIT_TIME*4/5);
	
	// CoPyPaStA
	unsigned short i, pos = 0;
	dummy_clocks(5);
	command(0x51, sector, 0x55);
	proceed();
	
	for(i=0; i<6; ++i)
	{
		print_byte(read_spi());
		avr_wait(AVR_WAIT_TIME*4/5);
	}
	
	return res;
	
	for(i=0; i<10 && read_spi() != 0x00; i++) {} // wait for 0
	
	for(i=0; i<10 && read_spi() != 0xFE; i++) {} // wait for data start
		
	for(i=0; i<offset; i++) // "skip" bytes
		read_spi();
	
	for(i=0; i<count; i++) // read len bytes
		buff[i] = read_spi();
	
	for(i+=offset; i<512; i++) // "skip" again
		read_spi();
	
	// Skip checksum
	read_spi();
	read_spi();
	
	SET_BIT(SD_PORT, CS);

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/
// Unimplemented for now

DRESULT disk_writep (
	const BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	DRESULT res;


	if (!buff) {
		if (sc) {

			// Initiate write process

		} else {

			// Finalize write process

		}
	} else {

		// Send data to the disk

	}

	return res;
}

