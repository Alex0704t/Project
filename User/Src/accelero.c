/*
 * accelero.c
 *
 *  Created on: 05.06.2015
 *      Author: Aleksey
 */

#include "accelero.h"

extern __IO uint8_t AxData[7];
axis_s lis3dsh_axis;

void LIS3DSH_CS_Init(void) {
/*PE3 chip select for accelerometer*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER |= GPIO_MODER_MODER3_0;//PE3 output
	GPIOE->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR3;//low speed
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT_3;//push-pull
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR3;//no pull-up & pull-down
}

void LIS3DSH_Int2_Init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;//GPIOA clock enable
	GPIOE->MODER &= ~GPIO_MODER_MODER1;//Input PE1
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR1;//no pull-up & pull-down
	EXTI->IMR |= EXTI_IMR_MR1;//no mask interrupt EXTI1
	EXTI->RTSR |= EXTI_RTSR_TR1;//rising trigger event EXTI1
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PE;//EXTI1 source PE1
	WriteRegSPI1(CTRL_REG3, CTRL_REG3_INT2_EN);//INT2 enable
	NVIC_SetPriorityGrouping(4);//4 field for priority group
	NVIC_SetPriority(EXTI1_IRQn, 4);
	NVIC_EnableIRQ(EXTI1_IRQn);//IRQ handler
}

void LIS3DSH_Init(void) {
	SPI1_Init();
//	SPI1_DMA_Init();
	LIS3DSH_CS_Init();
	LIS3DSH_CS_OFF();
}

void LIS3DSH_En(void) {
	if (CheckRegSPI1(WHO_I_AM, I_AM_LIS3DSH)) {
	  LED_ON(green);
	  LED_ON(red);
    delay_ms(300);
    LED_OFF(green);
    LED_OFF(red);
	}
	WriteRegSPI1(CTRL_REG4, CTRL_REG4_ODR_100|CTRL_REG4_XYZ);
	WriteRegSPI1(CTRL_REG5, 0x00);//scale 2G
	if (CheckRegSPI1(CTRL_REG4, CTRL_REG4_ODR_100|CTRL_REG4_XYZ)) {
    LED_ON(orange);
    LED_ON(blue);
    delay_ms(300);
    LED_OFF(orange);
    LED_OFF(blue);
	}
}

axis_s LIS3DSH_Decode(uint8_t *data, uint8_t axis_num) {
  axis_s res = {};
  res.x = (int16_t) (((uint16_t)data[1] << 8) | (uint16_t)data[0]);
  res.y = (int16_t) (((uint16_t)data[3] << 8) | (uint16_t)data[2]);
  if (axis_num > 2)
    res.z = (int16_t) (((uint16_t)data[5] << 8) | (uint16_t)data[4]);
  else
    res.z = 0;
  return res;
}

void Dir_Led(void) {
	//uint8_t temp;
	int16_t XData = 0;
	int16_t YData = 0;
	int16_t ZData = 0;
//	GetAxData();
//	ReadSPI1(OUT_DATA, AxData, 7);
	XData = (int16_t) ((AxData[2] << 8) |	AxData[1]);
	YData = (int16_t) ((AxData[4] << 8) |	AxData[3]);
	ZData = (int16_t) ((AxData[6] << 8) |	AxData[5]);
	if (XData > 800) {
    LED_ON(green);
    LED_OFF(red);
	}
	else if (XData < -800) {
    LED_OFF(green);
    LED_ON(red);
	}
	else {
    LED_OFF(green);
    LED_OFF(red);
	}
	if (YData > 800) {
    LED_OFF(orange);
    LED_ON(blue);
	}
	else if (YData < -800) {
    LED_ON(orange);
    LED_OFF(blue);
	}
	else {
    LED_OFF(orange);
    LED_OFF(blue);
	}
}

void MovDetectEN(void) {
	LIS3DSH_Int2_Init();//enable EXTI1 interrupt
	WriteRegSPI1(CTRL_REG1, CTRL_REG1_SM1_EN|CTRL_REG1_SM1_PIN);//SM1 enable, interrupt INT2 pin
	WriteRegSPI1(CTRL_REG3, CTRL_REG3_INT2_EN|CTRL_REG3_IEA);//INT2 enable, active HIGH
	WriteRegSPI1(CTRL_REG4, CTRL_REG4_XYZ|CTRL_REG4_ODR_100);//100 Hz 3-axes enable
	WriteRegSPI1(CTRL_REG5, 0x00);//scale 2G
	WriteRegSPI1(THRS1_1, 0x55);//threshold value
	WriteRegSPI1(ST1_1, GNTH1);//axes grater then threshold 1
	WriteRegSPI1(ST2_1, CONT);//Continues execution from RESET-	POINT
	WriteRegSPI1(MASK1_B, MASK_XY);//enable x, y
	WriteRegSPI1(MASK1_A, MASK_XY);//enable x, y
	WriteRegSPI1(SETT1, SETT_SITR);//enable CONT command
}


void LIS3DSH_View() {
  uint8_t data[6];
  axis_s temp = {};
  Button_Set_Name(user_button, "EXIT");

  while(1) {
      PCF8812_Clear();
      PCF8812_Title("LIS3DSH");
#if 1
      GetAxData();
      temp = LIS3DSH_Decode(AxData[1], 3);
#else
      ReadSPI1(OUT_DATA, data, 6);
      temp = LIS3DSH_Decode(data, 3);
#endif
      PCF8812_SValue("X ", temp.x, "", 2);
      PCF8812_SValue("Y ", temp.y, "", 3);
      PCF8812_SValue("Z ", temp.z, "", 4);

      if (temp.x > 800) {
        LED_ON(green);
        LED_OFF(red);
      }
      else if (temp.x < -800) {
        LED_OFF(green);
        LED_ON(red);
      }
      else {
        LED_OFF(green);
        LED_OFF(red);
      }
      if (temp.y > 800) {
        LED_OFF(orange);
        LED_ON(blue);
      }
      else if (temp.y < -800) {
        LED_ON(orange);
        LED_OFF(blue);
      }
      else {
        LED_OFF(orange);
        LED_OFF(blue);
      }

      delay_ms(330);
      if(Button_Get(user_button))
        return;
      PCF8812_DELAY;
    }
}
