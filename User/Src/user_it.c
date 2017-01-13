/**
*****************************************************************************
**
**  File        : user_it.c
**
**  Abstract    : Main Interrupt Service Routines.
**                This file provides template for all exceptions handler and
**                peripherals interrupt service routine.
**
**  Environment : Eclipse
**
*****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "user_it.h"

#include "../../USB_lib/STM32_USB_Device_Library/Class/hid_custom/inc/usbd_hid_core.h"
#include "../../USB_lib/STM32_USB_Device_Library/Core/inc/usbd_core.h"
#include "../../USB_lib/STM32_USB_OTG_Driver/inc/usb_core.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
//extern uint8_t  Buffer[];


#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif


/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)  // handler NMI calling if failed HSE.
   {
        if (RCC->CIR & RCC_CIR_CSSF)
          RCC->CIR|=RCC_CIR_CSSC;//clear CSSF flag
   }

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void Inc_Tick(void);
extern void Systick_Blink(void);

void SysTick_Handler(void) {
    Inc_Tick();
    Systick_Blink();
  }

/**
  * @brief  This function handles OTG_FS_WKUP_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS
void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI->PR = EXTI_PR_PR18;//clear EXTI18 pending bit
}
#endif

/**
  * @brief  This function handles OTG_HS_WKUP_IRQ Handler.
  * @param  None
  * @retval None
  */


/**
  * @brief  This function handles OTG_xx_IRQ Handler.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler(&USB_OTG_dev);
}



