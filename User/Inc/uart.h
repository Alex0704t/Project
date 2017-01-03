/*
 * uart.h
 *
 *  Created on: 25.07.2015
 *      Author: Aleksey
 */

#ifndef USER_UART_H_
#define USER_UART_H_

//#include "../Device/stm32f407xx.h"
#include "user.h"

#define BUFSIZE 30

void USART1_Init(void);
void USART2_Init(void);

void SendByte1(uint8_t data);
void SendByte2(uint8_t data);
void SendData1(uint8_t *data);
void SendData2(uint8_t *data);
uint8_t ReceiveByte1(void);
uint8_t ReceiveByte2(void);
void ReceiveData1(uint8_t *data);
void ReceiveData2(uint8_t *data);

void SendDataIT1(uint8_t *data, uint8_t size);
void SendDataIT2(uint8_t *data, uint8_t size);
void ReceiveDataIT1(uint8_t *data, uint8_t size);
void ReceiveDataIT2(uint8_t *data, uint8_t size);

void DMA_USART1_Tx_Init(void);
void SendData1DMA(uint8_t *data, uint8_t size);
void DMA_USART2_Tx_Init(void);
void SendData2DMA(uint8_t *data, uint8_t size);
void DMA_USART1_Rx_Init(void);
void ReceiveData1DMA(uint8_t *data, uint8_t size);
void DMA_USART2_Rx_Init(void);
void ReceiveData2DMA(uint8_t *data, uint8_t size);

#endif /* USER_UART_H_ */
