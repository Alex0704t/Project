//--------------------------------------------------------------
// File     : stm32_ub_atadrive.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_ATADRIVE_H
#define __STM32F4_UB_ATADRIVE_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include <FAT/fatfs/diskio.h>
#include "stm32f4xx.h"






//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void ATADrive_Init(void);
uint8_t ATADrive_CheckMedia(void);
int ATA_disk_initialize(void);
int ATA_disk_status(void);
int ATA_disk_read(BYTE *buff, DWORD sector, BYTE count);
int ATA_disk_write(const BYTE *buff, DWORD sector, BYTE count);
int ATA_disk_ioctl(BYTE cmd, void *buff); 


//--------------------------------------------------------------
#endif // __STM32F4_UB_ATADRIVE_H
