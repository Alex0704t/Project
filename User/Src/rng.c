/*
 * rng.c
 *
 *  Created on: 23 ���� 2015 �.
 *      Author: Aleksey
 */
#include "rng.h"


void RNG_Init(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;//RNG clock enable
	RNG->CR |= RNG_CR_RNGEN;//RNG enable
}


