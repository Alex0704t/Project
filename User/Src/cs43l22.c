//--------------------------------------------------------------
// File     : cs43l22.c
//--------------------------------------------------------------

#include <cs43l22.h>

//--------------------------------------------------------------
// Audio Codec functions
//--------------------------------------------------------------

static void EVAL_AUDIO_Delay(volatile uint32_t nCount);
static uint32_t Codec_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue);
static uint32_t Codec_ReadRegister(uint8_t RegisterAddr);
static uint32_t Codec_PauseResume(uint32_t Cmd);
static uint32_t Codec_Stop(uint32_t CodecPdwnMode);
static uint32_t Codec_Mute(uint32_t Cmd);
static uint32_t Codec_VolumeCtrl(uint8_t volume);
static void Audio_MAL_PauseResume(uint32_t Cmd, uint32_t Addr);

//--------------------------------------------------------------
uint32_t EVAL_AUDIO_Init(uint8_t volume)
{
  uint32_t counter = 0;
  Audio_Reset_Init();
  CS43L22_RESET();
  I2C1_Init();
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;//Enable the DMA clock
  /* Keep Codec powered OFF */
  counter += Codec_WriteRegister(POW_CONT_1, POW_DOWN_1);
  /* SPK always OFF & HP always ON */
  counter += Codec_WriteRegister(POW_CONT_2, HP_ON);
  /* Clock configuration: Auto detection */
  counter += Codec_WriteRegister(CLK_CONT, AUTO_DETECT|MCLK_DIV_2);
  /* Set the Slave Mode and the audio Standard */
  counter += Codec_WriteRegister(INT_CONT_1, DACDIF_0);
  /* Set the Master volume */
  Codec_VolumeCtrl(VOLUME_CONVERT(volume));
  /* Power on the Codec */
  counter += Codec_WriteRegister(POW_CONT_1, POW_UP);
  /* Additional configuration for the CODEC. These configurations are done to reduce
     the time needed for the Codec to power off. If these configurations are removed,
     then a long delay should be added between powering off the Codec and switching
     off the I2S peripheral MCLK clock (which is the operating clock for Codec).
     If this delay is not inserted, then the codec will not shut down properly and
     it results in high noise after shut down. */
  /* Disable the analog soft ramp */
  counter += Codec_WriteRegister(ANLG_ZC_SR, 0x00);
  /* Disable the digital soft ramp */
  counter += Codec_WriteRegister(MISC_CONT, DEEMPH);
  /* Disable the limiter attack level */
  counter += Codec_WriteRegister(LIM_CONT_1, 0x00);
  /* Adjust Bass and Treble levels */
  counter += Codec_WriteRegister(TONE_CONT, 0x0F);
  /* Adjust PCM volume level */
  counter += Codec_WriteRegister(PCM_A, 0x0A);
  counter += Codec_WriteRegister(PCM_B, 0x0A);
  /* Configure the I2S peripheral */
  I2S3_Init();
  return counter;
}


//--------------------------------------------------------------
void EVAL_AUDIO_Play(void)
{
  NVIC_EnableIRQ(DMA1_Stream7_IRQn);
  NVIC_SetPriority(DMA1_Stream7_IRQn, 4);
  SPI3->CR2 |= SPI_CR2_TXDMAEN;//enable DMA transmit
  if(!(SPI3->I2SCFGR & SPI_I2SCFGR_I2SE))
      SPI3->I2SCFGR |= SPI_I2SCFGR_I2SE;//enable I2S peripheral
 }


//--------------------------------------------------------------
uint32_t EVAL_AUDIO_PauseResume(uint32_t Cmd)
{
  /* Call the Audio Codec Pause/Resume function */
  if (Codec_PauseResume(Cmd) != 0)
  {
    return 1;
  }
  else
  {
    /* Call the Media layer pause/resume function */
    Audio_MAL_PauseResume(Cmd, 0);
    /* Return 0 if all operations are OK */
    return 0;
  }
}


//--------------------------------------------------------------
void EVAL_AUDIO_Stop(uint32_t Option)
{
  EVAL_AUDIO_StopDMA();
  Codec_Stop(Option);
}


//--------------------------------------------------------------
void EVAL_AUDIO_StopDMA(void)
{
  DMA1_Stream7->CR &= ~DMA_SxCR_EN;//disable stream
  while((DMA1_Stream7->CR & DMA_SxCR_EN) == DMA_SxCR_EN);
}


//--------------------------------------------------------------
uint32_t EVAL_AUDIO_VolumeCtl(uint8_t volume)
{
  /* Call the codec volume control function with converted volume value */
  return (Codec_VolumeCtrl(VOLUME_CONVERT(volume)));
}


//--------------------------------------------------------------
void EVAL_SET_DMA(int next_buffer_len, int16_t *next_samples)
{
  DMA1_Stream7->CR &= ~DMA_SxCR_EN;//DMA stream disable
  I2S3_DMA_DeInit();
  I2S3_DMA_Init(next_buffer_len, next_samples);
}



//--------------------------------------------------------------
static void EVAL_AUDIO_Delay(volatile uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

//--------------------------------------------------------------
static uint32_t Codec_PauseResume(uint32_t Cmd)
{
  uint32_t counter = 0;
  /* Pause the audio file playing */
  if (Cmd == AUDIO_PAUSE)
  {
    /* Mute the output first */
    counter += Codec_Mute(AUDIO_MUTE_ON);
    /* Put the Codec in Power save mode */
    counter += Codec_WriteRegister(POW_CONT_1, POW_DOWN_1);
  }
  else /* AUDIO_RESUME */
  {
    /* Unmute the output first */
    counter += Codec_Mute(AUDIO_MUTE_OFF);
    counter += Codec_WriteRegister(POW_CONT_2, HP_ON);
    /* Exit the Power save mode */
    counter += Codec_WriteRegister(POW_CONT_1, POW_UP);
  }
  return counter;
}


//--------------------------------------------------------------
static uint32_t Codec_Stop(uint32_t CodecPdwnMode)
{
  uint32_t counter = 0;
  /* Mute the output first */
  Codec_Mute(AUDIO_MUTE_ON);
  if (CodecPdwnMode == CODEC_PDWN_SW)
  {
    /* Power down the DAC and the speaker (PMDAC and PMSPK bits)*/
    counter += Codec_WriteRegister(POW_CONT_1, POW_DOWN_2);
  }
  else /* CODEC_PDWN_HW */
  {
    /* Power down the DAC components */
    counter += Codec_WriteRegister(POW_CONT_1, POW_DOWN_2);
    /* Wait at least 100us */
    EVAL_AUDIO_Delay(0xFFF);
    /* Reset The pin */
    CS43L22_RESET();
  }
  return counter;
}


//--------------------------------------------------------------
static uint32_t Codec_Mute(uint32_t Cmd)
{
  uint32_t counter = 0;
  /* Set the Mute mode */
  if (Cmd == AUDIO_MUTE_ON)
  {
    counter += Codec_WriteRegister(POW_CONT_2, HP_OFF);
  }
  else /* AUDIO_MUTE_OFF Disable the Mute */
  {
    counter += Codec_WriteRegister(POW_CONT_2, HP_ON);
  }
  return counter;
}



//--------------------------------------------------------------
static uint32_t Codec_VolumeCtrl(uint8_t volume)
{
  uint32_t counter = 0;
  if (volume > 0xE6)
  {
    /* Set the Master volume */
    counter += Codec_WriteRegister(MSTA, volume - 0xE7);
    counter += Codec_WriteRegister(MSTB, volume - 0xE7);
  }
  else
  {
    /* Set the Master volume */
    counter += Codec_WriteRegister(MSTA, volume + 0x19);
    counter += Codec_WriteRegister(MSTB, volume + 0x19);
  }
  return counter;
}


//--------------------------------------------------------------
static uint32_t Codec_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue)
{
  uint32_t result;
  int16_t check = 0;
  check = I2C1_WriteByte(RegisterAddr, RegisterValue);
  if(check == 0)//all ok
    result = 0;
  else//fail
    result = 1;
#ifdef VERIFY_WRITTENDATA
  /* Verify that the data has been correctly written */
  result = (Codec_ReadRegister(RegisterAddr) == RegisterValue)? 0:1;
#endif /* VERIFY_WRITTENDATA */
  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return result;
}


//--------------------------------------------------------------
static uint32_t Codec_ReadRegister(uint8_t RegisterAddr)
{
  uint32_t result = 0;
  int16_t check = 0;
  check = I2C1_ReadByte(CODEC_ADDRESS, RegisterAddr);
  if(check >= 0)
    result = check;//all ok
  else
    result = 0;//fail
  return result;
}



//--------------------------------------------------------------
static void Audio_MAL_PauseResume(uint32_t Cmd, uint32_t Addr)
{
  /* Pause the audio file playing */
  if (Cmd == AUDIO_PAUSE)
  {
    /* Disable the I2S DMA request */
    SPI3->CR2 &= ~SPI_CR2_TXDMAEN;//Tx buffer DMA disable
    /* Pause the I2S DMA Stream
        Note. For the STM32F40x devices, the DMA implements a pause feature,
              by disabling the stream, all configuration is preserved and data
              transfer is paused till the next enable of the stream.
              This feature is not available on STM32F40x devices. */
    DMA1_Stream7->CR &= ~DMA_SxCR_EN;

  }
  else /* AUDIO_RESUME */
  {
    /* Enable the I2S DMA request */
    SPI3->CR2 |= SPI_CR2_TXDMAEN;//Tx buffer DMA enable

    /* Resume the I2S DMA Stream
        Note. For the STM32F40x devices, the DMA implements a pause feature,
              by disabling the stream, all configuration is preserved and data
              transfer is paused till the next enable of the stream.
              This feature is not available on STM32F40x devices. */
    DMA1_Stream7->CR |= DMA_SxCR_EN;
    if(!(SPI3->I2SCFGR & SPI_I2SCFGR_I2SE))
          SPI3->I2SCFGR |= SPI_I2SCFGR_I2SE;//enable I2S peripheral
  }
}


void Audio_Reset_Init()
  {
  /*
   * Audio RST - PD4
   */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
  GPIOD->MODER |= GPIO_MODER_MODER4_0;//Output
  GPIOD->OTYPER &= ~GPIO_OTYPER_OT_4;//Push-pull
  GPIOD->PUPDR |= GPIO_PUPDR_PUPDR4_1;//Pull-down
  GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4_1;//High speed
  CLEAR_AUDIO_RST();//Audio RST = 0
  }


void Get_Audiochip_ID(void)
{
  uint8_t data = Codec_ReadRegister(CHIP_ID_R);
  char str1[PCF8812_LCD_LINE], str2[PCF8812_LCD_LINE];
  PCF8812_Clear();
  if((data & 0xF0) == CS43L22_ID)
  {
    strcpy(str1, "Audiochip CS43L22");
    switch(data & 0x0F)
    {
      case REV_A0:
        strcpy(str2, "Revision A0");
        break;
      case REV_A1:
        strcpy(str2, "Revision A1");
        break;
      case REV_B0:
        strcpy(str2, "Revision B0");
        break;
  case REV_B1:
        strcpy(str2, "Revision B1");
        break;
      default:
        strcpy(str2, "Revision unknown");
        break;
    }
  }
  else
    strcpy(str1, "Audiochip unknown");
  PCF8812_Putline(str1, 3);
  PCF8812_Putline(str2, 4);
  delay_ms(2000);
}

void StartBeep(uint8_t beep_freq, uint8_t volume, uint8_t beep_on, uint8_t beep_off)
{
  EVAL_AUDIO_Init(volume);
  EVAL_SET_DMA(1, (uint16_t *)0x00);//send "dummy" byte
  EVAL_AUDIO_Play();

  Codec_WriteRegister(POW_CONT_1, POW_DOWN_1);//cs43l22 power down
  Codec_WriteRegister(INT_CONT_1, DSP|MASTER_MODE);//DSP mode enable
  Codec_WriteRegister(POW_CONT_2, HP_A_ON|HP_B_OFF);//only headphone ch. A on

  Codec_WriteRegister(CLK_CONT, SINGLE_SPEED);

  /*set beep parameters and max. volume*/
  if(beep_on == cont_mode || beep_on < 0)
    {
      Codec_WriteRegister(BP_FRQ_ON_TM, beep_freq);
      Codec_WriteRegister(BP_VLM_OFF_TM, 0x06);
      Codec_WriteRegister(BP_TN_CONF, BEEP_CONT|BEEP_MIX_DIS);
    }
  else if(beep_off >= single_mode)
    {
      Codec_WriteRegister(BP_FRQ_ON_TM, beep_freq|beep_on);
      Codec_WriteRegister(BP_VLM_OFF_TM, 0x06);
      Codec_WriteRegister(BP_TN_CONF, BEEP_SING|BEEP_MIX_DIS);
    }
  else
    {
      Codec_WriteRegister(BP_FRQ_ON_TM, beep_freq|beep_on);
      Codec_WriteRegister(BP_VLM_OFF_TM, 0x06|beep_off);
      Codec_WriteRegister(BP_TN_CONF, BEEP_MULT|BEEP_MIX_DIS);
    }

  Codec_WriteRegister(POW_CONT_1, POW_UP);

}

void StopBeep()
{
  EVAL_AUDIO_Stop(CODEC_PDWN_HW);
  NVIC_DisableIRQ(DMA1_Stream7_IRQn);
}

extern Par_list beep_freq_list;
extern Par_list beep_on_list;
extern Par_list beep_off_list;


void SetBeep()
{
  uint8_t freq = PCF8812_Set_Param(&beep_freq_list);
  uint8_t on = PCF8812_Set_Param(&beep_on_list);
  uint8_t off = (on != cont_mode) ? PCF8812_Set_Param(&beep_off_list) : 0;
  StartBeep(freq, 70, on, off);
}

void BeepHandler()
{
  if(DMA1->HISR & DMA_HISR_TCIF7)//transfer complete
    {
      DMA1->HIFCR = DMA_HIFCR_CTCIF7;//clear flag
      DMA1_Stream7->CR |= DMA_SxCR_EN;//enable stream
      DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
    }
}

void AnalogWave_Init(uint16_t frequency, uint8_t volume)
{
  EVAL_AUDIO_Init(volume);
  EVAL_SET_DMA(1, (int16_t *)0x00);//send "dummy" byte
  EVAL_AUDIO_Play();

  //DAC1_Init();
  Tim6_Init(frequency);
  //DAC1_Init();
  DAC1_DMA_Init();

  Codec_WriteRegister(POW_CONT_1, POW_DOWN_1);//cs43l22 power down

  Codec_WriteRegister(PASS_A_SEL, AIN_1);
  Codec_WriteRegister(MISC_CONT, PASSTHRU_A);

  Codec_WriteRegister(POW_CONT_1, POW_UP);

}

void AnalogWave_Start()
{
#ifdef USE_DAC1_DMA
  DMA1_Stream5->CR |= DMA_SxCR_EN;//stream enable
#endif
  Tim6_Start();
  DAC->CR |= DAC_CR_EN1;//DAC channel1 enable
}

void AnalogWave_Stop()
{
#ifdef USE_DAC1_DMA
  DMA1_Stream5->CR &= ~DMA_SxCR_EN;//stream disable
#endif
  DAC->CR &= ~DAC_CR_EN1;//DAC channel1 disable
  Tim6_Stop();
}

void AnalogWave_Set(uint16_t frequency, uint8_t volume)
{
  EVAL_AUDIO_VolumeCtl(volume);
  Tim6_Set(frequency);
}

void SetVolume(void)
{
  static uint8_t volume = 0;
  RESET_ENC;
  while(1)
    {
    PCF8812_Clear();
    PCF8812_Title("VOLUME");
    Button_Set_Name(user_button, "OK");
    Button_Set_Name(button_1, "-10%");
    Button_Set_Name(button_2, "+10%");
    if(Button_Get(button_1))
      DECR_ENC(VOLUME_STEP);
    if(Button_Get(button_2))
      INCR_ENC(VOLUME_STEP);
    if(Button_Get(user_button))
      break;
    /*static uint8_t */volume = Get_Enc_Count(100);
    PCF8812_Percent("Volume", volume, 2);
    EVAL_AUDIO_VolumeCtl(volume);
    PCF8812_DELAY;
    }
}


Par_list beep_freq_list = {
   "FREQUENCY",
   16,
   "260 Hz", freq260,
   "520 Hz", freq520,
   "585 Hz", freq585,
   "666 Hz", freq666,
   "705 Hz", freq705,
   "774 Hz", freq774,
   "888 Hz", freq888,
  "1000 Hz", freq1000,
  "1043 Hz", freq1043,
  "1200 Hz", freq1200,
  "1333 Hz", freq1333,
  "1411 Hz", freq1411,
  "1600 Hz", freq1600,
  "1714 Hz", freq1714,
  "2000 Hz", freq2000,
  "2181 Hz", freq2181
};

Par_list beep_on_list = {
    "ON TIME",
    17,
    "86 ms",            on_86ms,
    "430 ms",           on_430ms,
    "780 ms",           on_780ms,
    "1.2 s",            on_1s2,
    "1.5 s",            on_1s5,
    "1.8 s",            on_1s8,
    "2.2 s",            on_2s2,
    "2.5 s",            on_2s5,
    "2.8 s",            on_2s8,
    "3.2 s",            on_3s2,
    "3.5 s",            on_3s5,
    "3.8 s",            on_3s8,
    "4.2 s",            on_4s2,
    "4.5 s",            on_4s5,
    "4.8 s",            on_4s8,
    "5.2 s",            on_5s2,
    "Continuous mode",  cont_mode
};

Par_list beep_off_list = {
    "OFF TIME",
    9,
    "1.23 s",       off_1s23,
    "2.58 s",       off_2s58,
    "3.9 s",        off_3s90,
    "5.2 s",        off_5s20,
    "6.6 s",        off_6s60,
    "8.05 s",       off_8s05,
    "9.35 s",       off_9s35,
    "10.8 s",       off_10s80,
    "Single mode",  single_mode
};

