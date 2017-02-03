/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Main program body
  ******************************************************************************
  */ 



/* Includes ------------------------------------------------------------------*/
#include "main.h"


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{
  MCU_Init();
  LIS3DSH_Init();
//  Audio_Init();
  LIS3DSH_En();
  MovDetectEN();
  while (1)
  {
//	  USB_Echo();
//	  USB_Count(1000);

      LIS3DSH_View();
//      Dir_Led();
//	  Main_menu();
//	  USB_Ctrl();
//	  LED_TOGGLE(blue);
//	  PCF8812_Clear();
//	  while(1);
  }
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
