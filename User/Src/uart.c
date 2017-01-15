/*
 * uart.c
 *
 *  Created on: 25 θώνÿ 2015 γ.
 *      Author: Aleksey
 */

#include "uart.h"

uint8_t *tx_uart1_ptr;
uint8_t *rx_uart1_ptr;
uint8_t *tx_uart2_ptr;
uint8_t *rx_uart2_ptr;

uint8_t tx_uart1_count;
uint8_t rx_uart1_count;
uint8_t tx_uart2_count;
uint8_t rx_uart2_count;

void USART1_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	/* baudrate = 115200 APB2clock = 84 MHz
	 * DIV = APB2clock/(8*(2-OVER8)*baudrate)=
	 * = 84000000 /(8 * 2 * 115200) = 45.57
	 * DIV mantissa = 0d45 = 0x2D
	 * DIV fraction = 16*0d0.57 = 0d9.12
	 * nearest real number 0d9 = 0x9
	 * USART_BRR = 0x2D9
	 */
	USART1->BRR = 0x2D9;
	USART1->CR1 |= USART_CR1_UE|USART_CR1_RE|USART_CR1_TE;//USART1 ON, TX ON, RX ON
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	//PB6 - TX, PB7 - RX
	GPIOB->MODER |= GPIO_MODER_MODER6_1|GPIO_MODER_MODER7_1;//PB6, PB7 alternate function
	GPIOB->AFR[0] |= 0x77000000;//PB6, PB7 AF7
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR7_0;//PB7 pull-up
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_0;//PB6 medium speed

	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(USART1_IRQn, 4);
	NVIC_EnableIRQ(USART1_IRQn);//IRQ handler

}

void USART2_Init()
{
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	/* baudrate = 115200 APB1clock = 42 MHz
	 * DIV = APB2clock/(8*(2-OVER8)*baudrate)=
	 * = 42000000 /(8 * 2 * 115200) = 22.79
	 * DIV mantissa = 0d22 = 0x16
	 * DIV fraction = 16*0d0.79 = 0d12.57
	 * nearest real number 0d13 = 0xD
	 * USART_BRR = 0x16D
	 */
	USART2->BRR = 0x16D;
	USART2->CR1 |= USART_CR1_UE|USART_CR1_RE|USART_CR1_TE;//USART1 ON, TX ON, RX ON
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	//PA2 - TX, PA3 - RX
	GPIOA->MODER |= GPIO_MODER_MODER2_1|GPIO_MODER_MODER3_1;//PA2, PA3 alternate function
	GPIOA->AFR[0] |= 0x00007700;//PA2, PA3 AF7
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_0;//PA3 pull-up
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_0;//PA2 medium speed

	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(USART2_IRQn, 4);
	NVIC_EnableIRQ(USART2_IRQn);//IRQ handler
}

void SendByte1(uint8_t data)
{
	while(!(USART1->SR & USART_SR_TC));//wait
	USART1->DR = data;
}

void SendByte2(uint8_t data)
{
	while(!(USART2->SR & USART_SR_TC));//wait
	USART2->DR = data;
}

void SendData1(uint8_t *data)
{
	while(*data)
	{
	SendByte1(*data);
	data++;
	}
	SendByte1('\0');
}

void SendData2(uint8_t *data)
{
	while(*data)
	{
	SendByte2(*data);
	data++;
	}
	SendByte2('\0');
}

uint8_t ReceiveByte1(void)
{
	uint8_t data;
	while(!(USART1->SR & USART_SR_TC));//wait
	if(USART1->SR & USART_SR_RXNE)
		data = USART1->DR;
	return data;
}

uint8_t ReceiveByte2(void)
{
	uint8_t data;
	while(!(USART2->SR & USART_SR_TC));//wait
	if(USART2->SR & USART_SR_RXNE)
		data = USART2->DR;
	return data;
}

void ReceiveData1(uint8_t *data)
{
*data = ReceiveByte1();
while(*data)
	{
	data++;
	*data = ReceiveByte1();
	}
*data = '\0';
}

void ReceiveData2(uint8_t *data)
{
*data = ReceiveByte2();
while(*data)
	{
	data++;
	*data = ReceiveByte2();
	}
*data = '\0';
}

void SendDataIT1(uint8_t *data, uint8_t size)
{
	tx_uart1_ptr = data;
	tx_uart1_count = size;
	USART1->CR1 |= USART_CR1_TXEIE;
}

void SendDataIT2(uint8_t *data, uint8_t size)
{
	tx_uart2_ptr = data;
	tx_uart2_count = size;
	USART2->CR1 |= USART_CR1_TXEIE;
}

void ReceiveDataIT1(uint8_t *data, uint8_t size)
{
	rx_uart1_ptr = data;
	rx_uart1_count = size;
	USART1->CR1 |= USART_CR1_RXNEIE;
}

void ReceiveDataIT2(uint8_t *data, uint8_t size)
{
	rx_uart2_ptr = data;
	rx_uart2_count = size;
	USART2->CR1 |= USART_CR1_RXNEIE;
}

void DMA_USART1_Tx_Init(void)
{
	/*USART1 TX DMA2 Stream7 channel4*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;//DMA2 enable
	DMA2_Stream7->CR |= DMA_SxCR_CHSEL_2;//channel4 select
	DMA2_Stream7->CR |= DMA_SxCR_PL_0;//medium priority
	DMA2_Stream7->CR &= ~(DMA_SxCR_MSIZE|DMA_SxCR_PSIZE);//memory & peripheral data size 1 byte
	DMA2_Stream7->CR |= DMA_SxCR_MINC;//memory increment mode
	DMA2_Stream7->CR |= DMA_SxCR_DIR_0;//memory to peripheral direction
	DMA2_Stream7->CR |= DMA_SxCR_TCIE;//transfer complete interrupt enable
	DMA2_Stream7->PAR = (uint32_t)&(USART1->DR);//peripheral address
	USART1->CR3 |= USART_CR3_DMAT;//DMA enable transmitter

	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(DMA2_Stream7_IRQn, 4);
	NVIC_EnableIRQ(DMA2_Stream7_IRQn);//IRQ handler
}

void SendData1DMA(uint8_t *data, uint8_t size)
{
	DMA2_Stream7->M0AR = (uint32_t)(data);//memory address
	DMA2_Stream7->NDTR = size;//data size
	DMA2_Stream7->CR |= DMA_SxCR_EN;//stream enable
}

void DMA_USART2_Tx_Init(void)
{
	/*USART2 TX DMA1 Stream6 channel4*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;//DMA1 enable
	DMA1_Stream6->CR |= DMA_SxCR_CHSEL_2;//channel4 select
	DMA1_Stream6->CR |= DMA_SxCR_PL_0;//medium priority
	DMA1_Stream6->CR &= ~(DMA_SxCR_MSIZE|DMA_SxCR_PSIZE);//memory & peripheral data size 1 byte
	DMA1_Stream6->CR |= DMA_SxCR_MINC;//memory increment mode
	DMA1_Stream6->CR |= DMA_SxCR_DIR_0;//memory to peripheral direction
	DMA1_Stream6->CR |= DMA_SxCR_TCIE;//transfer complete interrupt enable
	DMA1_Stream6->PAR = (uint32_t)&(USART2->DR);//peripheral address
	USART2->CR3 |= USART_CR3_DMAT;//DMA enable transmitter

	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(DMA1_Stream6_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);//IRQ handler
}

void SendData2DMA(uint8_t *data, uint8_t size)
{
	DMA1_Stream6->M0AR = (uint32_t)(data);//memory address
	DMA1_Stream6->NDTR = size;//data size
	DMA1_Stream6->CR |= DMA_SxCR_EN;//stream enable
}

void DMA_USART1_Rx_Init(void)
{
	/*USART1 RX DMA2 Stream2 channel4*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;//DMA2 enable
	DMA2_Stream2->CR |= DMA_SxCR_CHSEL_2;//channel4 select
	DMA2_Stream2->CR |= DMA_SxCR_PL_0;//medium priority
	DMA2_Stream2->CR &= ~(DMA_SxCR_MSIZE|DMA_SxCR_PSIZE);//memory & peripheral data size 1 byte
	DMA2_Stream2->CR |= DMA_SxCR_MINC;//memory increment mode
	DMA2_Stream2->CR &= ~DMA_SxCR_DIR;//peripheral to memory direction
	DMA2_Stream2->CR |= DMA_SxCR_TCIE;//transfer complete interrupt enable
	DMA2_Stream2->PAR = (uint32_t)&(USART1->DR);//peripheral address
	USART1->CR3 |= USART_CR3_DMAR;//DMA enable receiver

	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(DMA2_Stream2_IRQn, 4);
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);//IRQ handler
}

void ReceiveData1DMA(uint8_t *data, uint8_t size)
{
	DMA2_Stream2->M0AR = (uint32_t)(data);//memory address
	DMA2_Stream2->NDTR = size;//data size
	DMA2_Stream2->CR |= DMA_SxCR_EN;//stream enable
}


void DMA_USART2_Rx_Init(void)
{
	/*USART2 RX DMA1 Stream5 channel4*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;//DMA2 enable
	DMA1_Stream5->CR |= DMA_SxCR_CHSEL_2;//channel4 select
	DMA1_Stream5->CR |= DMA_SxCR_PL_0;//medium priority
	DMA1_Stream5->CR &= ~(DMA_SxCR_MSIZE|DMA_SxCR_PSIZE);//memory & peripheral data size 1 byte
	DMA1_Stream5->CR |= DMA_SxCR_MINC;//memory increment mode
	DMA1_Stream5->CR &= ~DMA_SxCR_DIR;//peripheral to memory direction
	DMA1_Stream5->CR |= DMA_SxCR_TCIE;//transfer complete interrupt enable
	DMA1_Stream5->PAR = (uint32_t)&(USART2->DR);//peripheral address
	USART2->CR3 |= USART_CR3_DMAR;//DMA enable receiver

	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(DMA1_Stream5_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);//IRQ handler
}

void ReceiveData2DMA(uint8_t *data, uint8_t size)
{
	DMA1_Stream5->M0AR = (uint32_t)(data);//memory address
	DMA1_Stream5->NDTR = size;//data size
	DMA1_Stream5->CR |= DMA_SxCR_EN;//stream enable
}

