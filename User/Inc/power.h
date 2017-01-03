/*
 * power.h
 *
 *  Created on: 24.05.2015.
 *      Author: Aleksey
 */

#ifndef USER_POWER_H_
#define USER_POWER_H_

//#include "../Device/stm32f407xx.h"
#include "user.h"

//#define SCB_SCR_SLEEPONEXIT ((u8)0x02) /*!< Sleep on exit bit */
//#define SCB_SCR_SLEEPDEEP ((u8)0x04) /*!< Sleep deep bit */


void GoStandBy(void);
void GoStop(void);
void GoSleep(void);

#endif /* USER_POWER_H_ */
