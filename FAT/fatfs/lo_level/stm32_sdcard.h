//--------------------------------------------------------------
// File     : stm32_sdcard.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_SDCARD_H
#define __STM32F4_SDCARD_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include <FAT/fatfs/diskio.h>
#include "stm32f4xx.h"
#include "main.h"
#include "user.h"
#include "spi.h"

#define SD_SPI_CLK_SLOW           SPI_CR1_BR_0|SPI_CR1_BR_2//baudrate Fpclk/64(656 kHz)
#define SD_SPI_CLK_FAST           0   //baudrate Fpclk/2(21 MHz)
//--------------------------------------------------------------
#define USE_DETECT_PIN            0  // (ohne Detect-Pin)
#define	USE_WRITE_PROTECTION	    0  // (ohne Schreibschutz)



//--------------------------------------------------------------
/* MMC/SD command */
//--------------------------------------------------------------
#define CMD0	  (0)        // GO_IDLE_STATE
#define CMD1	  (1)        // SEND_OP_COND (MMC)
#define	ACMD41	(0x80+41)  // SEND_OP_COND (SDC)
#define CMD8	  (8)        // SEND_IF_COND
#define CMD9	  (9)        // SEND_CSD
#define CMD10	  (10)       // SEND_CID
#define CMD12	  (12)       // STOP_TRANSMISSION
#define ACMD13	(0x80+13)  // SD_STATUS (SDC)
#define CMD16	  (16)       // SET_BLOCKLEN
#define CMD17	  (17)       // READ_SINGLE_BLOCK
#define CMD18	  (18)       // READ_MULTIPLE_BLOCK
#define CMD23	  (23)       // SET_BLOCK_COUNT (MMC)
#define	ACMD23	(0x80+23)  // SET_WR_BLK_ERASE_COUNT (SDC)
#define CMD24	  (24)       // WRITE_BLOCK
#define CMD25	  (25)       // WRITE_MULTIPLE_BLOCK
#define CMD32	  (32)       // ERASE_ER_BLK_START
#define CMD33	  (33)       // ERASE_ER_BLK_END
#define CMD38	  (38)       // ERASE
#define CMD55	  (55)       // APP_CMD
#define CMD58	  (58)       // READ_OCR
#ifndef NULL
#define NULL    0
#endif
#define SD_PRESENT        ((uint8_t)0x01)
#define SD_NOT_PRESENT    ((uint8_t)0x00)


//--------------------------------------------------------------
// Global Functions
//--------------------------------------------------------------
void SDCard_Init(void);
uint8_t UB_SDCard_CheckMedia(void);
DSTATUS MMC_disk_initialize(void);
DSTATUS MMC_disk_status(void);
DRESULT MMC_disk_read(BYTE *buff,DWORD sector,UINT count);
DRESULT MMC_disk_write(const BYTE *buff,DWORD sector,	UINT count);
DRESULT MMC_disk_ioctl(BYTE cmd,void *buff);




//--------------------------------------------------------------
#endif // __STM32F4_SDCARD_H
