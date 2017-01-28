/*
 * encoder.h
 *
 *  Created on: 16.07.2015
 *      Author: Aleksey
 */

#ifndef USER_ENCODER_H_
#define USER_ENCODER_H_

//#include "../Device/stm32f4xx.h"
#include "user.h"


#define ENC_A         ((GPIOA->IDR & GPIO_IDR_IDR_1)>>1)
#define ENC_B         ((GPIOA->IDR & GPIO_IDR_IDR_2)>>2)
#define ENC_MAX       3


void Enc_Init(void);
void Enc_Tim3_Init(void);
void Enc_State(void);
void Enc_Set_Zero();
int8_t Get_Enc_Count(uint8_t range);
int8_t Get_Enc_Diff();

#endif /* USER_ENCODER_H_ */
