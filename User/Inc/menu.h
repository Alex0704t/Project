/*
 * menu.h
 *
 *  Created on: 04.09.2015
 *      Author: Aleksey Totkal
 */

#ifndef USER_MENU_H_
#define USER_MENU_H_


#include "user.h"
#include "pcf8812.h"
#include "cs43l22.h"

#define MAX_OPT         6
#define OPT_LEN         16
#define BUTT_NUM        3
#define USB_SEND_PERIOD 1000
#define MENU_NAME_SIZE  17//PCF8812_STR_SIZ

//#define BREAK_OUT       if(brk_flag) {\
                        brk_flag = 0; \
                        active_menu->DeInit(); \
                        break;}

typedef void (*p_func)(void);

typedef struct menu {
  char name[MENU_NAME_SIZE];//menu name
  uint8_t num;//options num
  char* option[MAX_OPT];//options names
  struct menu* next_menu[MAX_OPT];//next menu pointers
  p_func action[MAX_OPT];//option functions pointers
  struct menu* prev_menu;//previous menu pointer
  p_func Init;//initialisation function pointer
  p_func DeInit;//deinitialisation function pointer
  uint8_t init_flag;//initialisation flag
  struct button_s* butt[BUTT_NUM];
}menu_s;//menu structure

/*********************************************************************/
menu_s main_menu;
/*********************************************************************/
menu_s led_menu;
menu_s led0_menu;
menu_s led1_menu;
menu_s led2_menu;
menu_s led3_menu;
menu_s led_all_menu;
/*********************************************************************/
menu_s usb_menu;
/*********************************************************************/
menu_s adc_menu;
/*********************************************************************/
menu_s audio_menu;
menu_s beep_menu;
menu_s wave_menu;
/*********************************************************************/
menu_s setting_menu;
/*********************************************************************/



void Enter_menu(menu_s *menu);
void Back_menu();
inline void No_op();
inline void Go_menu();
void Sel_item();
inline void Next_item();
inline void Prev_item();
void Main_menu();
/*********************************************************************/
void LED0_Toggle(void);
void LED1_Toggle(void);
void LED2_Toggle(void);
void LED3_Toggle(void);
void LED_all_Toggle(void);
void LED0_Bright(void);
void LED1_Bright(void);
void LED2_Bright(void);
void LED3_Bright(void);
void LED_all_Bright(void);
void LED0_Blink(void);
void LED1_Blink(void);
void LED2_Blink(void);
void LED3_Blink(void);
void LED_all_Blink(void);
/*********************************************************************/
void Send_Count(void);
/*********************************************************************/
void Sine_Init();
void Sine_Set();
void Beep_Run();
void Audio_Init();
/*********************************************************************/

#endif /* USER_MENU_H_ */
