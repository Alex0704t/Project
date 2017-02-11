/*
 * accelero.h
 *
 *  Created on: 05.07.2015
 *      Author: Aleksey
 */

#ifndef USER_ACCELERO_H_
#define USER_ACCELERO_H_

#include "user.h"
#include "spi.h"
#include "led.h"
#include "stm32f4xx.h"

typedef struct axis{int16_t x;
                    int16_t y;
                    int16_t z;
                   }axis_s;

typedef struct real_axis{double x;
                         double y;
                         double z;
                        }real_axis_s;


#define LIS3DSH_CS_ON()                   GPIOE->BSRR = GPIO_BSRR_BR_3
#define LIS3DSH_CS_OFF()                  GPIOE->BSRR = GPIO_BSRR_BS_3
#define DUMMY                             0x00
#define WHO_I_AM                          0x0F
#define I_AM_LIS3DSH                      0x3F
/*CTRL_REG1***********************************/
#define CTRL_REG1                         0x21
#define CTRL_REG1_SM1_EN                  0x01//SM1 enable
#define CTRL_REG1_SM1_PIN                 0x08//INT2 signal SM1
#define CTRL_REG1_HYST_MASK               0xE0//max hysteresis value for SM1
#define CTRL_REG1_HYST0_1                 0x20//hysteresis value for SM1
#define CTRL_REG1_HYST1_1                 0x40//hysteresis value for SM1
#define CTRL_REG1_HYST2_1                 0x80//hysteresis value for SM1
/*CTRL_REG3***********************************/
#define CTRL_REG3                         0x23
#define CTRL_REG3_STRT                    0x01//soft reset
#define CTRL_REG3_VFILT                   0x04//vector filter enable
#define CTRL_REG3_INT1_EN                 0x08//INT1/DRDY signal enable
#define CTRL_REG3_INT2_EN                 0x10//INT2 signal enable
#define CTRL_REG3_IEL                     0x20//INT signal pulsed
#define CTRL_REG3_IEA                     0x40//INT signal active HIGH
#define CTRL_REG3_DR_EN                   0x80//DRDY signal connected to INT1
/*CTRL_REG4***********************************/
#define CTRL_REG4                         0x20
#define CTRL_REG4_ODR_MASK                0xF0
#define CTRL_REG4_ODR_PW_DOWN             0x00
#define CTRL_REG4_ODR_3                   0x10//3.125 Hz
#define CTRL_REG4_ODR_6                   0x20//6.25 Hz
#define CTRL_REG4_ODR_12                  0x30//12.5 Hz
#define CTRL_REG4_ODR_25                  0x40//25 Hz
#define CTRL_REG4_ODR_50                  0x50//50 Hz
#define CTRL_REG4_ODR_100                 0x60//100 Hz
#define CTRL_REG4_ODR_400                 0x70//400 Hz
#define CTRL_REG4_ODR_800                 0x80//800 Hz
#define CTRL_REG4_ODR_1600                0x90//1600 Hz
#define CTRL_REG4_AX_EN_MASK              0x07//axis enable mask
#define CTRL_REG4_XYZ                     0x07//xyz-axes
#define CTRL_REG4_XY                      0x03// xy-axes
/*CTRL_REG5***********************************/
#define CTRL_REG5                         0x24
#define CTRL_REG5_SIM                     0x01//3-wire mode enable
#define CTRL_REG5_ST1                     0x02//positive self-test
#define CTRL_REG5_ST2                     0x04//negative self-test
#define CTRL_REG5_FSCALE_MASK             0x38
#define CTRL_REG5_FSCALE_2G               0x00//default value 2G
#define CTRL_REG5_FSCALE_4G               0x08
#define CTRL_REG5_FSCALE_6G               0x10
#define CTRL_REG5_FSCALE_8G               0x18
#define CTRL_REG5_FSCALE_16G              0x20
#define CTRL_REG5_BW_MASK                 0xC0//Anti-aliasing filter bandwidth.
#define CTRL_REG5_BW_800                  0x00//Default value: 800 Hz
#define CTRL_REG5_BW_400                  0x40//400 Hz
#define CTRL_REG5_BW_200                  0x80//200 Hz
#define CTRL_REG5_BW_50                   0xC0//50 Hz
/*SETT**************************************/
#define SETT1                             0x5B
#define SETT_SITR                         0x01//program flow can be modified by STOP and CONT commands
#define SETT_R_TAM                        0x02//valid next condition found and reset
#define SETT_THR3_MA                      0x04//threshold 3 limit value for axis and sign mask reset (MASKA_1)
#define SETT_ABS                          0x20//signed thresholds
#define SETT_THR3_SA                      0x40//threshold 3 limit value for axis and sign mask reset (MASKB_1)
#define SETT_P_DET                        0x80//peak detection enabled
/*MASK registers*****************************/
#define MASK1_B                           0x59//mask(swap) for SM1
#define MASK1_A                           0x5A//mask(default) for SM1
#define MASK2_B                           0x79//mask(swap) for SM2
#define MASK2_A                           0x7A//mask(default) for SM2
#define MASK_N_V                          0x01
#define MASK_P_V                          0x02
#define MASK_N_Z                          0x04
#define MASK_P_Z                          0x08
#define MASK_N_Y                          0x10
#define MASK_P_Y                          0x20
#define MASK_P_X                          0x40
#define MASK_N_X                          0x80
#define MASK_XY                           0xF0
#define MASK_XYZ                          0xFC
/*STATUS REGISTER****************************/
#define STAT_REG                          0x27
#define STAT_ZYXOR                        0x80//X-, Y- and Z-axis data overrun
#define STAT_ZOR                          0x40//Z-axis data overrun
#define STAT_YOR                          0x20//Y-axis data overrun
#define STAT_XOR                          0x10//X-axis data overrun
#define STAT_ZYXDA                        0x08//X-, Y- and Z-axis new data available
#define STAT_ZDA                          0x04//Z-axis new data available
#define STAT_YDA                          0x04//Y-axis new data available
#define STAT_XDA                          0x04//X-axis new data available
/*OUT_DATA***********************************/
#define OUT_DATA                          0x28
/*THRS1_1************threshold value for SM1*/
#define THRS2_1                           0x56
/*THRS1_1************threshold value for SM1*/
#define THRS1_1                           0x57
/*STx_1*********state machine code registers*/
#define ST1_1                             0x40
#define ST2_1                             0x41
#define ST3_1                             0x42
#define ST4_1                             0x43
#define ST5_1                             0x44
#define ST6_1                             0x45
#define ST7_1                             0x46
#define ST8_1                             0x47
#define ST9_1                             0x48
#define ST10_1                            0x49
#define ST11_1                            0x4A
#define ST12_1                            0x4B
#define ST13_1                            0x4C
#define ST14_1                            0x4D
#define ST15_1                            0x4E
#define ST16_1                            0x4F
/*Conditions*********************************/
#define NOP                               0x00//no operation
#define TI1                               0x01//Timer 1 (16-bit value) valid
#define TI2                               0x02//Timer 2 (16-bit value) valid
#define TI3                               0x03//Timer 3 (8-bit value) valid
#define TI4                               0x04//Timer 4 (8-bit value) valid
#define GNTH1                             0x05//Any/triggered axis greater than THRS1
#define GNTH2                             0x06//Any/triggered axis greater than THRS2
#define LNTH1                             0x07//Any/triggered axis less than or equal to THRS1
#define LNTH2                             0x08//Any/triggered axis less than or equal to THRS2
#define GTTH1                             0x09//Any/triggered axis greater than THRS1 but using always standard axis mask(MASK1)
#define LLTH2                             0x0A//All axis less than or equal to THRS2
#define GRTH1                             0x0B//Any/triggered axis greater than reversed THRS1
#define LRTH1                             0x0C//Any/triggered axis less than or equal to reversed THRS1
#define GRTH2                             0x0D//Any/triggered axis greater than reversed THRS2
#define LRTH2                             0x0E//Any/triggered axis less than or equal to reversed THRS2
#define NZERO                             0x0F//Any axis zero crossed
/*Commands***********************************/
#define STOP                              0x00
#define CONT                              0x11
#define JMP                               0x22
#define SRP                               0x33
#define CRP                               0x44
#define SETP                              0x55
#define SETS1                             0x66
#define STHR1                             0x77
#define OUTC                              0x88
#define OUTW                              0x99
#define STHR2                             0xAA
#define DEC                               0xBB
#define SISW                              0xCC
#define REL                               0xDD
#define STHR3                             0xEE
#define SSYNC                             0xFF
#define SABS0                             0x12
#define SABS1                             0x13
#define SELMA                             0x14
#define SRADI0                            0x21//Only for State Machine 2
#define SRADI1                            0x23//Only for State Machine 2
#define SELSA                             0x24
#define SCS0                              0x31//Only for State Machine 2
#define SCS1                              0x32//Only for State Machine 2
#define SRTAM0                            0x34
#define STIM3                             0x41
#define STIM4                             0x42
#define SRTAM1                            0xBB

typedef enum {
  SCALE_2G  = CTRL_REG5_FSCALE_2G,
  SCALE_4G  = CTRL_REG5_FSCALE_4G,
  SCALE_6G  = CTRL_REG5_FSCALE_6G,
  SCALE_8G  = CTRL_REG5_FSCALE_8G,
  SCALE_16G = CTRL_REG5_FSCALE_16G
}lis3dsh_scale;

typedef enum {
  ODR_power_down  = CTRL_REG4_ODR_PW_DOWN,
  ODR_3_125Hz     = CTRL_REG4_ODR_3,
  ODR_6_25Hz      = CTRL_REG4_ODR_6,
  ODR_12_5Hz      = CTRL_REG4_ODR_12,
  ODR_25Hz        = CTRL_REG4_ODR_25,
  ODR_50Hz        = CTRL_REG4_ODR_50,
  ODR_100Hz       = CTRL_REG4_ODR_100,
  ODR_400Hz       = CTRL_REG4_ODR_400,
  ODR_800Hz       = CTRL_REG4_ODR_800,
  ODR_1600Hz      = CTRL_REG4_ODR_1600
}lis3dsh_data_rate;

/** uncomment for select only X & Y axis  */
//#define USE_2AXIS
#ifdef USE_2AXIS
  #define CTRL_REG4_AXIS CTRL_REG4_XY
#else
  #define CTRL_REG4_AXIS CTRL_REG4_XYZ
#endif

void LIS3DSH_CS_Init(void);
void LIS3DSH_Init(void);
void LIS3DSH_En(lis3dsh_scale scale, lis3dsh_data_rate rate);
void LIS3DSH_Int2_Init(void);
axis_s LIS3DSH_Decode(uint8_t *data);
real_axis_s LIS3DSH_Convert(axis_s *data, lis3dsh_scale scale);
void LIS3DSH_Led(const axis_s *in, int32_t threshold);
void LIS3DSH_MovDetEn(lis3dsh_scale scale, lis3dsh_data_rate rate);
void LIS3DSH_View();
void LIS3DSH_IntHandler();


#endif /* USER_ACCELERO_H_ */
