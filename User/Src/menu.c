/*
 * menu.c
 *
 *  Created on: 27.08.2015
 *      Author: Aleksey Totkal
 */

#include "menu.h"

/*
 * *********************************************************************
 * Common variables
 * *********************************************************************
 */
static int8_t cur_pos = 0;
uint8_t brk_flag = 0;
uint8_t nxt_flag = 0;
menu_s *active_menu;
/*
 * *********************************************************************
 * Button variables
 * *********************************************************************
 */
button_s no_button  = {};
button_s sel_item   = {.enable = SET, .name = "OK",   .press_act = Sel_item};
button_s back       = {.enable = SET, .name = "BACK", .press_act = Back_menu};
button_s next       = {.enable = SET, .name = "NEXT", .press_act = Next_item};
button_s prev       = {.enable = SET, .name = "PREV", .press_act = Prev_item};


/*********************************************************************/
menu_s led0_menu = {
    "LED0 (GREEN)",
    4,
    {"ON/OFF",    "BIGHTNESS", "BLINK",    "BACK"},
    {NULL,        NULL,        NULL,       NULL},
    {LED0_Toggle, LED0_Bright, LED0_Blink, Back_menu},
    No_op,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};

menu_s led1_menu = {
    "LED1 (ORANGE)",
    4,
    {"ON/OFF",    "BIGHTNESS", "BLINK",    "BACK"},
    {NULL,        NULL,        NULL,       NULL},
    {LED1_Toggle, LED1_Bright, LED1_Blink, Back_menu},
    No_op,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};

menu_s led2_menu = {
    "LED2 (RED)",
    4,
    {"ON/OFF",    "BIGHTNESS", "BLINK",    "BACK"},
    {NULL,        NULL,        NULL,       NULL},
    {LED2_Toggle, LED2_Bright, LED2_Blink, Back_menu},
    No_op,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};

menu_s led3_menu = {
    "LED3 (BLUE)",
    4,
    {"ON/OFF",    "BIGHTNESS", "BLINK",    "BACK"},
    {NULL,        NULL,        NULL,       NULL},
    {LED3_Toggle, LED3_Bright, LED3_Blink, Back_menu},
    No_op,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};

menu_s led_all_menu = {
    "LED ALL",
    4,
    {"ON/OFF",       "BIGHTNESS",    "BLINK",       "BACK"},
    {NULL,           NULL,           NULL,          NULL},
    {LED_all_Toggle, LED_all_Bright, LED_all_Blink, Back_menu},
    No_op,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};

/*********************************************************************/
menu_s led_menu = {
    "LED",
    6,
    {"GREEN  (0)", "ORANGE (1)", "RED    (2)", "BLUE   (3)", "ALL",         "BACK"},
    {&led0_menu,   &led1_menu,   &led2_menu,   &led3_menu,   &led_all_menu, NULL},
    {Go_menu,      Go_menu,      Go_menu,      Go_menu,      Go_menu,       Back_menu},
    Tim4_OC_Init,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};
/*********************************************************************/
menu_s usb_menu = {
    "USB",
    3,
    {"ECHO",   "COUNT",    "BACK"},
    {NULL,     NULL,       NULL},
    {USB_Echo, Send_Count, Back_menu},
    No_op,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};
/*********************************************************************/
menu_s adc_menu = {
    "ADC",
    3,
    {"TEMPERATURE", "VOLTAGE",    "BACK"},
    {NULL,          NULL,         NULL},
    {Read_Temp,     Read_Voltage, Back_menu},
    No_op,//UB_USB_CDC_Init()
    ADC1_DeInit,
    DISABLE,
    {&sel_item, &back, &next}
};
/*********************************************************************/

menu_s beep_menu = {
    "BEEP",
    4,
    {"START",     "SET",    "STOP",    "BACK"},
    {NULL,        NULL,     NULL,      NULL},
    {Beep_Start,  SetBeep,  StopBeep,  Back_menu},
    No_op,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};

menu_s wave_menu = {
    "SINE WAVE",
    4,
    {"START",          "SET",        "STOP",          "BACK"},
    {NULL,             NULL,         NULL,            NULL},
    {AnalogWave_Start, Sine_Set,     AnalogWave_Stop, Back_menu},
    Sine_Init,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};

menu_s audio_menu = {
    "AUDIO",
    5,
    {"BEEP",     "SINE",     "CHIP ID",        "VOLUME",  "BACK"},
    {&beep_menu, &wave_menu, NULL,             NULL,      NULL},
    {Go_menu,    Go_menu,    Get_Audiochip_ID, SetVolume, Back_menu},
    Audio_Init,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};

menu_s setting_menu = {
    "SETTINGS",
    5,
    {"DATE & TIME",         "NO_OP",     "NO_OP",        "NO_OP",  "BACK"},
    {NULL,                  NULL,        NULL,           NULL,     NULL},
    {PCF8812_Input_Time,    No_op,       No_op,          No_op,    Back_menu},
    No_op,
    No_op,
    DISABLE,
    {&sel_item, &back, &next}
};

/*********************************************************************/
menu_s main_menu = {
    "MAIN",
    5,
    {"LED",      "USB",      "ADC",      "AUDIO",      "SETTINGS"},
    {&led_menu,  &usb_menu,  &adc_menu,  &audio_menu,  &setting_menu},
    {Go_menu,    Go_menu,    Go_menu,    Go_menu,      Go_menu},
    No_op,
    No_op,
    DISABLE,
    {&sel_item, &prev, &next}
};

/*
 * *********************************************************************
 * Menu function
 * *********************************************************************
 */
void Main_menu() {
  Enter_menu(&main_menu);
}

void Back_menu() {
  //Enter_menu(active_menu->prev_menu);
  brk_flag = 1;
  //RESET_ENC;
  active_menu->DeInit();
}

void Sel_item() {
  active_menu->action[cur_pos]();//run menu item function
}

void No_op() {
}

void Go_menu() {
  Enter_menu(active_menu->next_menu[cur_pos]);//enter next menu
}

void Next_item() {
  INCR_ENC(1);
}

void Prev_item() {
  DECR_ENC(1);
}

void Enter_menu(menu_s *menu) {
  RESET_ENC;//reset encoder counter
  while(1) {
  if(active_menu != menu) {
      active_menu = menu;//set current menu
  //set buttons parameters
  Button_Set(user_button, active_menu->butt[user_button]);
  Button_Set(button_1, active_menu->butt[button_1]);
  Button_Set(button_2, active_menu->butt[button_2]);
  if(active_menu->init_flag == DISABLE) {//if no initialised
      active_menu->Init();//execute initialisation
      active_menu->init_flag = ENABLE;//set flag
    }
  }
  PCF8812_Clear();//clear display
  if(strcmp(active_menu->name, "MAIN") == 0)//if main menu
    PCF8812_Time(view_all, 0);//view time in top of display
  else
    PCF8812_Title(active_menu->name);//else view menu name
//  PCF8812_Button(active_menu->butt[0]->name, active_menu->butt[1]->name, \
//             active_menu->butt[2]->name);//view buttons names
  for(uint8_t i = 0; i < active_menu->num; i++)
    PCF8812_Option(active_menu->option[i], i + 1);//view menu items
  cur_pos = Get_Enc_Count(active_menu->num - 1);//get cursor position number
  PCF8812_Cursor(cur_pos + 1);//view cursor
  Buttons_Executor();
  PCF8812_DELAY;
  BREAK_OUT;//exit loop if set break flag
  }
}
/*
 * *********************************************************************
 * LED functions
 * *********************************************************************
 */
void LED0_Toggle(void)
{
  Led_Toggle(green);
}

void LED1_Toggle(void)
{
  Led_Toggle(orange);
}

void LED2_Toggle(void)
{
  Led_Toggle(red);
}

void LED3_Toggle(void)
{
  Led_Toggle(blue);
}

void LED_all_Toggle(void)
{
  Led_Toggle(led_all);
}
/*********************************************************************/
void LED0_Bright(void)
{
  Led_Toggle(green);
  Led_Brightness(green, "LED0 (GREEN)");
}

void LED1_Bright(void)
{
  Led_Brightness(orange, "LED1 (ORANGE)");
}

void LED2_Bright(void)
{
  Led_Brightness(red, "LED2 (RED)");
}

void LED3_Bright(void)
{
  Led_Brightness(blue, "LED3 (BLUE)");
}

void LED_all_Bright(void)
{
  Led_Brightness(led_all, "LED ALL");
}
/*********************************************************************/
void LED0_Blink(void)
{
  Set_Blink(green, "LED0 (GREEN)");
}

void LED1_Blink(void)
{
  Set_Blink(orange, "LED1 (ORANGE)");
}

void LED2_Blink(void)
{
  Set_Blink(red, "LED2 (RED)");
}

void LED3_Blink(void)
{
  Set_Blink(blue, "LED3 (BLUE)");
}

void LED_all_Blink(void)
{
  Set_Blink(led_all, "LED ALL");
}
/*
 * *********************************************************************
 * USB functions
 * *********************************************************************
 */
void Send_Count(void)
{
  USB_Count(USB_SEND_PERIOD);
}

/*
 * *********************************************************************
 * ADC functions
 * *********************************************************************
 */


/*
 * *********************************************************************
 * Audio Menu function
 * *********************************************************************
 */

void Sine_Init()
{
  AnalogWave_Init(1000, 30);
}

void Sine_Set()
{
  uint32_t freq = PCF8812_Input_Int("Frequency", 20, 20000);
  AnalogWave_Stop();
  AnalogWave_Init(freq, 30);
  AnalogWave_Start();
}

void Audio_Init()
{
  EVAL_AUDIO_Init(70);
}

void Beep_Start()
{
  StartBeep(freq1000, 70, on_1s2, off_1s23);
}
