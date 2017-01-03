/*
 * menu.h
 *
 *  Created on: 04.09.2015
 *      Author: Aleksey Totkal
 */

#ifndef USER_MENU_H_
#define USER_MENU_H_


#include "user.h"

#define MAX_OPT         6
#define OPT_LEN         16
#define BUTT_NUM        3
#define USB_SEND_PERIOD 1000

#define BREAK_OUT       if(brk_flag) {\
                        brk_flag = 0; \
                        active_menu->DeInit(); \
                        break;}

typedef void (*p_func)(void);

typedef struct menu {
  uint8_t* name;//menu name
  uint8_t num;//options num
  uint8_t *option[MAX_OPT];//options names
  struct menu* next_menu[MAX_OPT];//next menu pointers
  p_func action[MAX_OPT];//option functions pointers
  p_func Init;//initialisation function pointer
  p_func DeInit;//deinitialisation function pointer
  uint8_t init_flag;//initialisation flag
  struct button_s* butt[BUTT_NUM];
}menu_s;//menu structure



void Enter_menu(menu_s *menu);
void Back_menu();
void No_op();
void Go_menu();
void Sel_item();
void Next_item();
void Prev_item();
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
void Beep_Start();
void Audio_Init();
/*********************************************************************/

#endif /* USER_MENU_H_ */
