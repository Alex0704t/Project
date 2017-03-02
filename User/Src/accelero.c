/*
 * accelero.c
 *
 *  Created on: 05.06.2015
 *      Author: Aleksey
 */

#include "accelero.h"

//axis_s lis3dsh_axis;
__IO uint8_t SPI1_data_ready = 0;
extern uint8_t  SPI1_DMA_mode_flag;

void LIS3DSH_CS_Init(void) {
/*PE3 chip select for accelerometer*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER |= GPIO_MODER_MODER3_0;//PE3 output
	GPIOE->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR3;//low speed
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT_3;//push-pull
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR3;//no pull-up & pull-down
}

inline void LIS3DSH_CS_DeInit(void) {
  GPIOE->MODER &= ~(GPIO_MODER_MODER3_0|GPIO_MODER_MODER3_1);//PE3 reset state
}


void LIS3DSH_Int2_Init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;//GPIOA clock enable
	GPIOE->MODER &= ~GPIO_MODER_MODER1;//Input PE1
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR1;//no pull-up & pull-down
	EXTI->IMR |= EXTI_IMR_MR1;//no mask interrupt EXTI1
	EXTI->RTSR |= EXTI_RTSR_TR1;//rising trigger event EXTI1
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PE;//EXTI1 source PE1
	LIS3DSH_WriteReg(CTRL_REG3, CTRL_REG3_INT2_EN);//INT2 enable
	SPI1_DMA_mode_flag |= SPI1_DMA_LIS3DSH|SPI1_DMA_LIS3DSH_INT2;//
	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(EXTI1_IRQn, 4);
	NVIC_EnableIRQ(EXTI1_IRQn);//IRQ handler
}

void LIS3DSH_Init(void) {
//	SPI1_Init();
	SPI1_DMA_Init();
	LIS3DSH_CS_Init();
	LIS3DSH_CS_OFF();
}

void LIS3DSH_DeInit(void) {
  LIS3DSH_ClearReg(CTRL_REG4);//power down LIS3DSH
  if ((SPI1_DMA_mode_flag & SPI1_DMA_LIS3DSH) == SPI1_DMA_LIS3DSH) {
    SPI1_DMA_mode_flag = 0;//clear DMA mode flag
//  SPI1_DeInit();
    SPI1_DMA_DeInit();
  }
  LIS3DSH_CS_DeInit();
}

void LIS3DSH_En(lis3dsh_scale scale, lis3dsh_data_rate rate) {
	if (LIS3DSH_CheckReg(WHO_I_AM, I_AM_LIS3DSH)) {
	  LED_ON(green);
	  LED_ON(red);
	  PCF8812_String("I'm LIS3DSH", 1);
    delay_ms(50);
    LED_OFF(green);
    LED_OFF(red);
	}
	LIS3DSH_ModReg(CTRL_REG4, CTRL_REG4_ODR_MASK, CTRL_REG4_AXIS|rate);//output data rate
	LIS3DSH_ModReg(CTRL_REG5, CTRL_REG5_FSCALE_MASK, scale);//set scale
	if (LIS3DSH_CheckReg(CTRL_REG4, rate|CTRL_REG4_AXIS)) {
    LED_ON(orange);
    LED_ON(blue);
    PCF8812_String("enable", 1);
    delay_ms(50);
    LED_OFF(orange);
    LED_OFF(blue);
	}

}

axis_s LIS3DSH_Decode(uint8_t *data) {
  axis_s res = {};
  res.x = (int16_t) (((uint16_t)data[1] << 8) | (uint16_t)data[0]);
  res.y = (int16_t) (((uint16_t)data[3] << 8) | (uint16_t)data[2]);
#ifndef USE_2AXIS
  res.z = (int16_t) (((uint16_t)data[5] << 8) | (uint16_t)data[4]);
#else
  res.z = 0;
#endif
  return res;
}

/** ToDo fixed incorrect axe Z value in 16G mode */
real_axis_s LIS3DSH_Convert(axis_s *data, lis3dsh_scale scale) {
  real_axis_s res = {};
  float f_scale = (scale == SCALE_2G)  ? (2.0  / INT16_MAX) :
                  (scale == SCALE_4G)  ? (4.0  / INT16_MAX) :
                  (scale == SCALE_6G)  ? (6.0  / INT16_MAX) :
                  (scale == SCALE_8G)  ? (8.0  / INT16_MAX) :
                  (scale == SCALE_16G) ? (16.0 / INT16_MAX) : 0;
  res.x = data->x * f_scale;
  res.y = data->y * f_scale;
  res.z = data->z * f_scale;
  return res;
}

void LIS3DSH_Led(const axis_s *in, int32_t threshold) {
	if (in->x > threshold) {
    LED_ON(green);
    LED_OFF(red);
	}
	else if (in->x < -threshold) {
    LED_OFF(green);
    LED_ON(red);
	}
	else {
    LED_OFF(green);
    LED_OFF(red);
	}
	if (in->y > threshold) {
    LED_OFF(orange);
    LED_ON(blue);
	}
	else if (in->y < -threshold) {
    LED_ON(orange);
    LED_OFF(blue);
	}
	else {
    LED_OFF(orange);
    LED_OFF(blue);
	}
}

void LIS3DSH_MovDetEn(lis3dsh_scale scale, lis3dsh_data_rate rate) {
	LIS3DSH_WriteReg(CTRL_REG1, CTRL_REG1_SM1_EN|CTRL_REG1_SM1_PIN);//SM1 enable, interrupt INT2 pin
	LIS3DSH_WriteReg(CTRL_REG3, CTRL_REG3_INT2_EN|CTRL_REG3_IEA);//INT2 enable, active HIGH
	LIS3DSH_ModReg(CTRL_REG4, CTRL_REG4_ODR_MASK, CTRL_REG4_AXIS|rate);//output data rate
	LIS3DSH_ModReg(CTRL_REG5, CTRL_REG5_FSCALE_MASK, scale);//set scale
	LIS3DSH_WriteReg(THRS1_1, 0x55);//threshold value
	LIS3DSH_WriteReg(ST1_1, GNTH1);//axes grater then threshold 1
	LIS3DSH_WriteReg(ST2_1, CONT);//Continues execution from RESET-	POINT
	LIS3DSH_WriteReg(MASK1_B, MASK_XY);//enable x, y
	LIS3DSH_WriteReg(MASK1_A, MASK_XY);//enable x, y
	LIS3DSH_WriteReg(SETT1, SETT_SITR);//enable CONT command
	LIS3DSH_Int2_Init();//enable EXTI1 interrupt
}

void LIS3DSH_View() {
  uint8_t data[6];
  axis_s temp = {};
  real_axis_s res = {};
  int32_t arr[3] = {};
  Button_Set_Name(user_button, "EXIT");
  while(1) {
    PCF8812_Clear();
    PCF8812_Title("LIS3DSH");
    if (Check_delay_ms(330)) {
#if 0
        LIS3DSH_GetAxis();
        SPI1_WaitFlag();
        LIS3DSH_GetData(data);
#else
        LIS3DSH_Read(OUT_DATA, data, 6);
#endif
        temp = LIS3DSH_Decode(data);
        res = LIS3DSH_Convert(&temp, SCALE_2G);
        arr[0] = temp.x, arr[1] = temp.y, arr[2] = temp.z;
        USB_Send_IntArr(arr, 3);
      }
     PCF8812_FValue("X ", res.x, " G", 1);
     PCF8812_FValue("Y ", res.y, " G", 2);
     PCF8812_FValue("Z ", res.z, " G", 3);
      uint16_t threshold = 500;
      if (temp.x > threshold) {
        PCF8812_Putchar(blank, 5, 9);
        PCF8812_Putchar(arrow_left, 5, 7);
      }
      else if (temp.x < -threshold) {
        PCF8812_Putchar(arrow_right, 5, 9);
        PCF8812_Putchar(blank, 5, 7);
      }
      else {
        PCF8812_Putchar(blank, 5, 9);
        PCF8812_Putchar(blank, 5, 7);
      }
      if (temp.y > threshold) {
        PCF8812_Putchar(arrow_down, 6, 8);
        PCF8812_Putchar(blank, 4, 8);
      }
      else if (temp.y < -threshold) {
        PCF8812_Putchar(blank, 6, 8);
        PCF8812_Putchar(arrow_up, 4, 8);
      }
      else {
        PCF8812_Putchar(blank, 6, 8);
        PCF8812_Putchar(blank, 4, 8);
      }
      if(Button_Get(user_button))
        return;
      PCF8812_DELAY;
    }
}

/**
  * @brief  This function handles data received via DMA past LIS3DSH interrupt
  * @param  None
  * @retval None
  */
void LIS3DSH_IntHandler() {
  uint8_t data[6];
  axis_s temp = {};
  LIS3DSH_GetData(data);
  temp = LIS3DSH_Decode(data);
  LIS3DSH_Led(&temp, 600);
  LIS3DSH_MovDetEn(SCALE_2G, ODR_100Hz);
}
