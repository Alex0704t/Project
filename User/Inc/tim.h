/*
 * tim.h
 *
 *  Created on: 20.07.2015
 *      Author: Aleksey
 */

#ifndef USER_TIM_H_
#define USER_TIM_H_

//#include "../Device/stm32f4xx.h"
#include "user.h"

#define APB1_FREQ          84000000//TIM counter clock (prescaled APB1)
#define APB2_FREQ          168000000//TIM counter clock (prescaled APB2)


#define SET_ENC(value)    (TIM3->CNT = value)//set enc counter value
#define INCR_ENC(value)   (TIM3->CNT += 2*value)//increase enc counter
#define DECR_ENC(value)   (TIM3->CNT -= 2*value)//decrease enc counter

#define T4_CH1_SET(value) (TIM4->CCR1 = value)
#define T4_CH2_SET(value) (TIM4->CCR2 = value)
#define T4_CH3_SET(value) (TIM4->CCR3 = value)
#define T4_CH4_SET(value) (TIM4->CCR4 = value)
#define T4_ALL_SET(value) (TIM4->CCR1 = TIM4->CCR2 = TIM4->CCR3 = TIM4->CCR4 = value)

#define TIM6_PSC          2
#define TIM5_PSC          42000

void Tim3_Init(void);
void Tim3_Enc_Init(void);
void Tim4_Init(void);
void Tim4_OC_Init(void);
void Tim4_DeInit(void);
void Tim6_Init(uint16_t frequency);
void Tim6_Start();
void Tim6_Stop();
void Tim6_Set(uint16_t frequency);
void Tim5_Init(uint8_t frequency);

#endif /* USER_TIM_H_ */
