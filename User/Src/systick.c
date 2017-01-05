#include "systick.h"

__IO uint32_t Tick;

void Clock_Init(void) {
  RCC->CR |= 	RCC_CR_HSEON;//HSE on
  while(!(RCC->CR & RCC_CR_HSERDY));//HSE wait
  RCC->CFGR &=	~RCC_CFGR_SW;//clear SW bits
  RCC->CFGR |=  RCC_CFGR_SW_HSE;//switch to HSE
  RCC->CR |= RCC_CR_CSSON;//CSS on
  RCC->PLLCFGR = 0;
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;//PLL source is HSE
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_3;//PLLM=8
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_8|RCC_PLLCFGR_PLLN_6|RCC_PLLCFGR_PLLN_4;//PLLN=336
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ_0|RCC_PLLCFGR_PLLQ_1|RCC_PLLCFGR_PLLQ_2;//PLLQ=7(for RNG, USB OTG, SDIO)
  RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;//PLLP=2
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;//AHB prescaler = 1
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;//APB1 prescaler = 4
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;//APB2 prescaler = 2
  FLASH->ACR |= FLASH_ACR_PRFTEN;//prefetch enable
  FLASH->ACR &= ~FLASH_ACR_LATENCY;//clear
  FLASH->ACR |= FLASH_ACR_LATENCY_5WS;//set flash latency
  RCC->CR |= 	RCC_CR_PLLON;//PLL on
  while(!(RCC->CR & RCC_CR_PLLRDY));//PLL wait
  RCC->CFGR &= ~RCC_CFGR_SW;//clear SW bits
  RCC->CFGR |= RCC_CFGR_SW_PLL;//switch to PLL
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);//wait PLL switch status
  //SystemCoreClockUpdate();//get SystemCoreClock value
  //SysTick_Config(SystemCoreClock/1000);//systick interrupt each 1 ms
  SysTick_Config(168000 - 1);//systick interrupt each 1 ms

  NVIC_SetPriorityGrouping(4);//4 field for priority group
  }



void Inc_Tick(void) {
  Tick++;
}

uint32_t Get_Tick(void)
{
  return Tick;
}

void delay_ms(uint32_t delay)
{
  uint32_t tickstart = 0;
  tickstart = Get_Tick();
  while((Get_Tick() - tickstart) < delay);
}

uint8_t Check_delay_ms(uint32_t delay)
{
  static uint32_t tickstart = 0;
  if(!tickstart)
    tickstart = Get_Tick();
  if((Get_Tick() - tickstart) < delay)
    return 0;
  else
    {
      tickstart = 0;
      return 1;
    }
}

void GPIO_Clock_En(void)
{
#ifdef USE_GPIOA
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;//GPIOA clock enable
#endif

#ifdef USE_GPIOB
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;//GPIOB clock enable
#endif

#ifdef USE_GPIOC
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;//GPIOC clock enable
#endif

#ifdef USE_GPIOD
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;//GPIOC clock enable
#endif

#ifdef USE_GPIOE
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;//GPIOE clock enable
#endif

#ifdef USE_GPIOH
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;//GPIOH clock enable
#endif
}

/*
; CPACR is located at address 0xE000ED88
LDR.W R0, =0xE000ED88
; Read CPACR
LDR R1, [R0]
; Set bits 20-23 to enable CP10 and CP11 coprocessors
ORR R1, R1, #(0xF << 20)
; Write back the modified value to the CPACR
STR R1, [R0]; wait for store to complete
DSB
;reset pipeline now the FPU is enabled
ISB
*/

#if GFX_USE_OS_RAW32
systemticks_t gfxSystemTicks(void)
{
  return Tick;
}
systemticks_t gfxMillisecondsToTicks(delaytime_t ms)
{
  return ms;
}
#endif
