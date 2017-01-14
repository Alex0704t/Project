/*
 * dac.c
 *
 *  Created on: 08.03.2016
 *      Author: Aleksey Totkal
 */

#include "dac.h"

static uint8_t i = 0;

void DAC1_Init()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;//GPIOA clock enable
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;//DAC clock enable
  // PA4 - DAC1 output
  GPIOA->MODER |= (GPIO_MODER_MODER4_1|GPIO_MODER_MODER4_0);//Analog mode
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4;//Push-pull
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4;//No pull-up & pull-down
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4_1;//High speed
  // DAC register set

  DAC->CR |= DAC_CR_TEN1;//channel1 Trigger enable
#ifdef DAC1_TIM6_EVENT
  DAC->CR &= ~DAC_CR_TSEL1;//
#else
  DAC->CR |= DAC_CR_TSEL1;//software trigger selected
#endif

  DAC->CR |= DAC_CR_MAMP1;//max amplitude

  DAC->CR &= ~DAC_CR_WAVE1;//clear wave bits
#if DAC1_MODE == DAC_TRIANGLE
  DAC->CR |= DAC_CR_WAVE1_1;//triangle wave
#elif DAC1_MODE == DAC_NOISE
  DAC->CR |= DAC_CR_WAVE1_0;//white noise
#else

#endif

  //DAC->CR |= DAC_CR_EN1;//DAC channel1 enable
}

void DAC1_DMA_Init(void)
{
  DAC1_Init();
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;//DMA1 enable
  /** DAC1 DMA1 Stream_5 channel_7 */
  DMA1_Stream5->CR = 0;//clear register
  DMA1_Stream5->CR |= DMA_SxCR_CHSEL;//channel_7 select
  DMA1_Stream5->CR |= DMA_SxCR_PL_1;//high priority
  DMA1_Stream5->CR |= DMA_SxCR_MSIZE_0|DMA_SxCR_PSIZE_0;//memory & peripheral data size half-word
  DMA1_Stream5->CR |= DMA_SxCR_MINC;//memory increment mode
  DMA1_Stream5->CR |= DMA_SxCR_DIR_0;//memory to peripheral direction
  DMA1_Stream5->CR |= DMA_SxCR_CIRC;//circular mode
  DMA1_Stream5->PAR |= (uint32_t)&(DAC->DHR12R1);//peripheral address
  DMA1_Stream5->M0AR |= (uint32_t)sin_table;//memory address
  DMA1_Stream5->NDTR |= SINE_RES;//data size

  //DMA1_Stream5->CR |= DMA_SxCR_EN;//stream enable
  DAC->CR |= DAC_CR_DMAEN1;//DAC channel1 DMA enable

}

void DAC1_Handler()
{
  DAC->DHR12R1 = sin_table[i++];
  if(i == SINE_RES)
    i = 0;
}
