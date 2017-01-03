/*
 * encoder.c
 *
 *  Created on: 16.07.2015
 *      Author: Aleksey
 */

#include "encoder.h"

uint8_t Enc_A_count = 0;
uint8_t Enc_A_state = 0;
uint8_t Enc_B_count = 0;
uint8_t Enc_B_state = 0;
uint8_t Enc_count = 0;
uint8_t Enc_state = 0;
uint8_t ab = 0;

void Enc_Init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;//GPIOA clock enable
	GPIOA->MODER &= ~(GPIO_MODER_MODER1|GPIO_MODER_MODER2);//Input PA1, PA2
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_1|GPIO_OTYPER_OT_2);//Push-pull
	//GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1|GPIO_PUPDR_PUPDR2);//no pull-up & pull-down
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0|GPIO_PUPDR_PUPDR2_0;//pull-up
	//GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR1|GPIO_OSPEEDER_OSPEEDR2;//High speed
}

void Enc_Tim3_Init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;//GPIOB clock enable
	GPIOB->MODER |= GPIO_MODER_MODER4_1|GPIO_MODER_MODER5_1;//AF PB4, PB5
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_4|GPIO_OTYPER_OT_5);//Push-pull
	//GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR4|GPIO_PUPDR_PUPDR5);//no pull-up & pull-down
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR4_0|GPIO_PUPDR_PUPDR5_0;//pull-up
	GPIOB->AFR[0] |= 0x00220000;//AF2(TIM3 CH1, CH2) PB4, PB5
	//GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4|GPIO_OSPEEDER_OSPEEDR5;//High speed
	Tim3_Enc_Init();
}

void Enc_State(void) {
	if(ENC_A == 1) {
		if(Enc_A_count < ENC_MAX)	{
			Enc_A_count++;
		}
		else {
			if(Enc_A_state == 0) {
				if(Enc_state == 0) {
					Enc_state = 1;
					Enc_count++;
				}
				else {
					Enc_state = 0;
					Enc_count--;
				}
			}
			Enc_A_state = 1;
		}
	}
	else {
		if(Enc_A_count > 0)	{
			Enc_A_count--;
		}
		else {
			if(Enc_A_state == 1) {
				if(Enc_state == 0) {
					Enc_state = 1;
					Enc_count++;
				}
				else {
					Enc_state = 0;
					Enc_count--;
				}
			}
			Enc_A_state = 0;
		}
	}
	if(ENC_B == 1) {
		if(Enc_B_count < ENC_MAX)	{
			Enc_B_count++;
		}
		else {
			if(Enc_B_state == 0) {
				if(Enc_state == 0) {
					Enc_state = 1;
					Enc_count--;
				}
				else {
					Enc_state = 0;
					Enc_count++;
				}
			}
			Enc_B_state = 1;
		}
	}
	else	{
		if(Enc_B_count > 0)	{
			Enc_B_count--;
		}
		else {
			if(Enc_B_state == 1) {
				if(Enc_state == 0) {
					Enc_state = 1;
					Enc_count--;
				}
				else {
					Enc_state = 0;
					Enc_count++;
				}
			}
			Enc_B_state = 0;
		}
	}
}

int8_t Get_Enc_Count(uint8_t range) {
  static int8_t prev = 0;
  int8_t ret = 0;
  ret = TIM3->CNT / 2;
  if(ret > range)
    RESET_ENC;
  if(ret < 0) {
    TIM3->CNT = 2 * range;
    ret = range;
    }
  if(prev != ret)
    PCF8812_On();//display on
  prev = ret;
	return ret;
}

int8_t Get_Enc_Diff() {
  int8_t temp = (int8_t)TIM3->CNT;//first read encoder counter
  delay_ms(40);//delay between two reading
  return (int8_t)TIM3->CNT - temp;//calculate difference encoder counter value*/
}
