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


  while (1)
  {
//	  delay_ms(1000);
//	  uint8_t len = Read_from_USB(&Buffer);
//	  if (/*Out*/Buffer[0]&1)  	LED_ON(orange);
//		  	  else			LED_OFF(orange);
//	  if (/*Out*/Buffer[0]&2)  	LED_ON(green);
//		  	  else			LED_OFF(green);
//	  if (/*Out*/Buffer[0]&4)  	LED_ON(red);
//		  	  else			LED_OFF(red);
//	  if (/*Out*/Buffer[0]&8)  	LED_ON(blue);
//		  	  else			LED_OFF(blue);
//	  //PCF8812_LIGHT_ON();
//	  PCF8812_Putline("Receive:",2);
//	  PCF8812_Putline(/*Out*/Buffer, 3);
//    PCF8812_Putline("Send:",4);
//    PCF8812_UValue("bytes ", len, "", 5);
//    Write_to_USB(&Buffer);
//    ClearBuf(&Buffer, 63);
//
//    USB_HID_SendReport("HI\n", 64);
//	  USB_Echo();
//	  USB_Count(1000);
//	  Main_menu();
	  USB_Ctrl();
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
