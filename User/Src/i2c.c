/*
 * i2c.c
 *
 *  Created on: 23.10.2015
 *      Author: Aleksey Totkal
 */

#include "i2c.h"

void I2C1_Init(void)
{
  /*
   * PB9 - Audio SDA
   * PB6 - Audio SDL
   */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;//I2C clock enable
  GPIOB->MODER |= GPIO_MODER_MODER6_1|GPIO_MODER_MODER9_1;//PB6, 9 alternative function
  GPIOB->OTYPER |= GPIO_OTYPER_OT_6|GPIO_OTYPER_OT_9;//open drain
  GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6|GPIO_PUPDR_PUPDR9);//no pull-up & pull-down
  GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_1|GPIO_OSPEEDER_OSPEEDR9_1;//High speed
  GPIOB->AFR[0] |= 0x04000000;//I2C1 AF4
  GPIOB->AFR[1] |= 0x00000040;//I2C1 AF4

  //I2C1->CR2 &= ~I2C_CR2_FREQ;
  //I2C1->CR2 |= 0x2;//Peripheral clock frequency 2 MHz
  I2C1->CR1 &= ~(I2C_CR1_PE|I2C_CR1_SMBUS);//Peripheral disable, I2C mode
  I2C1->CR1 = I2C_CR1_SWRST;
  I2C1->CR1 = 0;
  I2C1->CR2 = 42;
  //I2C1->CCR &= ~I2C_CCR_CCR;
  //I2C1->CCR |= 0x10;
  I2C1->CCR |= 210;//100 kHz SCL frequency
  I2C1->TRISE = 43;
  //I2C1->CR1 &= ~(I2C_CR1_PE|I2C_CR1_SMBUS);//Peripheral disable, I2C mode
  I2C1->OAR1 = 0x33;//I2C_OwnAddress1 = 0x33, 7-bit slave address
  I2C1->CR1 |= I2C_CR1_ACK;//Acknowledge Enable
  I2C1->CR1 |= I2C_CR1_PE;//I2C1 enable
/*
  NVIC_SetPriorityGrouping(4);//4 field for priority group
  NVIC_SetPriority(I2C1_EV_IRQn, 4);
  NVIC_EnableIRQ(I2C1_EV_IRQn);//IRQ handler
  NVIC_SetPriority(I2C1_ER_IRQn, 4);
  NVIC_EnableIRQ(I2C1_ER_IRQn);//IRQ handler*/
}
/*
void I2C1_EV_IRQHandler(void)
{

}

void I2C1_ER_IRQHandler(void)
{

}
*/

void I2C1_DeInit(void)
{
  RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
  RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
}

//--------------------------------------------------------------
// internal function
// it is called with Timeout
// Stop, Reset and reinit of the I2C interface
//--------------------------------------------------------------
int16_t I2C1_Timeout(void)
{
  I2C1_STOP;//send Stopbit
  I2C1_SWR_RST;//reset I2C
  I2C1_DeInit();//reset
  I2C1_Init();
  return 1;
}

//--------------------------------------------------------------
// Describe to an address by I2C of a Slave
// slave_adr => I2C-Basis-address of the slave
// addr      => Register address is described
// wert      => Byte value is written
//
// Return value :
//    0   , Ok
//    1   , Error
//--------------------------------------------------------------
int16_t I2C1_WriteByte(uint8_t addr, uint8_t data)
{
  I2C1_START;//send Startbit
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_SB);
  I2C1_ADDR_W;//send slave address(write)
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_ADDR);
  I2C1_CLEAR_FLAGS;//clear SR1 & SR2
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_TXE);
  I2C1_SEND_DATA(addr);//send address
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_TXE);
  I2C1_SEND_DATA(data);//send data
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_TXE);
  I2C1_STOP;//send Stopbit
  return 0;//all ok
}

//--------------------------------------------------------------
// Selections of an address by I2C of a Slave
// slave_adr => I2C base address of the Slave
// addr      => Register address it is read
//
// Return value :
//  0...255 , Byte value to him was read
//  < 0     , Error
//--------------------------------------------------------------

int16_t I2C1_ReadByte(uint8_t slave_addr, uint8_t addr)
{
  int16_t ret = 0;
  I2C1_START;//send Startbit
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_SB);
  I2C1_CLEAR_ACK;//ACK disable
  I2C1_ADDR_W;//send slave address(write)
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_ADDR);
  I2C1_CLEAR_FLAGS;//clear SR1 & SR2
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_TXE);
  I2C1_SEND_DATA(addr);//send address
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_TXE);
  I2C1_START;//send Startbit
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_SB);
  I2C1_ADDR_R;//send slave address(read)
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_ADDR);
  I2C1_CLEAR_FLAGS;//clear SR1 & SR2
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_RXNE);
  I2C1_STOP;//send Stopbit
  ret = (int16_t)I2C1->DR;
  I2C1_SET_ACK;I2C1_CLEAR_ACK;//ACK enable
  return ret;
}


uint8_t Transmit_I2C1(uint8_t *data, uint8_t size)
{
  I2C1_WAIT_FOR(!(I2C1->SR2 & I2C_SR2_BUSY));
  I2C1_CLEAR_POS;
  I2C1_START;//send startbit
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_SB);
  I2C1_ADDR_W;//send slave address(transmit mode)
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_ADDR);
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_TXE);
  I2C1_CLEAR_FLAGS;
  while(size--)
    {
      I2C1->DR = (*data++);
      I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_TXE);
    }
  I2C1_STOP;
  return 0;
}

uint8_t Receive_I2C1(uint8_t *data, uint8_t size)
{
  I2C1_WAIT_FOR(!(I2C1->SR2 & I2C_SR2_BUSY));
  I2C1_CLEAR_POS;
  I2C1_START;//send startbit
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_SB);
  I2C1_ADDR_R;//send slave address (receive mode)
  I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_ADDR);
  if(size == 1)
    {
      I2C1_CLEAR_ACK;
      I2C1_CLEAR_FLAGS;
      I2C1_STOP;
    }
  else if(size == 2)
    {
      I2C1_CLEAR_ACK;
      I2C1_SET_POS;
      I2C1_CLEAR_FLAGS;
    }
  else
    {
      I2C1_SET_ACK;
      I2C1_CLEAR_FLAGS;
    }
   while(size--)
     {
       if(size <= 3)
         {
           if(size == 1)
             {
               I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_RXNE);
               (*data++) = I2C1->DR;
               size--;
             }
           else if(size == 2)
             {
               I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_BTF);
               I2C1_STOP;
               (*data++) = I2C1->DR;
               size--;
               (*data++) = I2C1->DR;
               size--;
             }
           else
             {
               I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_BTF);
               I2C1_CLEAR_ACK;
               (*data++) = I2C1->DR;
               size--;
               I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_BTF);
               I2C1_STOP;
               (*data++) = I2C1->DR;
               size--;
               (*data++) = I2C1->DR;
               size--;
             }
         }
       else
         {
           I2C1_WAIT_FOR(I2C1->SR1 & I2C_SR1_RXNE);
           (*data++) = I2C1->DR;
           size--;
           if(I2C1->SR1 & I2C_SR1_BTF)
             {
               (*data++) = I2C1->DR;
               size--;
             }
         }
     }
  return 0;
}
