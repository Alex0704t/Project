/**
  ******************************************************************************
  * @file    usb_bsp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   This file is responsible to offer board support package and is 
  *          configurable by user.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS. 
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "../USB_lib/STM32_USB_OTG_Driver/inc/usb_bsp.h"

#include "../../inc/usbd_conf.h"
#include "main.h"

/**
* @brief  USB_OTG_BSP_Init
*         Initilizes BSP configurations
* @param  None
* @retval None
*/

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  /* Configure SOF VBUS ID DM DP Pins */
  //PA8, 9, 10, 11, 12 alternate functions
  GPIOA->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 \
               | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER11_1 \
               | GPIO_MODER_MODER12_1;
  //PA8, 9, 10, 11, 12 very high speed
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9 \
                 | GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR11 \
                 | GPIO_OSPEEDER_OSPEEDR12;
  //PA8, 9, 11, 12 push-pull, PA10 open drain
  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9 \
                | GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_12);
  GPIOA->OTYPER |= GPIO_OTYPER_OT_10;
  //PA8, 9, 11, 12 no pull-up & pull-down, PA10 pull-up
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9 \
                | GPIO_PUPDR_PUPDR11 | GPIO_PUPDR_PUPDR12);
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_0;
  //PA8, 9, 10, 11, 12 OTG FS/HS alternate functions
  GPIOA->AFR[1] |= 0x000AAAAA;
 
  RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
  /* enable the PWR clock */
  RCC->APB1RSTR |= RCC_APB1RSTR_PWRRST;
  RCC->APB1RSTR |= RCC_APB1ENR_PWREN;

#ifdef USB_OTG_FS_LOW_PWR_MGMT_SUPPORT
  /* Configure the Key button in EXTI mode */
  ButINT_Init();

  EXTI->PR = EXTI_PR_PR18;//clear EXTI18 pending bit
  //EXTI_ClearITPendingBit(EXTI_Line18);

  EXTI->IMR |= EXTI_IMR_MR18;//no mask interrupt EXTI0, 8, 9
  EXTI->RTSR |= EXTI_RTSR_TR18;//rising trigger event EXTI18

  EXTI->PR = EXTI_PR_PR18;//clear EXTI18 pending bit

  NVIC_SetPriorityGrouping(4);//4 field for priority group
  NVIC_SetPriority(OTG_FS_WKUP_IRQn, 4);
  NVIC_EnableIRQ(OTG_FS_WKUP_IRQn);//IRQ handler

  EXTI->PR = EXTI_PR_PR18;//clear EXTI18 pending bit
  EXTI->PR |= EXTI_PR_PR0;//clear EXTI0 pending bit
#else
  /* Configure the Key button in EXTI mode */
  Button_Init(user_button);
#endif

}
/**
* @brief  USB_OTG_BSP_EnableInterrupt
*         Enable USB Global interrupt
* @param  None
* @retval None
*/
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
  NVIC_SetPriority(OTG_FS_IRQn, 4);
  NVIC_EnableIRQ(OTG_FS_IRQn);//IRQ handler
}
/**
* @brief  USB_OTG_BSP_uDelay
*         This function provides delay time in micro sec
* @param  usec : Value of delay required in micro sec
* @retval None
*/
void USB_OTG_BSP_uDelay (const uint32_t usec)
{
  uint32_t count = 0;
  const uint32_t utime = (120 * usec / 7);
  do
  {
    if ( ++count > utime )
    {
      return ;
    }
  }
  while (1);
}


/**
* @brief  USB_OTG_BSP_mDelay
*          This function provides delay time in milli sec
* @param  msec : Value of delay required in milli sec
* @retval None
*/
void USB_OTG_BSP_mDelay (const uint32_t msec)
{
  USB_OTG_BSP_uDelay(msec * 1000);   
}
/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
