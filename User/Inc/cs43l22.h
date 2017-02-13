//--------------------------------------------------------------
// File     : cs43l22.h
//--------------------------------------------------------------


#ifndef __CS43L22_H
#define __CS43L22_H

//#include "../Device/stm32f4xx.h"
#include "user.h"


#define VERIFY_WRITTENDATA
//#define USE_I2S3_MCLK_ONLY//uncomment if use only beep or sine generation
#define CODEC_ADDRESS                 0x94

#define AUDIO_PAUSE                   0
#define AUDIO_RESUME                  1

#define CODEC_PDWN_HW                 1
#define CODEC_PDWN_SW                 2

#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0
//--------------------------------------------------------------
#define VOLUME_CONVERT(volume)    ((volume > 100)? 100:((uint8_t)((volume * 255) / 100)))
//--------------------------------------------------------------

/**************************************************************/

#define SET_AUDIO_RST()    GPIOD->BSRR = GPIO_BSRR_BS_4
#define CLEAR_AUDIO_RST()  GPIOD->BSRR = GPIO_BSRR_BR_4
#define CS43L22_RESET()    do {               \
                           CLEAR_AUDIO_RST(); \
                           delay_ms(1);       \
                           SET_AUDIO_RST();   \
                           }                  \
                           while(0)
#define PERIOD             3
#define VOLUME_STEP        10
#define SINE_FREQ_STEP     10

/** CS43L22 registers ***********************************************/
/** Registers address ***********************************************/
#define CHIP_ID_R       0x01//Chip ID & revision
#define POW_CONT_1      0x02//Power control 1
#define POW_CONT_2      0x04//Power control 2
#define CLK_CONT        0x05//Clocking control
#define INT_CONT_1      0x06//Interface control 1
#define INT_CONT_2      0x07//Interface control 2
#define PASS_A_SEL      0x08//Passthrough A Select
#define PASS_B_SEL      0x09//Passthrough B Select
#define ANLG_ZC_SR      0x0A//Analog ZC and SR Settings
#define PASS_GANG_CONT  0x0C//Passthrough Gang Control
#define PLY_BCK_CONT_1  0x0D//Playback Control 1
#define PLY_BCK_CONT_2  0x0F//Playback Control 2
#define MISC_CONT       0x0E//Miscellaneous Controls
#define PASS_A_VOL      0x14//Passthrough A Volume
#define PASS_B_VOL      0x15//Passthrough B Volume
#define PCM_A           0x1A//PCM A Volume
#define PCM_B           0x1B//PCM B Volume
#define BP_FRQ_ON_TM    0x1C//Beep Frequency & On Time
#define BP_VLM_OFF_TM   0x1D//Beep Volume & Off Time
#define BP_TN_CONF      0x1E//Beep & Tone Configuration
#define TONE_CONT       0x1F//Tone Control
#define MSTA            0x20//Master Volume Control A
#define MSTB            0x21//Master Volume Control B
#define HPA             0x22//Headphone Volume Control A
#define HPB             0x23//Headphone Volume Control B
#define SPKA            0x24//Speaker Volume Control A
#define SPKB            0x25//Speaker Volume Control B
#define PCM_SWP         0x26//PCM Channel Swap
#define LIM_CONT_1      0x27//Limiter Control 1, Min/Max Thresholds
#define LIM_CONT_2      0x28//Limiter Control 2, Release Rate
#define LIM_ATT_RATE    0x29//Limiter Attack Rate
#define STAT_R          0x2E//Status(Read Only)
#define BATT_COMP       0x2F//Battery Compensation
#define VP_BATT_LVL     0x30//VP Battery Level(Read Only)
#define SPK_STAT        0x31//Speaker Status(Read Only)
#define CHG_FREQ        0x34//Charge Pump Frequency
/** Chip ID & revision *************************************/
#define CS43L22_ID      0xE0
#define REV_A0          0x00
#define REV_A1          0x01
#define REV_B0          0x02
#define REV_B1          0x03
/** Power control 1 ****************************************/
#define POW_DOWN_1      0x01//Powered Down - same as setting 1001 1111
#define POW_DOWN_2      0x9F//Powered Up
#define POW_UP          0x9E//Powered Down - same as setting 0000 0001
/** Power control 2 ****************************************/
#define HP_B_ON         0x8F//Headphone channel B is always ON
#define HP_B_OFF        0xCF//Headphone channel B is always OFF
#define HP_A_ON         0x2F//Headphone channel A is always ON
#define HP_A_OFF        0x1F//Headphone channel A is always OFF
#define HP_ON           0xAF//Headphone channels is always ON
#define HP_OFF          0xFF//Headphone channels is always OFF
/** Clocking control ***************************************/
#define AUTO_DETECT     0x80//Auto-detection of Speed Mode
#define DOUBLE_SPEED    0x9F//Double-Speed Mode (DSM - 50 kHz -100 kHz Fs) clear bits mask
#define SINGLE_SPEED    0x20//Single-Speed Mode (SSM - 4 kHz -50 kHz Fs)
#define HALF_SPEED      0x40//Half-Speed Mode (HSM - 12.5kHz -25 kHz Fs)
#define QUARTER_SPEED   0x60//Quarter-Speed Mode (QSM - 4 kHz -12.5 kHz Fs)
#define GROUP_32K       0x10//8 kHz, 16 kHz or 32 kHz sample rate
#define VIDEOCLK        0x08//27 MHz MCLK
#define RATIO_1         0x04//Internal MCLK/LRCK Ratio bit 1
#define RATIO_0         0x02//Internal MCLK/LRCK Ratio bit 0
#define MCLK_DIV_2      0x01//Divides the input MCLK by 2 prior to all internal circuitry
/** Interface control 1 ************************************/
#define MASTER_MODE     0x80//Serial port I/O clocking master(output ONLY)
#define INV_SCLK        0x40//Inverted polarity of the SCLK signal
#define DSP             0x10//DSP Mode enabled
#define DACDIF_1        0x08//DAC Interface Format bit 1
#define DACDIF_0        0x04//DAC Interface Format bit 0
#define AWL_1           0x02//Audio Word Length bit 1
#define AWL_0           0x01//Audio Word Length bit 0
/** Interface control 2 ************************************/
#define SCLK_MCLK       0x40//SCLK equals MCLK
#define INV_SWCH        0x08//Inverted signal polarity of the SPK/HP_SW pin
/** Passthrough x Select ***********************************/
#define AIN_1           0x01//AIN1 select
#define AIN_2           0x02//AIN2 select
#define AIN_3           0x04//AIN3 select
#define AIN_4           0x08//AIN4 select
/** Analog ZC and SR Settings ******************************/
#define ANLG_SFT_B      0x08//Ch. B Analog Soft Ramp
#define ANLG_SFT_A      0x02//Ch. A Analog Soft Ramp
#define ANLG_ZC_B       0x04//Ch. B Analog Zero Cross
#define ANLG_ZC_A       0x01//Ch. A Analog Zero Cross
/** Passthrough Gang Control *******************************/
#define PASS_B_EQ_A     0x80//Single Volume Control Enabled
/** Playback Control 1 *************************************/
#define HP_GAIN_2       0x80//Headphone/Line Gain Setting bit 2
#define HP_GAIN_1       0x40//Headphone/Line Gain Setting bit 1
#define HP_GAIN_0       0x20//Headphone/Line Gain Setting bit 0
#define PLY_BCK_B_EQ_A  0x10//Single Volume Control for all Playback Channels Enabled
#define INV_PCMB        0x08//Invert B PCM Signal Polarity
#define INV_PCMA        0x04//Invert A PCM Signal Polarity
#define MST_B_MUTE      0x02//Configures a digital mute on the master volume control for channel B
#define MST_A_MUTE      0x01//Configures a digital mute on the master volume control for channel A
/** Playback Control 2 *************************************/
#define HP_B_MUTE       0x80//digital mute on headphone channel B
#define HP_A_MUTE       0x40//digital mute on headphone channel A
#define SPK_B_MUTE      0x20//digital mute on speaker channel B
#define SPK_A_MUTE      0x10//digital mute on speaker channel A
#define SPK_B_EQ_A      0x08//Single Volume Control for the Speaker Channel
#define SPK_SWAP        0x04//Configures a channel swap on the speaker channels
#define SPK_MONO        0x02//Parallel Full Bridge Output
#define MUTE_50_50      0x01//Speaker Mute 50/50
/** Miscellaneous Controls *********************************/
#define PASSTHRU_B      0x80//Analog In Routed to HP/Line Output B
#define PASSTHRU_A      0x40//Analog In Routed to HP/Line Output A
#define PASS_B_MUTE     0x20//analog mute on the channel B analog in to analog out passthrough
#define PASS_A_MUTE     0x10//analog mute on the channel A analog in to analog out passthrough
#define FREEZE          0x08//hold on all register settings
#define DEEMPH          0x04//digital de-emphasis filter
#define DIG_SFT         0x02//incremental volume ramp
#define DIG_ZC          0x01//Digital Zero Cross
/** PCMx Volume ********************************************/
#define PCM_MUTE        0x80//PCM Channel x Mute
/** Beep & Tone Configuration ******************************/
#define BEEP_OFF        0x3F//Beep off clear bits mask
#define BEEP_SING       0x40//Beep single
#define BEEP_MULT       0x80//Beep multiple
#define BEEP_CONT       0xC0//Beep continuous
#define BEEP_MIX_DIS    0x20//Beep Mix Disable
#define TREBCF_5        0xE7//Treble Corner Frequency 5 kHz clear bits mask
#define TREBCF_7        0x08//Treble Corner Frequency 7 kHz
#define TREBCF_10       0x10//Treble Corner Frequency 10 kHz
#define TREBCF_15       0x18//Treble Corner Frequency 15 kHz
#define BASSCF_50       0xF9//Bass Corner Frequency 50 Hz clear bits mask
#define BASSCF_100      0x02//Bass Corner Frequency 100 Hz
#define BASSCF_200      0x04//Bass Corner Frequency 200 Hz
#define BASSCF_250      0x06//Bass Corner Frequency 250 Hz
#define TCEN            0x01//Tone Control Enable
/** PCM Channel Swap ***************************************/
#define PCM_A_L         0x7F//HP/LINEOUTA Left clear bits mask
#define PCM_A_MIX       0x80//HP/LINEOUTA (Left + Right)/2
#define PCM_A_R         0xC0//HP/LINEOUTA Right
#define PCM_B_R         0xCF//HP/LINEOUTA Right clear bits mask
#define PCM_B_MIX       0x20//HP/LINEOUTA (Left + Right)/2
#define PCM_B_L         0x30//HP/LINEOUTA Left
/** Limiter Control 1, Min/Max Thresholds ******************/
#define LIM_SR_DIS      0x02//Limiter Soft Ramp Disable
#define LIM_ZC_DIS      0x01//Limiter Zero Cross Disable
/** Limiter Control 2, Release Rate ************************/
#define LIMIT           0x80//Peak Detect and Limiter
#define LIMIT_ALL       0x40//Peak Signal Limit All Channels
/** Status(Read Only) **************************************/
#define SP_CLK_ERR      0x40//Serial Port Clock Error
#define DSPA_OVFL       0x20//DSPA Engine Overflow
#define DSPB_OVFL       0x10//DSPB Engine Overflow
#define PCMA_OVFL       0x08//PCMA Overflow
#define PCMB_OVFL       0x04//PCMB Overflow
/** Battery Compensation ***********************************/
#define BATT_CMP        0x80//Battery Compensation
#define VP_MONITOR      0x40//VP Monitor
/** Speaker Status(Read Only) ******************************/
#define SPKA_SHRT       0x20//Speaker A Current Load Status
#define SPKB_SHRT       0x10//Speaker B Current Load Status
#define SPKR_HP         0x08//SPKR/HP Pin Status

#define START_VOLUME 60


enum beep_freq
{
  freq260  = 0x00,//C4
  freq520  = 0x10,//C5
  freq585  = 0x20,//D5
  freq666  = 0x30,//E5
  freq705  = 0x40,//F5
  freq774  = 0x50,//G5
  freq888  = 0x60,//A5
  freq1000 = 0x70,//B5
  freq1043 = 0x80,//C6
  freq1200 = 0x90,//D6
  freq1333 = 0xA0,//E6
  freq1411 = 0xB0,//F6
  freq1600 = 0xC0,//G6
  freq1714 = 0xD0,//A6
  freq2000 = 0xE0,//B6
  freq2181 = 0xF0 //C7
}beep_freq;

enum beep_on
{
  on_86ms,
  on_430ms,
  on_780ms,
  on_1s2,
  on_1s5,
  on_1s8,
  on_2s2,
  on_2s5,
  on_2s8,
  on_3s2,
  on_3s5,
  on_3s8,
  on_4s2,
  on_4s5,
  on_4s8,
  on_5s2,
  cont_mode
};

enum beep_off
{
  off_1s23    = 0x00,
  off_2s58    = 0x20,
  off_3s90    = 0x40,
  off_5s20    = 0x60,
  off_6s60    = 0x80,
  off_8s05    = 0xA0,
  off_9s35    = 0xC0,
  off_10s80   = 0xE0,
  single_mode = 0xF0
};

void Get_Audiochip_ID(void);
void Audio_Reset_Init();
void Beep_Start(uint8_t beep_freq, uint8_t beep_on, uint8_t beep_off);
void Beep_Stop();
void Beep_Set();
void AnalogWave_Init(uint16_t frequency);
void AnalogWave_Start();
void AnalogWave_Stop();
void AnalogWave_Set(uint16_t frequency);

void SetVolume(void);

/**************************************************************/

uint32_t EVAL_AUDIO_Init(uint8_t volume);
void EVAL_AUDIO_Play(void);
uint32_t EVAL_AUDIO_PauseResume(uint32_t Cmd);
void EVAL_AUDIO_Stop(uint32_t Option);
void EVAL_AUDIO_StopDMA(void);
uint32_t EVAL_AUDIO_VolumeCtl(uint8_t volume);
void EVAL_IRQ_ENABLE(void);
void EVAL_IRQ_DISABLE(void);
void EVAL_SET_DMA(int next_puffer_len, int16_t *next_samples);


//--------------------------------------------------------------
void EVAL_AUDIO_TransferComplete_CallBack(void);
void Beep_Handler();

 
#endif // __CS43L22_H


