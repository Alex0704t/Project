/*
 * power.c
 *
 *  Created on: 24 èþíÿ 2015 ã.
 *      Author: Aleksey
 */

#include "power.h"

void GoStandBy(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_CWUF;//Clear Wakeup flag
	PWR->CSR |= PWR_CSR_EWUP;//Wakeup pin enable
	PWR->CR |= PWR_CR_PDDS;//Select STANDBY mode
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;//Set SLEEPDEEP bit of Cortex System Control Register
	__WFI();//entering stanby
}

void GoStop(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR &= ~(PWR_CR_LPDS|PWR_CR_PDDS);//clear LPDS, PDDS bits
	PWR->CR |= PWR_CR_LPDS;//set low power mode in stop
	//SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;//clear SLEEPONEXIT bit of Cortex System Control Register
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;//Set SLEEPDEEP bit of Cortex System Control Register
	//SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;//enter sleep on exit handler
	__WFI();//entering stop
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;//clear SLEEPDEEP bit of Cortex System Control Register
}

void GoSleep(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	//SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;//Set SLEEPDEEP bit of Cortex System Control Register
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;//clear SLEEPDEEP bit of Cortex System Control Register
	__WFI();//entering sleep
}
