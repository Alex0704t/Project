//--------------------------------------------------------------
// File     : stm32_cs43l22_mp3.c
// Date     : 20.02.2016
// CPU      : STM32F4
// IDE      : Eclipse
// Function : CS43L22 (2 channel Audio-DAC)
//            play MP3 files from the Flash
//(SamplFrq of the MP3 must be 44.1 kHz!!)
//
// Note     :uses I2C1 and I2S3
// 
//            I2C =>  PB9  = SDA   [SlaveAdr=0x94]
//                    PB6  = SCL
//            I2S =>  PA4  = WS
//                    PC7  = MCLK
//                    PC10 = SCK
//                    PC12 = SD
//            Reset = PD4
//
//      to convert from MP3 files the PC programme is able:
//"FileConverter_UB.exe" are used
//
// MP3-Decoder : Helix-MP3
//      Source : https://datatype.helixcommunity.org/Mp3dec
//      Author : Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "../Audio/stm32_cs43l22_mp3.h"


//--------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------
CS43L22_STATUS_t cs43l22_status=CS43L22_NO_INIT;
MP3_t my_mp3;



//--------------------------------------------------------------
// Internal Functions
//--------------------------------------------------------------
void P_CS43L22_InitMP3(void);
//--------------------------------------------------------------
typedef void MP3CallbackFunction(void *context,int buffer);
static MP3CallbackFunction *MP3CallbackFunctionPtr;
static void *CallbackContextPtr;
//--------------------------------------------------------------
void PlayMP3WithCallback(MP3CallbackFunction *callback_ptr,void *context);
static void MP3CallbackFct(void *context,int buffer_nr);
void SetMP3Buffer(void *data_ptr, int sampl_cnt);
uint8_t TryMP3Buffer(void *data_ptr, int sampl_cnt);
void StartMP3Buffer(void);

//--------------------------------------------------------------
//Initialisation of the CS43L22 (in the MP3 fashion)
//Return value:
//-> ERROR, if initialisation failed
//-> SUCCESS if initialisation was OK
//--------------------------------------------------------------
ErrorStatus UB_CS43L22_InitMP3(void)
{
  ErrorStatus ret = ERROR;
  uint32_t check;
  if(cs43l22_status == CS43L22_NO_INIT)
  {
        my_mp3.mp3_data = MP3InitDecoder();//Init of MP3-Decoder
    if(my_mp3.mp3_data == 0)
    {
      //not enough free ram
      my_mp3.last_err = MP3_MALLOC_ERR;
      return ERROR;
    }

    check = 0;//EVAL_AUDIO_Init(70);
    if(check == 0)
    {
      ret = SUCCESS;
      my_mp3.last_err = MP3_OK;
      cs43l22_status = CS43L22_INIT_OK;
    }
    else
    {
      my_mp3.last_err = MP3_INIT_ERR;
      ret = ERROR;
    }
  }

  return ret;
}


//--------------------------------------------------------------
// if MP3 file playing once (from the Flash)
//-> MP3 has to come too & operator are handed over
//Volume: [0... 100]
//Return value:
//-> MP3_OK, MP3 is played
//-> != MP3_OK, MP3 is not played
//--------------------------------------------------------------
MP3_ERR_t UB_CS43L22_PlayMP3Single(UB_MP3 *mp3, uint8_t volume)
{
  MP3_ERR_t ret = MP3_INIT_ERR;

  if((cs43l22_status == CS43L22_INIT_OK) || (cs43l22_status == CS43L22_STOP)) {
    cs43l22_status = CS43L22_PLAY;

    //Start values
    my_mp3.start_ptr = &mp3->table[0];
    my_mp3.read_ptr = my_mp3.start_ptr;
    my_mp3.bytes_left_start = mp3->size;
    my_mp3.bytes_left = my_mp3.bytes_left_start;

    //only once playing
    my_mp3.loop_flag = 0;

    //Init of the remaining variables
    P_CS43L22_InitMP3();


    EVAL_AUDIO_Init(volume);//Init CS43L22

    PlayMP3WithCallback(MP3CallbackFct, 0);//MP3 start
    ret = MP3_OK;

  }

  return ret;
}

//--------------------------------------------------------------
// if MP3 file playing in loop (from the Flash)
//-> MP3 has to come too & operator are handed over
//Volume: [0... 100]
//Return value:
//-> MP3_OK, MP3 is played
//-> != MP3_OK, MP3 is not played
//--------------------------------------------------------------
MP3_ERR_t UB_CS43L22_PlayMP3Loop(UB_MP3 *mp3, uint8_t volume)
{
  MP3_ERR_t ret = MP3_INIT_ERR;

  if((cs43l22_status == CS43L22_INIT_OK) || (cs43l22_status == CS43L22_STOP))
  {
    cs43l22_status = CS43L22_PLAY;

    //Start value
    my_mp3.start_ptr = &mp3->table[0];
    my_mp3.read_ptr = my_mp3.start_ptr;
    my_mp3.bytes_left_start = mp3->size;
    my_mp3.bytes_left = my_mp3.bytes_left_start;
    
    my_mp3.loop_flag = 1;//File in an endless loop play

    P_CS43L22_InitMP3();//Init remain variables

    EVAL_AUDIO_Init(volume);//Init CS43L22

    PlayMP3WithCallback(MP3CallbackFct, 0);//MP3 start

    ret = MP3_OK;

  }

  return ret;
}


//--------------------------------------------------------------
// Pause-Function
//--------------------------------------------------------------
void UB_CS43L22_PauseMP3(void)
{
  if(cs43l22_status == CS43L22_PLAY) {
    cs43l22_status = CS43L22_PAUSE;
    EVAL_AUDIO_PauseResume(AUDIO_PAUSE);
  }
}


//--------------------------------------------------------------
// Resume-Function
//--------------------------------------------------------------
void UB_CS43L22_ResumeMP3(void)
{
  if(cs43l22_status == CS43L22_PAUSE) {
    cs43l22_status = CS43L22_PLAY;
    EVAL_AUDIO_PauseResume(AUDIO_RESUME);
  }
}


//--------------------------------------------------------------
// Stop-Function
//--------------------------------------------------------------
void UB_CS43L22_StopMP3(void)
{
  if((cs43l22_status == CS43L22_PLAY) || (cs43l22_status == CS43L22_PAUSE)) {
    cs43l22_status = CS43L22_STOP;
    EVAL_AUDIO_Stop(CODEC_PDWN_HW);//CODEC_PDWN_SW => noise

    my_mp3.dma_enable = 0;
    my_mp3.next_buffer_ptr = 0;
  }
}


//--------------------------------------------------------------
// Volume : [0...100]
//--------------------------------------------------------------
void UB_CS43L22_SetVolumeMP3(uint8_t Volume)
{
  EVAL_AUDIO_VolumeCtl(Volume);
}


//--------------------------------------------------------------
// the last mistake MP3 select
// Return value :
//  ->   MP3_OK , all OK
//  -> !=MP3_OK , Error
//--------------------------------------------------------------
MP3_ERR_t UB_CS43L22_GetMP3Err(void)
{
  MP3_ERR_t ret;

  ret = my_mp3.last_err;
  my_mp3.last_err = MP3_OK;

  return ret;
}


//--------------------------------------------------------------
// internal Function
//--------------------------------------------------------------
void P_CS43L22_InitMP3(void)
{
  //Reset all Variables
  my_mp3.akt_buf_nr = 0;
  my_mp3.data_ptr = my_mp3.buffer0;
  my_mp3.ende_flag = 0;
  my_mp3.dma_enable = 0;
  my_mp3.next_buffer_ptr = 0;
  my_mp3.next_buffer_len = 0;
  my_mp3.last_err = MP3_OK;

  MP3CallbackFunctionPtr = 0;
  CallbackContextPtr = 0;
}


//--------------------------------------------------------------
// internal Function
//--------------------------------------------------------------
void PlayMP3WithCallback(MP3CallbackFunction *callback_ptr, void *context)
{
  
  EVAL_AUDIO_StopDMA();
  my_mp3.dma_enable = 0;

  EVAL_AUDIO_Play();

  MP3CallbackFunctionPtr = callback_ptr;
  CallbackContextPtr = context;
  my_mp3.akt_buf_nr = 0;

  if (MP3CallbackFunctionPtr) {
    MP3CallbackFunctionPtr(CallbackContextPtr, my_mp3.akt_buf_nr);
  }
}


//--------------------------------------------------------------
// Internal Function
//--------------------------------------------------------------
static void MP3CallbackFct(void *context, int buffer_nr)
{
  int offset;
  int error;
  int ok;

  //find given next sync
  offset = MP3FindSyncWord((unsigned char*)my_mp3.read_ptr, my_mp3.bytes_left);
  if(offset < 0) {
    //found no sync
    my_mp3.last_err = MP3_SYNC_ERR;
    my_mp3.ende_flag = 1;
  }
  else if(offset > 0){
    //but sync ok no data
    //reading frame info
    error = MP3GetNextFrameInfo(my_mp3.mp3_data, &my_mp3.mp3_info,(unsigned char*)my_mp3.read_ptr);
    if(error == ERR_MP3_NONE) {
      //no mistake
      if(my_mp3.mp3_info.samprate != 44100) {
        //false sampelrate
        my_mp3.last_err = MP3_FRQ_ERR;
        my_mp3.ende_flag = 1;
      }
    }
    else if(error == ERR_MP3_INVALID_FRAMEHEADER) {
      //in wrong frame header
      //the frame skip
      //that here is NOT a mistake !!
      my_mp3.bytes_left -= offset;
      my_mp3.read_ptr += offset;
      //check whether the end achieved by mp3
      if(my_mp3.bytes_left < MP3_MIN_BYTES_REMAINING) {
        my_mp3.ende_flag = 1;
      }
      MP3CallbackFunctionPtr(context, buffer_nr);
    }
    else {
      //other error
      my_mp3.last_err = MP3_FRAME_ERR;
      my_mp3.ende_flag = 1;
    }
  }
  else {
    //sync ok and play data
    error = MP3Decode(my_mp3.mp3_data, (unsigned char**)&my_mp3.read_ptr, &my_mp3.bytes_left, my_mp3.data_ptr, 0);
    switch(error) {
      case ERR_MP3_NONE :
        //decode ok(no error)
        ok = 0;
      break;
      case ERR_MP3_INDATA_UNDERFLOW :
        //reached the end
    	my_mp3.last_err = MP3_IDATA_ERR;
        my_mp3.ende_flag = 1;
        ok = -1;
      break;
      case ERR_MP3_MAINDATA_UNDERFLOW :
        //not SUFFICIENT data
        //read simply again
        MP3CallbackFunctionPtr(context, buffer_nr);
        my_mp3.last_err = MP3_MDATA_ERR;
        ok = -1;
      break;
      default :
        //Error occurred
        //some bytes skip and read again
    	my_mp3.last_err = MP3_DECODE_ERR;
        if(my_mp3.bytes_left > (2*MP3_BYTES_SKIP)) {
          my_mp3.bytes_left -= MP3_BYTES_SKIP;
          my_mp3.read_ptr += MP3_BYTES_SKIP;
          MP3CallbackFunctionPtr(context, buffer_nr);
        }
        else {
          //Error at the end
          my_mp3.ende_flag = 1;
        }
        ok = -1;
      break;
    }

    if(ok == 0) {
      //Reading frame info
      MP3GetLastFrameInfo(my_mp3.mp3_data, &my_mp3.mp3_info);
      if(my_mp3.mp3_info.outputSamps > 0) {
        //Play data from buffer
        SetMP3Buffer(my_mp3.data_ptr, my_mp3.mp3_info.outputSamps);
        //Switch buffer
        if (buffer_nr == 0) {
          my_mp3.data_ptr = my_mp3.buffer0;
        } else {
          my_mp3.data_ptr = my_mp3.buffer1;
        }
      }
    }
  }

  //test whether border reached
  if(my_mp3.ende_flag == 1) {
    //test on Play Loop
    if(my_mp3.loop_flag == 1) {
      //skip to top
      my_mp3.ende_flag = 0;
      my_mp3.read_ptr = my_mp3.start_ptr;
      my_mp3.bytes_left = my_mp3.bytes_left_start;
      my_mp3.akt_buf_nr = 0;
      my_mp3.data_ptr = my_mp3.buffer0;
      MP3CallbackFunctionPtr(context, buffer_nr);
    }
    else {
      //stop mp3
      EVAL_AUDIO_Stop(CODEC_PDWN_HW);//CODEC_PDWN_SW => noise
      cs43l22_status = CS43L22_STOP;
    }
  }
}



//--------------------------------------------------------------
// internal Function
//--------------------------------------------------------------
void SetMP3Buffer(void *data_ptr, int sampl_cnt)
{
  while (!TryMP3Buffer(data_ptr, sampl_cnt)) {
    __asm__ volatile ("wfi");
  }
}


//--------------------------------------------------------------
// internal Function
//--------------------------------------------------------------
uint8_t TryMP3Buffer(void *data_ptr, int sampl_cnt)
{
  if (my_mp3.next_buffer_ptr) {
    return 0;
  }

  NVIC_DisableIRQ(DMA1_Stream7_IRQn);

  my_mp3.next_buffer_ptr = data_ptr;
  my_mp3.next_buffer_len = sampl_cnt;

  if (my_mp3.dma_enable==0) {
    StartMP3Buffer();
  }

  NVIC_EnableIRQ(DMA1_Stream7_IRQn);

  return 1;
}





//--------------------------------------------------------------
// internal Function
//--------------------------------------------------------------
void StartMP3Buffer(void)
{
  EVAL_SET_DMA(my_mp3.next_buffer_len, my_mp3.next_buffer_ptr);

  my_mp3.next_buffer_ptr = 0;
  my_mp3.akt_buf_nr = 1 - my_mp3.akt_buf_nr;
  my_mp3.dma_enable = 1;

  if (MP3CallbackFunctionPtr) {
    MP3CallbackFunctionPtr(CallbackContextPtr, my_mp3.akt_buf_nr);
  }
}





//--------------------------------------------------------------
// it is called if MP3 buffer was completely played
//--------------------------------------------------------------
void EVAL_AUDIO_TransferComplete_CallBack(void)
{
  if(DMA1->HISR & DMA_HISR_TCIF7)//transfer complete
  {
    DMA1->HIFCR = DMA_HIFCR_CTCIF7;//clear flag
    if (my_mp3.next_buffer_ptr) {
      StartMP3Buffer();
    } else {
      my_mp3.dma_enable = 0;
    }
  }
}











