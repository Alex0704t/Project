/*
 * user.c
 *
 *  Created on: 18.12.2016.
 *      Author: User
 */

#include "user.h"

void MCU_Init() {
  Clock_Init();
  //GPIO_Clock_En();
  Led_Init();
  Button_Init();
  Enc_Tim3_Init();
  Tim4_OC_Init();
  PCF8812_Init();
  RTC_Init();
  PCF8812_Test();
  USB_HID_Init();
  //UB_USB_CDC_Init();
  //SCB->CPACR |= 0x00F00000;//enable FPU
  //Audio_Init();
  //AnalogWave_Init(1000, 30);
}
