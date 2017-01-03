/*
 * adc.h
 *
 *  Created on: 21.07.2015
 *      Author: Aleksey
 */

#ifndef USER_ADC_H_
#define USER_ADC_H_

//#include "../Device/stm32f407xx.h"
#include "user.h"

#define U_3V 2940

void ADC1_Init(void);
void ADC1_Temp_Init(void);
void ADC1_DeInit(void);
uint16_t Read_ADC1(void);
void Read_Temp(void);
void Read_Voltage(void);

#endif /* USER_ADC_H_ */
