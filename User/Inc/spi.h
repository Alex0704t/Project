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

#define LIS3DSH_BUFSIZE 7

//--------------------------------------------------------------
// I2S-Clock Defines :
// HSE-Clock = 8 MHz, PLL_M = 8
// I2S_VCO = (HSE / PLL_M) * PLLI2S_N => 271 MHz
// I2S_CLK = I2S_VCO / PLLI2S_R => 135,5 MHz
//--------------------------------------------------------------
#define CS43L22_PLLI2S_N   271
#define CS43L22_PLLI2S_R   2

enum {SPI1_DMA_disable      = 0x00,
      SPI1_DMA_LIS3DSH      = 0x01,
      SPI1_DMA_LIS3DSH_INT2 = 0x02
}SPI1_DMA_mode;

void SPI1_Init(void);
void SPI1_DeInit(void);
void SPI1_DMA_Init(void);
void SPI1_DMA_DeInit(void);

uint8_t SPI_ByteExchange(SPI_TypeDef *SPIx, uint8_t data);
void SPI_DataExchange(SPI_TypeDef *SPIx, uint8_t *in_data, uint8_t *out_data, uint8_t size);
void LIS3DSH_Write(uint8_t addr, uint8_t *data, uint8_t size);
void LIS3DSH_Read(uint8_t addr, uint8_t *data, uint8_t size);
void LIS3DSH_WriteReg(uint8_t addr, uint8_t value);
uint8_t LIS3DSH_ReadReg(uint8_t addr);
void LIS3DSH_SetReg(uint8_t addr, uint8_t value);
void LIS3DSH_ClearReg(uint8_t addr);
void LIS3DSH_ModReg(uint8_t addr, uint8_t mask, uint8_t value);
uint8_t LIS3DSH_CheckReg(uint8_t addr, uint8_t value);
void LIS3DSH_GetAxis(void);
inline void LIS3DSH_GetData(uint8_t *data);
inline void LIS3DSH_SetFlag();
inline void LIS3DSH_ResetFlag();
inline void LIS3DSH_WaitFlag();

void SPI2_Init(void);
void SPI2_DMA_Init(void);
void SPI2_Send_byte(uint8_t data);
void SPI2_Send_data(uint8_t* data, uint16_t length);
void SPI2_DMA_Send(__IO uint8_t* data, uint16_t length);
void SPI2_Send_buff();

void I2S3_Init(void);
void I2S3_DMA_Init(uint32_t size, int16_t *data);
void I2S3_DMA_DeInit(void);
void Send_SPI3(uint8_t data);

#endif /* USER_SPI_H_ */
