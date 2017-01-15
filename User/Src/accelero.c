/*
 * accelero.c
 *
 *  Created on: 05.06.2015
 *      Author: Aleksey
 */

#include "accelero.h"

extern __IO uint8_t AxData[7];


void CS_Init(void)
{
/*PE3 chip select for accelerometer*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

	GPIOE->MODER |= GPIO_MODER_MODER3_0;//PE3 output
	GPIOE->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR3;//low speed
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT_3;//push-pull
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR3;//no pull-up & pull-down
}

void AcceleroINT2_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;//GPIOA clock enable
	GPIOA->MODER &= ~GPIO_MODER_MODER1;//Input PE1
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR1;//no pull-up & pull-down
	EXTI->IMR |= EXTI_IMR_MR1;//no mask interrupt EXTI1
	EXTI->RTSR |= EXTI_RTSR_TR1;//rising trigger event EXTI1
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PE;//EXTI1 source PE1
	WriteRegSPI1(CTRL_REG3, CTRL_REG3_INT2_EN);//INT2 enable
	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(EXTI1_IRQn, 4);
	NVIC_EnableIRQ(EXTI1_IRQn);//IRQ handler
}




void Accelero_Init(void)
{
	//SPI1_Init();
	SPI1_DMA_Init();
	CS_Init();
	CS_OFF();
}

void WriteSPI1(uint8_t addr, uint8_t *data, uint8_t size)
{
	uint8_t temp = 0;
	CS_ON();
	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = addr;
	while(!(SPI1->SR & SPI_SR_RXNE));
	temp = SPI1->DR;
	while(size--)
	{
	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = *data++;
	while(!(SPI1->SR & SPI_SR_RXNE));
	temp = SPI1->DR;
	}
	CS_OFF();
}

void ReadSPI1(uint8_t addr, uint8_t *data, uint8_t size)
{
	uint8_t temp = 0;
	addr |= 0x80;
	CS_ON();
	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = addr;
	while(!(SPI1->SR & SPI_SR_RXNE));
	temp = SPI1->DR;
	while(size--)
	{
	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = DUMMY;
	while(!(SPI1->SR & SPI_SR_RXNE));
	*data++ = SPI1->DR;
	}
	CS_OFF();
}

void WriteRegSPI1(uint8_t addr, uint8_t value)
{
	WriteSPI1(addr, &value, 1);
}

uint8_t ReadRegSPI1(uint8_t addr)
{
	uint8_t data = 0;
	ReadSPI1(addr, &data, 1);
	return data;
}

void SetRegSPI1(uint8_t addr, uint8_t value)
{
	uint8_t temp = ReadRegSPI1(addr);
	temp |= value;
	WriteSPI1(addr, &temp, 1);
}

uint8_t CheckRegSPI1(uint8_t addr, uint8_t value)
{
	uint8_t data = 0;
	ReadSPI1(addr, &data, 1);
	if(data == value)
		return SET;
	else
		return RESET;
}

void Accelero_EN(void)
{
	if(CheckRegSPI1(WHO_I_AM, I_AM_LIS3DSH))
	{
	LED_1_ON();
	LED_3_ON();
	delay_ms(300);
	LED_1_OFF();
	LED_3_OFF();
	}
	WriteRegSPI1(CTRL_REG4, CTRL_REG4_ODR_400|CTRL_REG4_XY);
	if(CheckRegSPI1(CTRL_REG4, CTRL_REG4_ODR_400|CTRL_REG4_XY))
	{
	LED_0_ON();
	LED_2_ON();
	delay_ms(300);
	LED_0_OFF();
	LED_2_OFF();
	}
}

void Dir_Led(void)
{
	//uint8_t temp;
	int16_t XData = 0;
	int16_t YData = 0;
	//int16_t ZData = 0;
	//GetAxData();
	XData = (int16_t) (AxData[2]<<8)|	AxData[1];
	/*temp = 	(AxData[1]<<8)|	AxData[0];
		if (temp >= 0x8000)
		{
			temp -= 0x8000;
			XxData = (int16_t) temp;
		}
		else
		{
			temp = ~temp + 1;
			XxData = -1* (int16_t) temp;
		}*/
	YData = (int16_t) (AxData[4]<<8)|	AxData[3];
	//ZData = (int16_t) (AxData[6]<<8)|	AxData[5];
	if (XData > 800)
	{
		LED_2_OFF();
		LED_0_ON();
	}
	else if (XData < -800)
	{
		LED_0_OFF();
		LED_2_ON();
	}
	else
	{
		LED_0_OFF();
		LED_2_OFF();
	}
	if (YData > 800)
	{
		LED_1_OFF();
		LED_3_ON();
	}
	else if (YData < -800)
	{
		LED_3_OFF();
		LED_1_ON();
	}
	else
	{
		LED_1_OFF();
		LED_3_OFF();
	}
}

void MovDetectEN(void)
{
	AcceleroINT2_Init();//enable EXTI1 interrupt
	WriteRegSPI1(CTRL_REG1, CTRL_REG1_SM1_EN|CTRL_REG1_SM1_PIN);//SM1 enable, interrupt INT2 pin
	WriteRegSPI1(CTRL_REG3, CTRL_REG3_INT2_EN|CTRL_REG3_IEA);//INT2 enable, active HIGH
	WriteRegSPI1(CTRL_REG4, CTRL_REG4_XYZ|CTRL_REG4_ODR_100);//100 Hz 3-axes enable
	WriteRegSPI1(CTRL_REG5, 0x00);//scale 2G
	WriteRegSPI1(THRS1_1, 0x55);//threshold value
	WriteRegSPI1(ST1_1, GNTH1);//axes grater then threshold 1
	WriteRegSPI1(ST2_1, CONT);//Continues execution from RESET-	POINT
	WriteRegSPI1(MASK1_B, MASK_XY);//enable x, y
	WriteRegSPI1(MASK1_A, MASK_XY);//enable x, y
	WriteRegSPI1(SETT1, SETT_SITR);//enable CONT command
}
