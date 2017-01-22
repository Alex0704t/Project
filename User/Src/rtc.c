/*
 * rtc.c
 *
 *  Created on: 23.08.2015
 *      Author: Aleksey Totkal
 */

#include "rtc.h"

void RTC_Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;//PWR clock enable
  PWR->CR |= PWR_CR_DBP;//Disable Backup Domain write protection
  RCC->BDCR |= RCC_BDCR_BDRST;//Reset  Backup Domain
  RCC->BDCR &= ~RCC_BDCR_BDRST;
  RCC->CSR |= RCC_CSR_LSION;//Enable LSI
  while(!(RCC->CSR & RCC_CSR_LSIRDY));//wait for LSI ready
  RCC->BDCR |= RCC_BDCR_RTCSEL_1;//LSI Backup Domain clock source
  RCC->BDCR |= RCC_BDCR_RTCEN;//RTC clock enable
  RTC->WPR |= 0xCA;//KEY1 to write protection register
  RTC->WPR |= 0x53;//KEY2 to write protection register
  RTC->ISR |= RTC_ISR_INIT;//enter initialization mode
  while(!(RTC->ISR & RTC_ISR_INITF));//waiting for initialization flag
  //ck_apre frequency = RTCCLK frequency/(PREDIV_A+1)
  RTC->PRER |= 0x007F0000;//LSE 32768 Hz / (127 + 1) = 256 Hz
                          //LSI 32000 Hz / (127 + 1) = 250 Hz
  //ck_spre frequency = ck_apre frequency/(PREDIV_S+1)
#ifdef USE_LSE
  RTC->PRER |= 0x000000FF;//LSE 256 Hz / (255 + 1) = 1 Hz
#else
  RTC->PRER |= 0x000000F9;//LSI 250 Hz / (249 + 1) = 1 Hz
#endif//USE_LSE
//  RTC->TR = 0x0;//set hour, minute, second
//  RTC->DR = 0x00154825;//set year, month, day, weak day(25.08.15 tue)
  RTC->CR &= ~RTC_CR_FMT;//24 hour format
  RTC->ISR &= ~RTC_ISR_INIT;//exit initialization mode
  RTC->WPR |= 0xFF;//enable write protection
}

rtc_time_s Get_Time(void)
{
  //return RTC time structure
  rtc_time_s out = {0};
  out.pm    = ((RTC->TR >> 22) & 0x1);
  out.hour  = ((RTC->TR >> 20) & 0x3) * 10 + ((RTC->TR >> 16) & 0xF);
  out.min   = ((RTC->TR >> 12) & 0x7) * 10 + ((RTC->TR >> 8)  & 0xF);
  out.sec   = ((RTC->TR >> 4)  & 0x7) * 10 + (RTC->TR & 0xF);
  out.year  = ((RTC->DR >> 20) & 0xF) * 10 + ((RTC->DR >> 16) & 0xF);
  out.month = ((RTC->DR >> 12) & 0x1) * 10 + ((RTC->DR >> 8)  & 0xF);
  out.date  = ((RTC->DR >> 4)  & 0x3) * 10 + (RTC->DR & 0xF);
  out.w_day = ((RTC->DR >> 13) & 0x3);
  return out;
}


void Set_Time(rtc_time_s* in)
{
  uint32_t temp = 0;
  temp |= (in->pm       << 22);
  temp |= (in->hour / 10  << 20)|(in->hour % 10  << 16);
  temp |= (in->min / 10   << 12)|(in->min % 10   << 8);
  temp |= (in->sec / 10   << 4) |(in->sec % 10);
  RTC->WPR |= 0xCA;//KEY1 to write protection register
  RTC->WPR |= 0x53;//KEY2 to write protection register
  RTC->ISR |= RTC_ISR_INIT;//enter initialization mode
  while(!(RTC->ISR & RTC_ISR_INITF));//waiting for initialization flag
  RTC->TR = temp;//write time
  temp = 0;
  temp |= (in->year / 10  << 20)|(in->year % 10  << 16);
  temp |= (in->w_day    << 13);
  temp |= (in->month / 10 << 12)|(in->month % 10 << 8);
  temp |= (in->date / 10  << 4) |(in->date % 10);
  RTC->DR = temp;//write date
  RTC->ISR &= ~RTC_ISR_INIT;//exit initialization mode
  RTC->WPR |= 0xFF;//enable write protection
}


void Get_Time_String(void* in, char* s, uint8_t view_mode){
  rtc_time_s temp;
  if(!in)//no passing time structure pointer
    temp = Get_Time();
  else
    temp = *(rtc_time_s*)in;//cast to time structure
  if(view_mode == view_time)
    snprintf(s, PCF8812_STR_SIZ, "%.2d:%.2d:%.2d", temp.hour, temp.min, temp.sec);
  else if(view_mode == view_date)
    snprintf(s, PCF8812_STR_SIZ, "%.2d.%.2d.%.2d", temp.date, temp.month, temp.year);
  else
    snprintf(s, PCF8812_STR_SIZ, "%.2d.%.2d.%.2d %.2d:%.2d:%.2d", \
             temp.date, temp.month, temp.year, temp.hour, temp.min, temp.sec);
}
