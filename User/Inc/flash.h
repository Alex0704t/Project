/*
 * flash.h
 *
 *  Created on: 23.09.2015
 *      Author: Aleksey
 */

#ifndef USER_FLASH_H_
#define USER_FLASH_H_

//#include "../Device/stm32f407xx.h"
#include "user.h"

#define FLASH_KEY1 0x45670123
#define FLASH_KEY2 0xCDEF89AB


void Flash_Unlock(void);
void Flash_Lock(void);
void Flash_Erase_Sector(uint32_t Sector);
void Flash_Program(uint32_t Address, uint32_t Data);

#endif /* USER_FLASH_H_ */
