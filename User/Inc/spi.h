/*
 * spi.h
 *
 *  Created on: 05.06.2015
 *      Author: Aleksey
 */

#ifndef USER_SPI_H_
#define USER_SPI_H_

//#include "../Device/stm32f4xx.h"
#include "user.h"

#define SPI1_BUFSIZE 10
#define LCD_BUFSIZE 51

//--------------------------------------------------------------
// I2S-Clock Defines :
// HSE-Clock = 8 MHz, PLL_M = 8
// I2S_VCO = (HSE / PLL_M) * PLLI2S_N => 271 MHz
// I2S_CLK = I2S_VCO / PLLI2S_R => 135,5 MHz
//--------------------------------------------------------------
#define CS43L22_PLLI2S_N   271
#define CS43L22_PLLI2S_R   2

enum {READ = 0,
	WRITE = 1};

void SPI1_Init(void);
void SPI1_DMA_Init(void);
void WriteSPI1(uint8_t addr, uint8_t *data, uint8_t size);
void ReadSPI1(uint8_t addr, uint8_t *data, uint8_t size);
void WriteRegSPI1(uint8_t addr, uint8_t value);
uint8_t ReadRegSPI1(uint8_t addr);
void SetRegSPI1(uint8_t addr, uint8_t value);
uint8_t CheckRegSPI1(uint8_t addr, uint8_t value);
void SPI1_DMA_Transfer(uint8_t write, uint8_t addr, uint8_t data, uint16_t size);
void GetAxData(void);


void SPI2_Init(void);
void SPI2_DMA_Init(void);
void Send_SPI2_byte(uint8_t data);
void Send_SPI2_data(uint8_t* data, uint16_t length);
void Send_SPI2_DMA(__IO uint8_t* data, uint16_t length);
void Send_SPI2_buff();

void I2S3_Init(void);
void I2S3_DMA_Init(uint32_t size, int16_t *data);
void I2S3_DMA_DeInit(void);
void Send_SPI3(uint8_t data);

#endif /* USER_SPI_H_ */
