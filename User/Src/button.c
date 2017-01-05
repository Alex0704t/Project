#include "button.h"

button_s butt[3] = {0};


void Button_Init(uint8_t button) {
  switch(button) {
    case user_button:
      RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;//GPIOA clock enable
      GPIOA->MODER &= ~GPIO_MODER_MODER0;//Input PA0
      GPIOA->OTYPER &= ~GPIO_OTYPER_IDR_0;//Push-pull
      GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0;//no pull-up & pull-down
      break;
    case button_1:
      RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;//GPIOC clock enable
      GPIOC->MODER &= ~GPIO_MODER_MODER8;//Input PC8
      GPIOC->OTYPER &= ~GPIO_OTYPER_IDR_8;//Push-pull
      GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR8;//no pull-up & pull-down
      break;
    case button_2:
      RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;//GPIOC clock enable
      GPIOC->MODER &= ~GPIO_MODER_MODER9;//Input PC9
      GPIOC->OTYPER &= ~GPIO_OTYPER_IDR_9;//Push-pull
      GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR9;//no pull-up & pull-down
      break;
    default:
      break;
  }
}

void ButtINT_Init(void) {
	//Button_Init();
	// set EXTI registers bit 8 has effect to both 8 and 9 lines
	// bit 9 has no effect
	EXTI->IMR |= EXTI_IMR_MR0|EXTI_IMR_MR8;//no mask interrupt EXTI0, 8
	EXTI->RTSR |= EXTI_RTSR_TR0|EXTI_RTSR_TR8;//rising trigger event EXTI0, 8, 9
  //EXTI->FTSR |= EXTI_FTSR_TR0|EXTI_FTSR_TR8;//falling trigger event EXTI0, 8, 9
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;//EXTI0 source PA0
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PC;//EXTI8, 9 source PC8, PC9
	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(EXTI0_IRQn, 4);
	NVIC_EnableIRQ(EXTI0_IRQn);//IRQ handler
	NVIC_SetPriority(EXTI9_5_IRQn, 4);
	NVIC_EnableIRQ(EXTI9_5_IRQn);//IRQ handler
}


uint8_t Get_Button(uint8_t button) {
  uint8_t temp = butt[button].state;
  butt[button].state = button_released;
  return temp;
}

void EXTI0_IRQHandler(void) {
	if((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0) {
      EXTI->PR |= EXTI_PR_PR0;//clear pending bit of set 1

    }
}

void EXTI9_5_IRQHandler(void) {
  if((EXTI->PR & EXTI_PR_PR8) == EXTI_PR_PR8) {
      EXTI->PR |= EXTI_PR_PR8;//clear pending bit of set 1

    }
  }


void Button_Handler() {
  if(butt[user_button].enable)
      Button_Handle(user_button);
  if(butt[button_1].enable)
      Button_Handle(button_1);
  if(butt[button_2].enable)
      Button_Handle(button_2);
}

void Button_Handle(uint8_t button) {
  //if button pressing
      if(Check_Button(button)) {
          PCF8812_On();
  //increment counter
          butt[button].count++;
  //button hold
          if(butt[button].state != button_hold \
          && butt[button].count >= SHORT_PRESS_LIMIT) {
              butt[button].state = button_hold;
              return;
  //no repeat mode
              if(!butt[button].repeat_ms) {
                  return;
              }
          }
      }
  //button released
      else {
  //increment counter if set past hold delay
          if(butt[button].count < 0)
              butt[button].count++;
  //reset button state & set delay past holding button
          if(butt[button].state == button_hold) {
              butt[button].state = button_released;
              butt[button].count = PAST_HOLD_DELAY;
          }
  //filter contact bounce
          else if(butt[button].state != button_hold){
              if(butt[button].count <= CONTACT_BOUNCE_LIMIT) {
                  butt[button].count = 0;
                  butt[button].state = button_released;
              }
  //button pressed
              else if(butt[button].count > CONTACT_BOUNCE_LIMIT \
              && butt[button].count < SHORT_PRESS_LIMIT) {
                  butt[button].state = button_pressed;
                  butt[button].count = PAST_HOLD_DELAY;
                  //butt[button].count = 0;
                  return;
              }
          }
      }
}

void Exec_button(uint8_t button) {
//execute short press action
   if(butt[button].state == button_pressed) {
       butt[button].state = button_released;
       butt[button].press_act();
       PCF8812_Butt_ind(button);
       }
//execute long press action
   else if(butt[button].state == button_hold) {
       PCF8812_Butt_ind(button);
//once execute
       if(!butt[button].repeat_ms) {
           butt[button].hold_act();
       }
//repeat executing
       else {
           static uint32_t start = 0;
           if(!start)
             start = Get_Tick();
           if((Get_Tick() - start) >= butt[button].repeat_ms) {
               butt[button].hold_act();
               start = Get_Tick();
           }
       }
    }
}

void Execute_buttons() {
  if(butt[user_button].enable) {
      Exec_button(user_button);
  }
  if(butt[button_1].enable) {
      Exec_button(button_1);
  }
  if(butt[button_2].enable) {
      Exec_button(button_2);
  }
}

void Set_Button(uint8_t button, button_s *in) {

  Enable_button(button);
  if(!in->hold_act) {
     in->hold_act = in->press_act;
     in->repeat_ms = 500;
  }
  butt[button] = *in;
  //PCF8812_Button(butt[0].name, butt[1].name, butt[2].name);
}

void Enable_button(uint8_t button) {
  butt[button] = (button_s){};
  Button_Init(button);
  butt[button].enable = SET;
}

uint8_t Check_Button(uint8_t button) {
  return (button == user_button)  ? PRESS_USER_BUTTON :
         (button == button_1)     ? PRESS_BUTTON_1    :
         (button == button_2)     ? PRESS_BUTTON_2    : 0;
}

