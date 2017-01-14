/*
 * adc.c
 *
 *  Created on: 21.07.2015
 *      Author: Aleksey
 */

#include "adc.h"


void ADC1_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;//ADC1 clock enable

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;//GPIOA clock enable
	GPIOA->MODER |= GPIO_MODER_MODER1;//PA1 analog input
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR1;//no pull-up & pull-down

	ADC1->CR1 &= ~ADC_CR1_RES;//ADC1 resolution 12 bit
	ADC1->SQR1 &= ~ADC_SQR1_L;//1 channel regular sequence length
	ADC1->SQR3 |= 0x00000001;//regular channel 1
	ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;//channel 1 sample 15 cycles

	//ADC1->CR2 |= ADC_CR2_EOCS;//EOC bit set at the end of each regular conversion
	//ADC2->CR2 |= ADC_CR2_ALIGN;//data align left
	//ADC1->CR1 |= ADC_CR1_EOCIE;//end of conversion interrupt enable

	/*
	ADC1->CR1 |= ADC_CR1_AWDCH_0;//AWD select for channel 1
	ADC1->HTR = 0xD00;//high threshold AWD
	ADC1->LTR = 0x200;//low threshold AWD
	ADC1->CR1 |= ADC_CR1_AWDEN;//analog watchdog enable
	ADC1->CR1 |= ADC_CR1_AWDIE;//analog watchdog interrupt enable
	//ADC1->CR1 |= ADC_CR1_AWDSGL;
	 */

	ADC1->CR2 |= ADC_CR2_ADON;//ADC on

	NVIC_SetPriority(ADC_IRQn, 4);
	NVIC_EnableIRQ(ADC_IRQn);//IRQ handler
}

void ADC1_DeInit(void)
{
  ADC1->CR2 &= ~ADC_CR2_ADON;//ADC off
  RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;//ADC1 clock disable
}

void ADC1_Temp_Init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;//ADC1 clock enable

  ADC1->CR1 &= ~ADC_CR1_RES;//ADC1 resolution 12 bit
  ADC1->SQR1 &= ~ADC_SQR1_L;//1 channel regular sequence length
  ADC1->SQR3 |= 0x00000007;//regular channel 7
  ADC1->SQR3 |= 0x00000016;//regular channel 16(temperature sensor)
  ADC1->SMPR2 |= ADC_SMPR2_SMP7_0;//channel 7 sample 15 cycles

  /*
  ADC1->CR1 |= ADC_CR1_AWDCH_0|ADC_CR1_AWDCH_1|ADC_CR1_AWDCH_2;//AWD select for channel 7
  ADC1->HTR = 0xD00;//high threshold AWD
  ADC1->LTR = 0x200;//low threshold AWD
  ADC1->CR1 |= ADC_CR1_AWDEN;//analog watchdog enable
  ADC1->CR1 |= ADC_CR1_AWDIE;//analog watchdog interrupt enable
  //ADC1->CR1 |= ADC_CR1_AWDSGL;
   */

  ADC1->CR2 |= ADC_CR2_ADON;//ADC on
  NVIC_SetPriority(ADC_IRQn, 4);
  NVIC_EnableIRQ(ADC_IRQn);//IRQ handler
}

uint16_t Read_ADC1(void)
{
	ADC1->CR2 |= ADC_CR2_SWSTART;//start conversion
	while(!(ADC1->SR & ADC_SR_EOC));//wait
	return ADC1->DR;
}

void ADC_IRQHandler(void) {
	if(ADC1->SR & ADC_SR_AWD)	{
	ADC1->SR &= ~ADC_SR_AWD;//clear AWD flag
	}
}

void Read_Temp(void) {
  uint16_t value;
  ADC1_Temp_Init();
  while(1) {
      PCF8812_Clear();
      PCF8812_Title("TEMP SENSOR");
      Button_Set_Name(user_button, "OK");
      value = Read_ADC1();
      //USB_Send_Int(value);
      PCF8812_SValue("temp", value, "C", 2);
      if(Button_Get(user_button))
          break;
      delay_ms(500);
    }
}

void Read_Voltage(void) {
  uint16_t value;
  ADC1_Init();
  while(1) {
      PCF8812_Clear();
      PCF8812_Putline("VOLTAGE  (PA1-IN)", 0);
      Button_Set_Name(user_button, "OK");
      value = Read_ADC1();
      value = value*U_3V/4095;
      USB_Send_Int(value);
      //Logview_Send(value);
      //LCD_Value("Vdc", value, "mV", 2);
      PCF8812_Float_Value("Vdc ", value/1000.0, "V", 2);
      if(Button_Get(user_button))
          break;
      delay_ms(500);
    }
}
