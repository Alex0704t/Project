/*
 * rng.h
 *
 *  Created on: 23.05.2015
 *      Author: Aleksey
 */

#ifndef USER_RNG_H_
#define USER_RNG_H_

//#include "../Device/stm32f4xx.h"
#include "user.h"

#define RAND RNG->DR

void RNG_Init(void);

#endif /* USER_RNG_H_ */
