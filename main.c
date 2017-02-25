/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Main program body
  ******************************************************************************
  */ 



/* Includes ------------------------------------------------------------------*/
#include <stm32_fatfs.h>
#include "main.h"


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  MCU_Init();

#if 1
  FIL myFile;

  // Init vom FATFS-System
  Fatfs_Init();

  // Check ob Medium eingelegt ist
  if(UB_Fatfs_CheckMedia(MMC_0)==FATFS_OK) {
    // Media mounten
    if(UB_Fatfs_Mount(MMC_0)==FATFS_OK) {
      LED_ON(blue);
      // File zum schreiben im root neu anlegen
      if(UB_Fatfs_OpenFile(&myFile, "0:/New_File.txt", F_WR_CLEAR) == FATFS_OK) {
          LED_ON(green);
        // ein paar Textzeilen in das File schreiben
        UB_Fatfs_WriteString(&myFile,"FATFS in SPI-Mode");
        UB_Fatfs_WriteString(&myFile,"here second line");
        UB_Fatfs_WriteString(&myFile,"end");
        // File schliessen
        UB_Fatfs_CloseFile(&myFile);
      }
      // Media unmounten
      UB_Fatfs_UnMount(MMC_0);
    }
    else {
        LED_ON(red);
    }
  }
  double pi = 3.14;
  PCF8812_Print("Done.Bye!\nI try print\nmultistring text right now!\n");
  PCF8812_Printf("test %d %.2f\n", 666, pi);
  for (uint8_t i = 0; i < 10; i++)
    PCF8812_Printf("iter %d\n", i);

  while(1)
  {


#else
  SPI1_DMA_Init();
  LIS3DSH_En(SCALE_2G, ODR_25Hz);
//  LIS3DSH_MovDetEn(SCALE_2G, ODR_25Hz);
  while(1)
  {
//
      LIS3DSH_View();
//      Main_menu();
#endif
  }
}



/*
int main(void) {
  MCU_Init();
  UB_CS43L22_InitMP3();

  while (1) {

      UB_CS43L22_PlayMP3Loop(&alarm2, 70);
//
//	  LIS3DSH_Use();
  }
}

*/

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
