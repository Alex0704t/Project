//--------------------------------------------------------------
// File     : stm32_cs43l22_mp3.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_CS43L22_MP3_H
#define __STM32F4_UB_CS43L22_MP3_H


//#define USE_MP3

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include <cs43l22.h>

#include "../Audio/helix_mp3/pub/mp3dec.h"
#include "../Device/stm32f4xx.h"



//--------------------------------------------------------------
// Structure of a MP3 file
//--------------------------------------------------------------
typedef struct UB_MP3_t
{
  const uint8_t *table;//Table with the data
  uint32_t size;       //Number of bytes
}UB_MP3;

//--------------------------------------------------------------
// Use the embed MP3-Files
//--------------------------------------------------------------
extern UB_MP3 skinny_mp3;
extern UB_MP3 alarm1;
extern UB_MP3 alarm2;
extern UB_MP3 signal1;
extern UB_MP3 signal2;

//--------------------------------------------------------------
// internal Enum
//--------------------------------------------------------------
typedef enum {
  CS43L22_NO_INIT = 0,
  CS43L22_INIT_OK,
  CS43L22_PLAY,
  CS43L22_PAUSE,
  CS43L22_STOP
}CS43L22_STATUS_t;

#define   MP3_BUFFER_SIZE   4096

//--------------------------------------------------------------
// Error list
//--------------------------------------------------------------
typedef enum {
  MP3_OK = 0,       //no error
  MP3_INIT_ERR,     //failed to init
  MP3_MALLOC_ERR,   //not enough RAM free
  MP3_SYNC_ERR,     //found no sync
  MP3_FRQ_ERR,      //error in Samplefrq
  MP3_FRAME_ERR,    //other frame error
  MP3_IDATA_ERR,    //indata underflow
  MP3_MDATA_ERR,    //maindata underflow
  MP3_DECODE_ERR    //other decoder error
}MP3_ERR_t;


//--------------------------------------------------------------
// internal structure
//--------------------------------------------------------------
typedef struct {
  HMP3Decoder mp3_data;
  MP3FrameInfo mp3_info;
  const uint8_t *start_ptr;
  const uint8_t *read_ptr;
  int bytes_left_start;
  int bytes_left;
  int16_t *data_ptr;
  int16_t buffer0[MP3_BUFFER_SIZE];
  int16_t buffer1[MP3_BUFFER_SIZE];
  uint8_t akt_buf_nr;
  uint8_t ende_flag;
  uint8_t dma_enable;
  int16_t *next_buffer_ptr;
  int next_buffer_len;
  uint8_t loop_flag;
  MP3_ERR_t last_err;
}MP3_t;

//--------------------------------------------------------------
// HELIX MP3 Defines
//--------------------------------------------------------------
#define MP3_MIN_BYTES_REMAINING   10000   //the end of the mp3 file
#define MP3_BYTES_SKIP            1000    //boar jump bytes

//--------------------------------------------------------------
// Global functions
//--------------------------------------------------------------
ErrorStatus UB_CS43L22_InitMP3(void);
MP3_ERR_t UB_CS43L22_PlayMP3Single(UB_MP3 *mp3, uint8_t volume);
MP3_ERR_t UB_CS43L22_PlayMP3Loop(UB_MP3 *mp3, uint8_t volume);
void UB_CS43L22_StopMP3(void);
void UB_CS43L22_PauseMP3(void);
void UB_CS43L22_ResumeMP3(void);
void UB_CS43L22_SetVolumeMP3(uint8_t volume);
MP3_ERR_t UB_CS43L22_GetMP3Err(void);

//--------------------------------------------------------------
#endif // __STM32F4_UB_CS43L22_MP3_H
