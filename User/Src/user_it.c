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
extern void Led_Systick_Blink(void);

void SysTick_Handler(void) {
    Inc_Tick();
    Led_Systick_Blink();
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
void OTG_FS_IRQHandler(void) {
  USBD_OTG_ISR_Handler(&USB_OTG_dev);
}



void EXTI1_IRQHandler(void)
{
  if(EXTI->PR & EXTI_PR_PR1)
    {
      EXTI->PR |= EXTI_PR_PR1;//clear pending bit of set 1
//      Led_Blink(blue, 500, 250);

//      GetAxData();
//      Dir_Led();
    }
}

void ADC_IRQHandler(void) {
  if(ADC1->SR & ADC_SR_AWD) {
  ADC1->SR &= ~ADC_SR_AWD;//clear AWD flag
  }
}

void EXTI0_IRQHandler(void) {
  if((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0) {
      EXTI->PR |= EXTI_PR_PR0;//clear pending bit of set 1

    }
}

void EXTI9_5_IRQHandler(void) {
  if((EXTI->PR & EXTI_PR_PR8) == EXTI_PR_PR8) {
      EXTI->PR |= EXTI_PR_PR8;//clear pending bit of set 1
    }
  }

void DMA1_Stream7_IRQHandler(void)
{
#ifdef USE_MP3
  EVAL_AUDIO_TransferComplete_CallBack();
#else
  Beep_Handler();
#endif
}

void SPI1_IRQHandler(void)
{
  if(SPI1->SR & SPI_SR_TXE)
  {

  }
}

void DMA2_Stream3_IRQHandler(void) {
  if(DMA2->LISR & DMA_LISR_TCIF3) {//check transfer complete
    DMA2->LIFCR |= DMA_LIFCR_CTCIF3;//clear interrupt flag
    DMA2_Stream3->CR &= ~DMA_SxCR_EN;//stream disable
    DMA2_Stream3->CR &= ~DMA_SxCR_TCIE;//transfer complete interrupt disable
  }
}


void DMA2_Stream0_IRQHandler(void) {
  if(DMA2->LISR & DMA_LISR_TCIF0) {//check transfer complete
    DMA2->LIFCR |= DMA_LIFCR_CTCIF0;//clear interrupt flag
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;//stream disable
    DMA2_Stream0->CR &= ~DMA_SxCR_TCIE;//transfer complete interrupt disable
//    MovDetectEN();
//    LIS3DSH_View();
    LIS3DSH_CS_OFF();
    LIS3DSH_SetFlag();
  }
}

void SPI2_IRQHandler(void) {
  if (SPI2->SR & SPI_SR_TXE) {

  }
}

void DMA1_Stream4_IRQHandler(void)
{
  if(DMA1->HISR & DMA_HISR_TCIF4)//check transfer complete
  {
    DMA1->HIFCR |= DMA_HIFCR_CTCIF4;//clear interrupt flag
    //DMA1_Stream4->CR &= ~DMA_SxCR_TCIE;//transfer complete interrupt disable
    while(!(SPI2->SR & SPI_SR_TXE));
    while((SPI2->SR & SPI_SR_BSY));
    DMA1_Stream4->CR &= ~DMA_SxCR_EN;//stream disable
    PCF8812_buff_state = PCF8812_FLUSHED;
  }
}

void TIM4_IRQHandler(void) {
  if(TIM4->SR & TIM_SR_UIF) {
  TIM4->SR &= ~TIM_SR_UIF;//clear update interrupt flag
  }
}

void TIM3_IRQHandler(void) {
  if(TIM3->SR & TIM_SR_UIF) {
  TIM3->SR &= ~TIM_SR_UIF;//clear update interrupt flag
  }
}

void TIM5_IRQHandler() {
  if(TIM5->SR & TIM_SR_UIF) {
    TIM5->SR &= ~TIM_SR_UIF;//clear update interrupt flag
    Button_Handler();
    PCF8812_Handler();
    PCF8812_Count();
    }
}

void TIM6_DAC_IRQHandler(void)
{
  TIM6->SR = 0;
#ifndef USE_DAC1_DMA
  DAC1_Handler();
#endif
}

extern uint8_t *tx_uart1_ptr;
extern uint8_t *rx_uart1_ptr;
extern uint8_t *tx_uart2_ptr;
extern uint8_t *rx_uart2_ptr;

extern uint8_t tx_uart1_count;
extern uint8_t rx_uart1_count;
extern uint8_t tx_uart2_count;
extern uint8_t rx_uart2_count;

void USART1_IRQHandler(void)
{
  if(USART1->SR & USART_SR_TXE)
  {
  USART1->DR = *tx_uart1_ptr++;
  if(--tx_uart1_count == 0)
    USART1->CR1 &= ~USART_CR1_TXEIE;
  }

  if(USART1->SR & USART_SR_RXNE)
  {
  *rx_uart1_ptr++ = USART1->DR;
  if(--rx_uart1_count == 0)
    USART1->CR1 &= ~USART_CR1_RXNEIE;
  }
}

void USART2_IRQHandler(void)
{
  if(USART2->SR & USART_SR_TXE)
  {
  USART2->DR = *tx_uart2_ptr++;
  if(--tx_uart2_count == 0)
    USART2->CR1 &= ~USART_CR1_TXEIE;
  }

  if(USART2->SR & USART_SR_RXNE)
  {
  *rx_uart2_ptr++ = USART2->DR;
  if(--rx_uart2_count == 0)
    USART2->CR1 &= ~USART_CR1_RXNEIE;
  }
 }

void DMA2_Stream7_IRQHandler(void)
{
  if(DMA2->HISR & DMA_HISR_TCIF7)//transfer complete
  {
    DMA2->HIFCR |= DMA_HIFCR_CTCIF7;//clear interrupt flag
    DMA2_Stream7->CR &= ~DMA_SxCR_EN;//stream disable
  }
}

void DMA1_Stream6_IRQHandler(void)
{
  if(DMA1->HISR & DMA_HISR_TCIF6)//transfer complete
  {
    DMA1->HIFCR |= DMA_HIFCR_CTCIF6;//clear interrupt flag
    DMA1_Stream6->CR &= ~DMA_SxCR_EN;//stream disable
  }
}

void DMA2_Stream2_IRQHandler(void)
{
  if(DMA2->LISR & DMA_LISR_TCIF2)//transfer complete
  {
    DMA2->LIFCR |= DMA_LIFCR_CTCIF2;//clear interrupt flag
    DMA2_Stream2->CR &= ~DMA_SxCR_EN;//stream disable
  }
}

void DMA1_Stream5_IRQHandler(void)
{
  if(DMA1->HISR & DMA_HISR_TCIF5)//transfer complete
  {
    DMA1->HIFCR |= DMA_HIFCR_CTCIF5;//clear interrupt flag
    DMA1_Stream5->CR &= ~DMA_SxCR_EN;//stream disable
  }
}
