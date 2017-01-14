
/*
 * NewProj
 * user.h
 *
 *  Created on: 23.10.2016
 *      Author: Aleksey Totkal
 */

#ifndef USER_INC_USER_H_
#define USER_INC_USER_H_

#include "main.h"
#include "stm32f4xx.h"

#include "led.h"
#include "button.h"
#include "systick.h"
#include "tim.h"
#include "adc.h"
#include "rng.h"
#include "flash.h"
#include "power.h"
#include "uart.h"
#include "spi.h"
#include "accelero.h"
#include "encoder.h"
#include "pcf8812.h"
#include "rtc.h"
#include "menu.h"
#include "cs43l22.h"
#include "i2c.h"
#include "dac.h"
#include "user_it.h"

#ifndef SWAP
#define SWAP(type, a, b) {type temp; temp = a; a = b; b = temp;}
#endif

#ifndef ABS(x)
#define ABS(x)         (x < 0) ? (-x) : x
#endif

#ifndef MAX(a, b)
#define MAX(a, b)       (a < b) ? (b) : a
#endif

#ifndef USE_FULL_ASSERT
#define USE_FULL_ASSERT
#endif

void MCU_Init();

/*
 * Pin using table
 ****************************************************
 * PA0      User button(discovery)
 * PA1      ADC input channel 1
 * PA2
 * PA3
 * PA4      Audio LRCK
 * PA5      Accelerometer SPI1_SCK
 * PA6      Accelerometer SPI1_MISO
 * PA7      Accelerometer SPI1_MOSI
 * PA8
 * PA9      USB VBUS_FS
 * PA10     USB OTG_ID
 * PA11     USB OTG_FS_DM
 * PA12     USB OTG_FS_PM
 * PA13     ST-LINK TCK/SWCLK
 * PA14     ST-LINK TMS/SWDIO
 * PA15
 * ****************************************************
 * PB0
 * PB1
 * PB2
 * PB3      ST-LINK T_SWO
 * PB4      Encoder TIM3_CH1 (User)
 * PB5      Encoder TIM3_CH2 (User)
 * PB6      Audio SDA
 * PB7
 * PB8
 * PB9      Audio SCL
 * PB10     Audio CLK_IN(Microphone)
 * PB11
 * PB12
 * PB13     LCD SPI2_SCK (User)
 * PB14     LCD Light (User)
 * PB15     LCD SPI2_MOSI (User)
 * ****************************************************
 * PC0      USB OTG_FS_PowerSwitchOn
 * PC1
 * PC2
 * PC3      Audio PDM_OUT(Microphone)
 * PC4      Audio (not fitted)
 * PC5
 * PC6
 * PC7      Audio MCLK
 * PC8      Button 1 (User)
 * PC9      Button 2 (User)
 * PC10     Audio SCLK
 * PC11
 * PC12     Audio SDIN
 * PC13
 * PC14
 * PC15
 * ****************************************************
 * PD0
 * PD1
 * PD2
 * PD3
 * PD4      Audio_RST
 * PD5      USB OTG_FS_OverCurrent
 * PD6
 * PD7
 * PD8      LCD Vdd (User)
 * PD9      LCD D/C (User)
 * PD10     LCD SCE (User)
 * PD11     LCD RES (User)
 * PD12     LED4
 * PD13     LED3
 * PD14     LED5
 * PD15     LED6
 * ****************************************************
 * PE0      Accelerometer MEMS_INT1
 * PE1      Accelerometer MEMS_INT2
 * PE2
 * PE3      Accelerometer CS_I2C/SPI
 * PE4
 * PE5
 * PE6
 * PE7
 * PE8
 * PE9
 * PE10
 * PE11
 * PE12
 * PE13
 * PE14
 * PE15
 */

//--------------------------------------------------------------

#endif /* USER_INC_USER_H_ */
