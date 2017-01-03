#ifndef __SYSTICK_H
#define __SYSTICK_H

//#include "../Device/stm32f4xx.h"
//#include "../Device/system_stm32f4xx.h"
#include "user.h"

#define LOOP_MS(x) for(uint32_t tick = GetTick(); GetTick() - tick < (x);)

void Clock_Init(void);
void Inc_Tick(void);
uint32_t Get_Tick(void);
void delay_ms(uint32_t delay);
uint8_t Check_delay_ms(uint32_t delay);
void GPIO_Clock_En(void);

#if GFX_USE_OS_RAW32
systemticks_t gfxSystemTicks(void);
systemticks_t gfxMillisecondsToTicks(delaytime_t ms);
#endif

#endif//__SYSTICK_H
