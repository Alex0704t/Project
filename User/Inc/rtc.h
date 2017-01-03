/*
 * rtc.h
 *
 *  Created on: 23.08.2015
 *      Author: Aleksey Totkal
 */

#ifndef USER_RTC_H_
#define USER_RTC_H_

//#include "../Device/stm32f4xx.h"
#include "user.h"


//#define USE_LSE//use LSE as RTC clock
#define USE_LSI//use LSI as RTC clock

typedef struct rtc_time
{
  uint8_t year;
  uint8_t w_day;
  uint8_t month;
  uint8_t date;
  uint8_t pm;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
}rtc_time_s;

enum w_day
{monday = 1, tuesday, wednesday, thursday, friday, saturday, sunday};

void RTC_Init(void);
rtc_time_s Get_Time(void);
void Set_Time(rtc_time_s* in);
rtc_time_s Input_Time();
void Get_Time_String(void* in, uint8_t*, uint8_t);

#endif /* USER_RTC_H_ */
