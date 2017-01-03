/*
 * i2c.h
 *
 *  Created on: 23.10.2015
 *      Author: Aleksey Totkal
 */

#ifndef USER_I2C_H_
#define USER_I2C_H_

#include "user.h"

#define CS43L22_ADDR_WRITE      0x94
#define CS43L22_ADDR_READ       0x95
#define I2C1_TIMEOUT            3000
#define I2C1_START              I2C1->CR1 |= I2C_CR1_START//start generation
#define I2C1_STOP               I2C1->CR1 |= I2C_CR1_STOP//stop generation
#define I2C1_ADDR_R             I2C1->DR = CS43L22_ADDR_READ//receiver mode
#define I2C1_ADDR_W             I2C1->DR = CS43L22_ADDR_WRITE//transmitter mode
#define I2C1_SET_ACK            I2C1->CR1 |= I2C_CR1_ACK//acknowledge generation
#define I2C1_CLEAR_ACK          I2C1->CR1 &= ~I2C_CR1_ACK//no acknowledge generation
#define I2C1_SET_POS            I2C1->CR1 |= I2C_CR1_POS//Set Acknowledge/PEC Position
#define I2C1_CLEAR_POS          I2C1->CR1 &= ~I2C_CR1_POS//Clear Acknowledge/PEC Position
#define I2C1_CLEAR_FLAGS        do {                           \
                                    __IO uint32_t tempreg = 0; \
                                    tempreg = I2C1->SR1;       \
                                    tempreg = I2C1->SR2;       \
                                    (void)tempreg;             \
                                   }                           \
                                while(0)
#define I2C1_WAIT_FOR(event)    for(uint32_t timeout = I2C1_TIMEOUT;    \
                                ((timeout > 0) && !(event)); timeout--);\
                                if (!(event)) return I2C1_Timeout();
#define I2C1_SEND_DATA(data)    I2C1->DR = (data)
#define I2C1_SWR_RST            do {                            \
                                    I2C1->CR1 |= I2C_CR1_SWRST; \
                                    I2C1->CR1 &= ~I2C_CR1_SWRST;\
                                    }                           \
                                while(0)

void I2C1_Init(void);
void I2C1_DeInit(void);
int16_t I2C1_Timeout(void);
int16_t I2C1_WriteByte(uint8_t addr, uint8_t data);
int16_t I2C1_ReadByte(uint8_t slave_addr, uint8_t addr);
uint8_t Transmit_I2C1(uint8_t *data, uint8_t size);
uint8_t Receive_I2C1(uint8_t *data, uint8_t size);
void WriteReg_I2C1(uint8_t addr, uint8_t value);
void ReadReg_I2C1(uint8_t addr, uint8_t *value);
void SetReg_I2C1(uint8_t addr, uint8_t value);
void ClearReg_I2C1(uint8_t addr, uint8_t mask);

#endif /* USER_I2C_H_ */
