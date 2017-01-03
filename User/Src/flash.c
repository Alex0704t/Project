/*
 * flash.c
 *
 *  Created on: 23 θώνÿ 2015 γ.
 *      Author: Aleksey
 */

#include "flash.h"

void Flash_Unlock(void)
{
  if((FLASH->CR & FLASH_CR_LOCK) != RESET)
  {
    /* Authorize the FLASH Registers access */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
  }
}

void Flash_Lock(void)
{
  /* Set the LOCK Bit to lock the FLASH Registers access */
  FLASH->CR |= FLASH_CR_LOCK;
}

void Flash_Erase_Sector(uint32_t Sector)
{
	while(FLASH->SR & FLASH_SR_BSY);//wait end another flash operation
	FLASH->CR &= 0xFFFFFCFF;//clear flash control register
	FLASH->CR |= FLASH_CR_PSIZE_1;//program size x32 for VCC 2.7V to 3.6V
	FLASH->CR &= FLASH_CR_SNB;//clear sector number
	FLASH->CR |= FLASH_CR_SER | (Sector << 3);//set number of sector and sector erase bit
	FLASH->CR |= FLASH_CR_STRT;//start erase
	while(FLASH->SR & FLASH_SR_BSY);//wait for end erase
}

void Flash_Program(uint32_t Address, uint32_t Data)
{
	while(FLASH->SR & FLASH_SR_BSY);//wait end another flash operation
	FLASH->CR &= 0xFFFFFCFF;//clear flash control register
	FLASH->CR |= FLASH_CR_PSIZE_1;//program size x32 for VCC 2.7V to 3.6V
	FLASH->CR |= FLASH_CR_PG;//flash programming operation
	*(uint32_t*)Address = Data;
	while(FLASH->SR & FLASH_SR_BSY);//wait for last operation to be completed
    FLASH->CR &= (~FLASH_CR_PG);//deactivated
}

