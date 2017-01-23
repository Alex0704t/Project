#include "led.h"

uint16_t led_time[4][2] = {0};
uint16_t led_count[4] = {0};
uint8_t led_flag[4] = {0};

void Led_Init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;//GPIOD clock enable
	GPIOD->MODER &= ~0xFF000000;//Clear Pin 12,13,14,15 mode
	GPIOD->MODER |= 0x55000000;//PD12,13,14,15 output
	GPIOD->OSPEEDR &= ~0xFF000000;//Low speed PD12,13,14,15
	GPIOD->OTYPER &= ~0xF000;//Push-pull PD12,13,14,15
	GPIOD->PUPDR &= ~0xFF000000;//No pull-up & pull-down PD12,13,14,15
}

void Led_Set(uint8_t led, uint8_t value)
{
  switch(led)
  {
    case led_all:
    T4_ALL_SET(value);
    break;
    case green:
    T4_CH1_SET(value);
    break;
    case orange:
    T4_CH2_SET(value);
    break;
    case red:
    T4_CH3_SET(value);
    break;
    case blue:
    T4_CH4_SET(value);
    break;
  }
}

void Led_Brightness(uint8_t led, char* name) {
  uint8_t bright = 0;
  RESET_BLINK(led);
  RESET_ENC;
  Button_Set_Name(user_button, "OK");
  Button_Set_Name(button_1, "-10%");
  Button_Set_Name(button_2, "+10%");
  while(1) {
  PCF8812_Clear();
  PCF8812_Title(name);
  PCF8812_Percent("Brightness", bright, 2);
  Led_Set(led, bright);
  bright = Get_Enc_Count(100);
  if(Button_Get(button_1))
    DECR_ENC(BRIGHT_STEP);
  if(Button_Get(button_2))
    INCR_ENC(BRIGHT_STEP);
  if(Button_Get(user_button))
    return;
  PCF8812_DELAY;
  }
}

void Led_Toggle(uint8_t led)
{
  static uint8_t flag[5] = {0};
  uint8_t i, m, n;
  //RESET_BLINK(led);
  if(led == led_all)
    {
      m = green;
      n = blue;
    }
  else
    m = n = led;
  for(i = m; i <= n; i++)
    {
    if(flag[i] == 1)
      {
        flag[i] = 0;
        LED_OFF(i);
      }
    else
      {
        flag[i] = 1;
        LED_ON(i);
      }
    }
}

void Systick_Blink(void)
{
  for(uint8_t i = 0; i <= 3; i++)
    {
      if(led_time[i][0])
      {
        if(led_count[i])
          led_count[i]--;
        else
          {
          if(led_flag[i] == 0)
            {
              led_flag[i] = 1;
              led_count[i] = led_time[i][0];
              Led_Set(i + 1, 100);
            }
          else
            {
              led_flag[i] = 0;
              led_count[i] = led_time[i][1];
              Led_Set(i + 1, 0);
            }
          }
      }
    }
}

void Led_Blink(uint8_t led, uint16_t on_time, uint16_t off_time)
{
  uint8_t i, m, n;
    if(led == led_all)
      {
        m = green;
        n = blue;
      }
    else
      m = n = led;
    for(i = m; i <= n; i++)
      {
        led_flag[i - 1] = 0;
        led_count[i - 1] = 0;
        led_time[i - 1][0] = on_time;
        led_time[i - 1][1] = off_time;
      }
}

void Set_Blink(uint8_t led, char* name)
{
  uint16_t on_time = 0, off_time = 0;
  RESET_ENC;
  Button_Set_Name(user_button, "OK");
  Button_Set_Name(button_1, "-0.5s");
  Button_Set_Name(button_2, "+0.5s");
  INCR_ENC(TIME_STEP);
  while(1)
    {
    PCF8812_Clear();
    PCF8812_Title(name);
    PCF8812_Float_Value("PULSE ", on_time / 10.0, "s", 2);
    on_time = Get_Enc_Count(100);
    if(Button_Get(button_1))
      DECR_ENC(TIME_STEP);
    if(Button_Get(button_2))
      INCR_ENC(TIME_STEP);
    if(Button_Get(user_button))
      break;
    PCF8812_DELAY;
    }
  RESET_ENC;
  INCR_ENC(TIME_STEP);
  while(1)
    {
    PCF8812_Clear();
    PCF8812_Title(name);
    PCF8812_Float_Value("PAUSE ", off_time / 10.0, "s", 2);
    off_time = Get_Enc_Count(100);
    if(Button_Get(button_1))
      DECR_ENC(TIME_STEP);
    if(Button_Get(button_2))
      INCR_ENC(TIME_STEP);
    if(Button_Get(user_button))
      break;
    PCF8812_DELAY;
    }
  Led_Blink(led, 100 * on_time, 100 * off_time);
}
