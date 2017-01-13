/*
 * dac.h
 *
 *  Created on: 6.03.2016
 *      Author: Aleksey Totkal
 */

#ifndef USER_DAC_H_
#define USER_DAC_H_

#include "user.h"


#define   USE_DAC1_DMA
#define   SINE_RES          128                                  // Waveform resolution

static const uint16_t sin_table[SINE_RES] = { 2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897,
                                      2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640,
                                      3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002,
                                      4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894,
                                      3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344,
                                      3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483,
                                      2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517,
                                      1424, 1332, 1242, 1154, 1068, 985, 904, 826, 751, 679,
                                      610, 545, 484, 426, 373, 323, 278, 237, 201, 169,
                                      141, 118, 100, 87, 79, 75, 76, 82, 93, 109,
                                      129, 154, 184, 218, 257, 300, 347, 399, 455, 514,
                                      577, 644, 714, 788, 865, 944, 1026, 1111, 1198, 1287,
                                      1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047 };


//static uint8_t i = 0;

#define DAC_DATA        0
#define DAC_TRIANGLE    1
#define DAC_NOISE       2
/** uncomment corresponding string */
/** select DAC1 mode               */
#define DAC1_MODE DAC_DATA
//#define DAC1_MODE DAC_TRIANGLE
//#define DAC1_MODE DAC_NOISE
/** select DAC1 event source       */
#define DAC1_TIM6_EVENT
//#define DAC1_SWTRIG

void DAC1_Init();
void DAC1_DMA_Init(void);
void DAC1_Handler();


#endif /* USER_DAC_H_ */
