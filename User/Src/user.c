/*
 * user.c
 *
 *  Created on: 18.12.2016.
 *      Author: User
 */

#include "user.h"

void MCU_Init() {
  Clock_Init();
//  RTC_Init();

  Led_Init();
  Enc_Tim3_Init();

  PCF8812_Init();
  PCF8812_Test();
  USB_HID_Init();
  //SCB->CPACR |= 0x00F00000;//enable FPU
  //Audio_Init();
  //AnalogWave_Init(1000, 30);
}


