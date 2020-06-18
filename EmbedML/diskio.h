/*-----------------------------------------------------------------------
/  PFF - Low level disk interface module include file    (C)ChaN, 2014
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include "pff.h"


/* I/O Ports */
#define SD_PORT PORTD
#define SD_PIN PIND
#define SD_DDR DDRD
#define CS 6
#define DI 3
#define SCK 4
#define DO 2


/* Useful macros */
#define SET_BIT(p,i) ((p) |=  (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) &   (1 << (i)))
#define WRT_BYTE(p,b,i) ((p) |=  (b << (i * 8)))
#define GET_BYTE(p,i) ((p) &   (11111111UL << (i * 8)))


/* Status of Disk Functions */
typedef BYTE	DSTATUS;


/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Function succeeded */
	RES_ERROR,		/* 1: Disk error */
	RES_NOTRDY,		/* 2: Not ready */
	RES_PARERR		/* 3: Invalid parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
DRESULT disk_readp (BYTE* buff, DWORD sector, UINT offser, UINT count);
DRESULT disk_writep (const BYTE* buff, DWORD sc);

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */


/* Drive I/O functions */

void spi_init();
void set_di(char value);
void write_spi(unsigned char data);
unsigned char read_spi();
void dummy_clocks(int n);
void proceed();
void command(char cmd, unsigned long int arg, char crcbits);


#ifdef __cplusplus
}
#endif

#endif	/* _DISKIO_DEFINED */
