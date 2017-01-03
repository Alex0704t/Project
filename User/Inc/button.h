#ifndef __BUTTON_H
#define __BUTTON_H

//#include "../Device/stm32f4xx.h"
#include "user.h"

#define BUTT_DELAY()        delay_ms(35)
#define PRESS_USER_BUTTON   ((GPIOA->IDR & GPIO_IDR_IDR_0) == GPIO_IDR_IDR_0)
#define PRESS_BUTTON_1      ((GPIOC->IDR & GPIO_IDR_IDR_8) == GPIO_IDR_IDR_8)
#define PRESS_BUTTON_2      ((GPIOC->IDR & GPIO_IDR_IDR_9) == GPIO_IDR_IDR_9)


#define CONTACT_BOUNCE_LIMIT  2
#define SHORT_PRESS_LIMIT     25
#define PAST_HOLD_DELAY       -5



enum button
{
  user_button, button_1, button_2
};

enum button_state
{
 button_released, button_pressed, button_hold
};

//EXTI->PR |= EXTI_PR_PR0;//clear pending bit of set 1

void Button_Init(void);
void ButtINT_Init(void);

typedef void (*p_func)(void);

typedef struct button_s{
  uint8_t* name;
  uint8_t enable;
  uint32_t repeat_ms;
  volatile/*__IO*/ uint8_t state;
  volatile/*__IO*/ int16_t count;
  p_func press_act;
  p_func hold_act;
}button_s;



uint8_t Get_Button(uint8_t button);
uint8_t Check_Button(uint8_t button);
void Button_Handler();
void Button_Handle(uint8_t button);
void Exec_button(uint8_t button);
void Execute_buttons();

void Set_Button(uint8_t button, button_s *in);

#endif
