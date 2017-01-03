/*
 * tim.c
 *
 *  Created on: 20.07.2015
 *      Author: Aleksey
 */

#include "tim.h"

void Tim4_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;//Tim4 clock enabled 84 MHz
	TIM4->PSC = 500 - 1;//prescaller(1...65536)
	TIM4->ARR = 100;//auto-reload value
	TIM4->DIER |= TIM_DIER_UIE;//update interrupt enable
	TIM4->CR1 &= ~TIM_CR1_DIR;//count direction up
	TIM4->CR1 |= TIM_CR1_ARPE;//ARR preload enable
	TIM4->CR1 |= TIM_CR1_CEN;//TIM4 enabled

	NVIC_SetPriority(TIM4_IRQn, 4);
	NVIC_EnableIRQ(TIM4_IRQn);
}

void Tim4_OC_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;//GPIOD clock enable
	GPIOD->MODER &= ~0xFF000000;//Clear Pin 12,13,14,15 mode
	GPIOD->MODER |= 0xAA000000;//Pin 12,13,14,15 alternative function
	GPIOD->OSPEEDR &= ~0xFF000000;//Low speed PD12,13,14,15
	GPIOD->OTYPER &= ~0xF000;//Push-pull PD12,13,14,15
	GPIOD->PUPDR &= ~0xFF000000;//No pull-up & pull-down PD12,13,14,15
	GPIOD->AFR[1] |= 0x22220000;//AF2 - Tim4 channels
	Tim4_Init();
	TIM4->CCMR1 = 0x6060;//PWM1 mode channels 1,2
	TIM4->CCMR2 = 0x6060;//PWM1 mode channels 3,4
	TIM4->CCMR1 |= TIM_CCMR1_OC1PE|TIM_CCMR1_OC2PE;//Output compare preload enable
	TIM4->CCMR2 |= TIM_CCMR2_OC3PE|TIM_CCMR2_OC4PE;//Output compare preload enable
	//TIM4->CCER |= TIM_CCER_CC1P|TIM_CCER_CC2P|TIM_CCER_CC3P|TIM_CCER_CC4P;//Output polarity Low
	TIM4->CCER |= TIM_CCER_CC1E|TIM_CCER_CC2E|TIM_CCER_CC3E|TIM_CCER_CC4E;//Output enable
}

void Tim4_DeInit(void)
{
  NVIC_DisableIRQ(TIM4_IRQn);
  TIM4->CR1 &= ~TIM_CR1_CEN;//TIM4 disabled
  RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN;//Tim4 clock disabled
}

void TIM4_IRQHandler(void) {
	if(TIM4->SR & TIM_SR_UIF)	{
	TIM4->SR &= ~TIM_SR_UIF;//clear update interrupt flag
	}
}

void Tim3_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;//Tim3 clock enabled 84 MHz
	TIM3->PSC = 20000 - 1;//prescaller(1...65536)
	TIM3->ARR = 256;//auto-reload value
	TIM3->DIER |= TIM_DIER_UIE;//update interrupt enable
	TIM3->CR1 &= ~TIM_CR1_DIR;//count direction up
	TIM3->CR1 |= TIM_CR1_ARPE;//ARR preload enable
	TIM3->CR1 |= TIM_CR1_CEN;//clock enabled

	NVIC_SetPriority(TIM3_IRQn, 4);
	NVIC_EnableIRQ(TIM3_IRQn);
}

void Tim3_Enc_Init(void)
{
	Tim3_Init();
	TIM3->PSC = 0;//prescaller value for encoder mode
	TIM3->SMCR |= TIM_SMCR_SMS_1;//counting on TI1 edges only
	TIM3->CCER &= ~(TIM_CCER_CC1P|TIM_CCER_CC2P);//active high level TI1, TI2
}

void TIM3_IRQHandler(void) {
	if(TIM3->SR & TIM_SR_UIF)	{
	TIM3->SR &= ~TIM_SR_UIF;//clear update interrupt flag
	}
}

void Tim6_Init(uint16_t frequency)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;//Tim6 clock enabled 84 MHz
  TIM6->PSC = TIM6_PSC - 1;//prescaller(1...65536)
  TIM6->ARR = APB1_FREQ/(SINE_RES * TIM6_PSC * frequency);//auto-reload value
  TIM6->DIER |= TIM_DIER_UIE;//update interrupt enable
  TIM6->CR2 |= TIM_CR2_MMS_1;//update event is trigger output

  NVIC_SetPriority(TIM6_DAC_IRQn, 4);
  //NVIC_EnableIRQ(TIM6_DAC_IRQn);

  //TIM6->CR1 |= TIM_CR1_CEN;//TIM6 enabled
}

void Tim6_Start()
{
#ifndef USE_DAC1_DMA
  NVIC_EnableIRQ(TIM6_DAC_IRQn);
#endif
  TIM6->CR1 |= TIM_CR1_CEN;//TIM6 enabled
}

void Tim6_Stop()
{
#ifndef USE_DAC1_DMA
  NVIC_DisableIRQ(TIM6_DAC_IRQn);
#endif
  TIM6->CR1 &= ~TIM_CR1_CEN;//TIM6 disabled
}

void Tim6_Set(uint16_t frequency)
{
  TIM6->PSC = TIM6_PSC - 1;//prescaller(1...65536)
  TIM6->ARR = APB1_FREQ/(SINE_RES * TIM6_PSC * frequency);;//auto-reload value
}

void TIM6_DAC_IRQHandler(void)
{
  TIM6->SR = 0;
#ifndef USE_DAC1_DMA
  DAC1_Handler();
#endif
}

void Tim5_Init(uint8_t frequency)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;//Tim5 clock enabled 84 MHz

  TIM5->PSC = TIM5_PSC - 1;//prescaller(1...65536)
  TIM5->ARR = APB1_FREQ/(TIM5_PSC * frequency);//auto-reload value
  TIM5->DIER |= TIM_DIER_UIE;//update interrupt enable

  TIM5->CR1 &= ~TIM_CR1_DIR;//count direction up
  TIM5->CR1 |= TIM_CR1_ARPE;//ARR preload enable

  NVIC_SetPriority(TIM5_IRQn, 0);
  NVIC_EnableIRQ(TIM5_IRQn);

  TIM5->CR1 |= TIM_CR1_CEN;//TIM5 enabled
}

void TIM5_IRQHandler() {
  if(TIM5->SR & TIM_SR_UIF) {
    TIM5->SR &= ~TIM_SR_UIF;//clear update interrupt flag
    PCF8812_Handler();
    PCF8812_Count();
    Button_Handler();
    }
}
