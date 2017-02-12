#ifndef __LED_H
#define __LED_H

//#include "../Device/stm32f4xx.h"
#include "user.h"

#ifndef PWM_LED_CONTROL
#define PWM_LED_CONTROL
#endif

#ifdef PWM_LED_CONTROL

#define LED_ON(led)       do\
                          {RESET_BLINK(led);\
                          Led_Set(led, 100);\
                          }while (0)
#define LED_OFF(led)      do\
                          {RESET_BLINK(led);\
                          Led_Set(led, 0);\
                          }while (0)
#define LED_TOGGLE(led)   do\
                          {RESET_BLINK(led);\
                          Led_Toggle(led);\
                          }while (0)

#else

#define LED_ON(led)       do\
                          {if (led != led_all)\
                             GPIOD->BSRR = 0x1 << (11 + led);\
                          else\
                             GPIOD->BSRR = 0x0000F000;\
                          }while(0)

#define LED_OFF(led)      do\
                          {if (led != led_all)\
                             GPIOD->BSRR = 0x1 << (27 + led);\
                          else\
                             GPIOD->BSRR = 0xF0000000;\
                          }while(0)

#define LED_TOGGLE(led)   do\
                          {if (led != led_all)\
                             GPIOD->ODR ^= 0x1 << (11 + led);\
                          else\
                             GPIOD->ODR ^= 0x0000F000;\
                          }while(0)

#endif

#define RESET_BLINK(led)  Led_Blink(led, 0, 0);
#define BRIGHT_STEP       10
#define TIME_STEP         5

enum {led_all,
      green,
      orange,
      red,
      blue
     } led;



//led on macros

//GPIOD->BSRRL
/* 
#define LED_0_ON() 				  GPIOD->BSRRL = 0x1000
#define LED_1_ON() 				  GPIOD->BSRRL = 0x2000
#define LED_2_ON() 				  GPIOD->BSRRL = 0x4000
#define LED_3_ON() 				  GPIOD->BSRRL = 0x8000
#define LED_GREEN_ON() 			GPIOD->BSRRL = 0x1000
#define LED_ORANGE_ON() 		GPIOD->BSRRL = 0x2000
#define LED_RED_ON() 			  GPIOD->BSRRL = 0x4000
#define LED_BLUE_ON() 			GPIOD->BSRRL = 0x8000
#define LED_ALL_ON() 			  GPIOD->BSRRL = 0xF000
*/

#define LED_0_ON() 				  GPIOD->BSRR = 0x1000
#define LED_1_ON() 				  GPIOD->BSRR = 0x2000
#define LED_2_ON() 				  GPIOD->BSRR = 0x4000
#define LED_3_ON() 				  GPIOD->BSRR = 0x8000
#define LED_GREEN_ON() 			GPIOD->BSRR = 0x1000
#define LED_ORANGE_ON() 		GPIOD->BSRR = 0x2000
#define LED_RED_ON() 			  GPIOD->BSRR = 0x4000
#define LED_BLUE_ON() 			GPIOD->BSRR = 0x8000
#define LED_ALL_ON() 			  GPIOD->BSRR = 0xF000


//led off macros

//GPIOD->BSRRH
/* 
#define LED_0_OFF() 			  GPIOD->BSRRH = 0x1000
#define LED_1_OFF() 			  GPIOD->BSRRH = 0x2000
#define LED_2_OFF() 			  GPIOD->BSRRH = 0x4000
#define LED_3_OFF() 			  GPIOD->BSRRH = 0x8000
#define LED_GREEN_OFF() 		GPIOD->BSRRH = 0x1000
#define LED_ORANGE_OFF() 		GPIOD->BSRRH = 0x2000
#define LED_RED_OFF() 			GPIOD->BSRRH = 0x4000
#define LED_BLUE_OFF()			GPIOD->BSRRH = 0x8000
#define LED_ALL_OFF() 			GPIOD->BSRRH = 0xF000
*/

#define LED_0_OFF() 			  GPIOD->BSRR = 0x10000000
#define LED_1_OFF() 			  GPIOD->BSRR = 0x20000000
#define LED_2_OFF() 			  GPIOD->BSRR = 0x40000000
#define LED_3_OFF() 			  GPIOD->BSRR = 0x80000000
#define LED_GREEN_OFF() 		GPIOD->BSRR = 0x10000000
#define LED_ORANGE_OFF() 		GPIOD->BSRR = 0x20000000
#define LED_RED_OFF() 			GPIOD->BSRR = 0x40000000
#define LED_BLUE_OFF() 			GPIOD->BSRR = 0x80000000
#define LED_ALL_OFF() 			GPIOD->BSRR = 0xF0000000


//led toggle macros

#define LED_0_TOGGLE() 			GPIOD->ODR ^= 0x1000
#define LED_1_TOGGLE() 			GPIOD->ODR ^= 0x2000
#define LED_2_TOGGLE() 			GPIOD->ODR ^= 0x4000
#define LED_3_TOGGLE() 			GPIOD->ODR ^= 0x8000
#define LED_GREEN_TOGGLE() 	GPIOD->ODR ^= 0x1000
#define LED_ORANGE_TOGGLE() GPIOD->ODR ^= 0x2000
#define LED_RED_TOGGLE() 		GPIOD->ODR ^= 0x4000
#define LED_BLUE_TOGGLE() 	GPIOD->ODR ^= 0x8000
#define LED_ALL_TOGGLE() 		GPIOD->ODR ^= 0xF000

void Led_GPIO_Init(void);
void Led_Init();
void Led_Brightness(uint8_t led, char* name);
void Led_Set(uint8_t led, uint8_t value);
void Led_Toggle(uint8_t led);
void Led_Systick_Blink(void);
void Led_Blink(uint8_t led, uint16_t on_time, uint16_t off_time);
void Led_Set_Blink(uint8_t led, char* name);



#endif
