/*
 * spi.c
 *
 *  Created on: 05.06.2015
 *      Author: Aleksey
 */

#include "spi.h"

uint8_t *pTx_SPI1_data, *pRx_SPI1_data;

void SPI1_Init(void) {
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	/*
	 * PA5 - SPI1_SCK
	 * PA6 - SPI1_MISO
	 * PA7 - SPI1_MOSI
	 */
	GPIOA->MODER |= 0x0000A800;//alternate function mode
  GPIOA->OSPEEDR |= 0x0000A800;//high speed
//	GPIOA->PUPDR &= ~0x0000FC00;//no pull-up & pull-down
	GPIOA->PUPDR |= 0x0000A800;//pull-down

//  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR6;//clear PA6 PUPDR
//	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR6_0;//PA6 pull-up(MISO for SD card)
	GPIOA->AFR[0] |= 0x55500000;//SPI1 alternate function AF5

	SPI1->CR1 = 0;//clear CR1 register
	/**
 * 000 Fpclk/2
 * 001 Fpclk/4
 * 010 Fpclk/8
 * 011 Fpclk/16
 * 100 Fpclk/32
 * 101 Fpclk/64
 * 110 Fpclk/128
 * 111 Fpclk/256
 */
/** select baudrate ***********************************************************/
	SPI1->CR1 |= SPI_CR1_BR_2;
//	SPI1->CR1 |= SPI_CR1_BR_1;
	SPI1->CR1 |= SPI_CR1_BR_0;
/** select clock phase & polarity *********************************************/
//	SPI1->CR1 |= SPI_CR1_CPOL;//CPOL = 1
//	SPI1->CR1 |= SPI_CR1_CPHA;//CPHA = 1
/** select 8 bit or 16 bit frame **********************************************/
//	SPI1->CR1 |= SPI_CR1_DFF;//frame 16bit
/** select MSB or LSB transmitted first ***************************************/
//	SPI1->CR1 |= SPI_CR1_LSBFIRST;//LSB transmitted first
/** select slave or master mode ***********************************************/
	SPI1->CR1 |= SPI_CR1_MSTR;//master mode
/** select 2 or 1 data line mode **********************************************/
//	SPI1->CR1 |= SPI_CR1_BIDIMODE;//1 line mode
/** receive or transmit enable ************************************************/
//  SPI1->CR1 |= SPI_CR1_BIDIOE;//output enable (transmit only)
/** enable slave software slave management ************************************/
	SPI1->CR1 |= SPI_CR1_SSM;//software slave management enable
/** enable external slave select **********************************************/
	SPI1->CR1 |= SPI_CR1_SSI;//external slave select
/** Tx buffer empty interrupt enable ******************************************/
//	SPI1->CR2 |= SPI_CR2_TXEIE;
/** Rx buffer not empty interrupt enable **************************************/
//  SPI1->CR2 |= SPI_CR2_RXNEIE;

//  NVIC_SetPriority(SPI1_IRQn, 4);
//  NVIC_EnableIRQ(SPI1_IRQn);//IRQ handler

	SPI1->CR1 |= SPI_CR1_SPE;//SPI enable



}

void SPI1_DeInit(void) {
  RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;//disable SPI1 clock
  SPI1->CR1 &= ~SPI_CR1_SPE;//SPI1 disable
  NVIC_DisableIRQ(SPI1_IRQn);//disable IRQ handler
}

uint8_t SPI1_TxRxByte(uint8_t data) {
  uint8_t temp;
  while (!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = data;
  while (!(SPI1->SR & SPI_SR_RXNE));
  temp = SPI1->DR;
  while (!(SPI1->SR & SPI_SR_TXE));
  while ((SPI1->SR & SPI_SR_BSY));
  return temp;
}

void SPI1_Rx(uint8_t *data, uint16_t size) {
  while (!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = 0xFF;
  while (size--) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = 0xFF;
    while (!(SPI1->SR & SPI_SR_RXNE));
    *data++ = SPI1->DR;
  }
}

void SPI1_Tx(uint8_t *data, uint16_t size) {
  while (size--) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = *data++;
  }
  while (!(SPI1->SR & SPI_SR_TXE));
  while ((SPI1->SR & SPI_SR_BSY));
}

void SPI1_TxRx(uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
#if 1
  while (!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = *tx_data;
  while (--size) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = *(++tx_data);
    while (!(SPI1->SR & SPI_SR_RXNE));
    *rx_data++ = SPI1->DR;
  }
  while (!(SPI1->SR & SPI_SR_RXNE));
  *rx_data = SPI1->DR;
  while (!(SPI1->SR & SPI_SR_TXE));
  while ((SPI1->SR & SPI_SR_BSY));
#else
//    if(!tx_data)
//      for(uint8_t i = 0; i < size; i++)
//        rx_data[i] = SPI1_TxRxByte(0xFF);
//    else if(!rx_data)
//      for(uint8_t i = 0; i < size; i++)
//        SPI1_TxRxByte(tx_data[i]);
//    else
//      for(uint8_t i = 0; i < size; i++) {
//        rx_data[i] = SPI1_TxRxByte(tx_data[i]);
//      }

  while(size--) {
     *rx_data++ = SPI1_TxRxByte(*tx_data++);
//     rx_data++;
//     tx_data++;
//     size--;
  }
#endif
}

#if 0
/**
  * @brief  Transmit and Receive an amount of data in blocking mode.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @param  Size: amount of data to be sent and received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
uint8_t SPI1_TxRx(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
#if (USE_SPI_CRC != 0U)
  __IO uint16_t tmpreg1 = 0U;
#endif /* USE_SPI_CRC */
  uint32_t tickstart = 0U;
  /* Variable used to alternate Rx and Tx during transfer */
  uint32_t txallowed = 1U;
  uint8_t errorcode = 0;

  /* Init tickstart for timeout management*/
  tickstart = GetTick();


  /* Set the transaction information */
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pRxBuffPtr  = (uint8_t *)pRxData;
  hspi->RxXferCount = Size;
  hspi->RxXferSize  = Size;
  hspi->pTxBuffPtr  = (uint8_t *)pTxData;
  hspi->TxXferCount = Size;
  hspi->TxXferSize  = Size;

  /*Init field not used in handle to zero */
  hspi->RxISR       = NULL;
  hspi->TxISR       = NULL;

#if (USE_SPI_CRC != 0U)
  /* Reset CRC Calculation */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    SPI_RESET_CRC(hspi);
  }
#endif /* USE_SPI_CRC */

    while((hspi->TxXferCount > 0U) || (hspi->RxXferCount > 0U))
    {
      /* check TXE flag */
      if(txallowed && (hspi->TxXferCount > 0U) && (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE)))
      {
        *(__IO uint8_t *)&SPI1->DR = (*pTxData++);
        hspi->TxXferCount--;
        /* Next Data is a reception (Rx). Tx not allowed */
        txallowed = 0U;

#if (USE_SPI_CRC != 0U)
        /* Enable CRC Transmission */
        if((hspi->TxXferCount == 0U) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE))
        {
          SET_BIT(SPI1->CR1, SPI_CR1_CRCNEXT);
        }
#endif /* USE_SPI_CRC */
      }

      /* Wait until RXNE flag is reset */
      if((hspi->RxXferCount > 0U) && (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE)))
      {
        (*(uint8_t *)pRxData++) = SPI1->DR;
        hspi->RxXferCount--;
        /* Next Data is a Transmission (Tx). Tx is allowed */
        txallowed = 1U;
      }
      if((Timeout != HAL_MAX_DELAY) && ((HAL_GetTick()-tickstart) >=  Timeout))
      {
          return 1;
      }
    }

#if (USE_SPI_CRC != 0U)
  /* Read CRC from DR to close CRC calculation process */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    /* Wait until TXE flag */
    if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SET, Timeout, tickstart) != HAL_OK)
    {
      /* Error on the CRC reception */
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_CRC);
      return 1;
    }
    /* Read CRC */
    tmpreg1 = SPI1->DR;
    /* To avoid GCC warning */
    UNUSED(tmpreg1);
  }

  /* Check if CRC error occurred */
  if(__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR))
  {
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_CRC);
    /* Clear CRC Flag */
    __HAL_SPI_CLEAR_CRCERRFLAG(hspi);

    errorcode = 1;
  }
#endif /* USE_SPI_CRC */

  /* Wait until TXE flag */
  if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_TXE, SET, Timeout, tickstart) != HAL_OK)
  {
      return 1;
  }

  /* Check Busy flag */
  if(SPI_CheckFlag_BSY(hspi, Timeout, tickstart) != HAL_OK)
  {
    return 1;
  }

  return errorcode;
}
#endif

void LIS3DSH_Write(uint8_t addr, uint8_t *data, uint8_t size) {
#if 1
  uint8_t temp = 0;
  LIS3DSH_CS_ON();
  while (!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = addr;
  while (!(SPI1->SR & SPI_SR_RXNE));
  temp = SPI1->DR;
  while (size--) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = *data++;
    while (!(SPI1->SR & SPI_SR_RXNE));
    temp = SPI1->DR;
  }
  LIS3DSH_CS_OFF();
#else
  LIS3DSH_CS_ON();
  SPI1_TxRxByte(addr);
  while (size--) {
    SPI1_TxRxByte(*data++);
  }
  LIS3DSH_CS_OFF();
#endif
}

void LIS3DSH_Read(uint8_t addr, uint8_t *data, uint8_t size) {
#if 1
  uint8_t temp = 0;
  addr |= 0x80;
  LIS3DSH_CS_ON();
  while (!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = addr;
//  while (!(SPI1->SR & SPI_SR_RXNE));
  temp = SPI1->DR;
  while (size--) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = 0xFF;
    while (!(SPI1->SR & SPI_SR_RXNE));
    *data++ = SPI1->DR;
  }
  LIS3DSH_CS_OFF();
#else
  LIS3DSH_CS_ON();
  SPI1_TxRxByte(addr|0x80);
  while (size--) {
      *data++ = SPI1_TxRxByte(0xFF);
  }
  LIS3DSH_CS_OFF();
#endif
}

void LIS3DSH_WriteReg(uint8_t addr, uint8_t value) {
  LIS3DSH_Write(addr, &value, 1);
}

uint8_t LIS3DSH_ReadReg(uint8_t addr) {
  uint8_t data = 0;
  LIS3DSH_Read(addr, &data, 1);
  return data;
}

void LIS3DSH_SetReg(uint8_t addr, uint8_t value) {
  uint8_t temp = LIS3DSH_ReadReg(addr);
  temp |= value;
  LIS3DSH_Write(addr, &temp, 1);
}

void LIS3DSH_ClearReg(uint8_t addr) {
  LIS3DSH_Write(addr, (uint8_t*)0x00, 1);
}

void LIS3DSH_ModReg(uint8_t addr, uint8_t mask, uint8_t value) {
  uint8_t temp = LIS3DSH_ReadReg(addr);
  temp &= ~mask;
  temp |= value;
  LIS3DSH_Write(addr, &temp, 1);
}

uint8_t LIS3DSH_CheckReg(uint8_t addr, uint8_t value) {
  uint8_t data = 0;
  LIS3DSH_Read(addr, &data, 1);
  if ((data & value) == value)
    return SET;
  else
    return RESET;
}

extern uint8_t  SPI1_DMA_mode_flag;

void SPI1_DMA_Init(void) {
	SPI1_Init();
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;//DMA2 enable
	/** SPI1 TX DMA2 Stream_3 channel_3*/
	DMA2_Stream3->CR = 0;
	DMA2_Stream3->FCR = 0;
	DMA2_Stream3->CR |= DMA_SxCR_CHSEL_0|DMA_SxCR_CHSEL_1;//channel 3 select
	DMA2_Stream3->CR |= DMA_SxCR_PL_0|DMA_SxCR_PL_1;//very high priority
	DMA2_Stream3->CR &= ~(DMA_SxCR_MSIZE|DMA_SxCR_PSIZE);//memory & peripheral data size 1 byte
	DMA2_Stream3->CR |= DMA_SxCR_MINC;//memory increment mode
	DMA2_Stream3->CR |= DMA_SxCR_DIR_0;//memory to peripheral direction
	DMA2_Stream3->CR |= DMA_SxCR_TCIE;//transfer complete interrupt enable
	DMA2_Stream3->PAR = (uint32_t)&(SPI1->DR);//peripheral address
//	DMA2_Stream3->M0AR = (uint32_t)(lis3dsh_tx_buf);//memory address
//	DMA2_Stream3->NDTR = LIS3DSH_BUFSIZE;//data size
	SPI1->CR2 |= SPI_CR2_TXDMAEN;//Tx buffer DMA enable
	/** SPI1 RX DMA2 Stream_0 channel_3*/
	DMA2_Stream0->CR = 0;
	DMA2_Stream0->FCR = 0;
	DMA2_Stream0->CR |= DMA_SxCR_CHSEL_0|DMA_SxCR_CHSEL_1;//channel 3 select
	DMA2_Stream0->CR |= DMA_SxCR_PL_0|DMA_SxCR_PL_1;//very high priority
	DMA2_Stream0->CR &= ~(DMA_SxCR_MSIZE|DMA_SxCR_PSIZE);//memory & peripheral data size 1 byte
	DMA2_Stream0->CR |= DMA_SxCR_MINC;//memory increment mode
	DMA2_Stream0->CR &= ~DMA_SxCR_DIR;//peripheral to memory direction
	DMA2_Stream0->CR |= DMA_SxCR_TCIE;//transfer complete interrupt enable
	DMA2_Stream0->PAR = (uint32_t)&(SPI1->DR);//peripheral address
//	DMA2_Stream0->M0AR = (uint32_t)(lis3dsh_rx_buf);//memory address
//	DMA2_Stream0->NDTR = LIS3DSH_BUFSIZE;//data size
	SPI1->CR2 |= SPI_CR2_RXDMAEN;//Rx buffer DMA enable

	NVIC_SetPriority(DMA2_Stream3_IRQn, 4);
	NVIC_EnableIRQ(DMA2_Stream3_IRQn);//IRQ handler
	NVIC_SetPriority(DMA2_Stream0_IRQn, 4);
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);//IRQ handler
}

void SPI1_DMA_DeInit(void) {
  SPI1->CR2 &= ~SPI_CR2_TXDMAEN;//Tx buffer DMA disable
  SPI1->CR2 &= ~SPI_CR2_RXDMAEN;//Rx buffer DMA disable
  NVIC_EnableIRQ(DMA2_Stream3_IRQn);//disable IRQ handler
  NVIC_EnableIRQ(DMA2_Stream0_IRQn);//disable IRQ handler
  SPI1_DeInit();
}

__IO uint8_t SPI1_data_ready;
__IO uint8_t lis3dsh_ax_data[LIS3DSH_BUFSIZE];
uint8_t lis3dsh_tx_dummy[LIS3DSH_BUFSIZE] = {OUT_DATA|0x80, 0, 0, 0, 0, 0, 0};


void SPI1_DMA_TxRx(uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
//  uint8_t tx_buf[size], rx_buf[size];
//  tx_data = tx_buf;
//  rx_data = rx_buf;
//  for (uint8_t i = 0; i < size; i++) {
//     tx_buf[i] = tx_data[i];
//  }

    DMA2_Stream0->M0AR = (uint32_t)(rx_data);//memory address
    DMA2_Stream0->NDTR = size;//data size
    DMA2_Stream3->M0AR = (uint32_t)(tx_data);//memory address
    DMA2_Stream3->NDTR = size;//data size

//  if ((SPI1_DMA_mode_flag & SPI1_DMA_LIS3DSH) != SPI1_DMA_LIS3DSH)
//    SPI1_DMA_mode_flag = SPI1_DMA_LIS3DSH;//set DMA mode flag if need
  SPI1_ResetFlag();//
//  LIS3DSH_CS_ON();
    DMA2_Stream0->CR |= DMA_SxCR_EN;//stream enable
    DMA2_Stream3->CR |= DMA_SxCR_EN;//stream enable
  SPI1_WaitFlag();
//  for (uint8_t i = 0; i < size; i++) {
//     rx_data[i] = rx_buf[i];
//  }
}

void LIS3DSH_GetAxis() {
	DMA2_Stream0->M0AR = (uint32_t)(lis3dsh_ax_data);//memory address
	DMA2_Stream0->NDTR = LIS3DSH_BUFSIZE;//data size
	DMA2_Stream3->M0AR = (uint32_t)(lis3dsh_tx_dummy);//memory address
	DMA2_Stream3->NDTR = LIS3DSH_BUFSIZE;//data size
	if ((SPI1_DMA_mode_flag & SPI1_DMA_LIS3DSH) != SPI1_DMA_LIS3DSH)
	  SPI1_DMA_mode_flag = SPI1_DMA_LIS3DSH;//set DMA mode flag if need
	SPI1_ResetFlag();//
	LIS3DSH_CS_ON();
  DMA2_Stream0->CR |= DMA_SxCR_EN;//stream enable
	DMA2_Stream3->CR |= DMA_SxCR_EN;//stream enable
}

void LIS3DSH_GetData(uint8_t *data) {
  for (uint8_t i = 1; i < LIS3DSH_BUFSIZE; i++)
    data[i - 1] = lis3dsh_ax_data[i];
//  data = lis3dsh_ax_data + 1;
}

void SPI1_SetFlag() {
  SPI1_data_ready = 1;
}

void SPI1_ResetFlag() {
  SPI1_data_ready = 0;
}

void SPI1_WaitFlag() {
  while (!SPI1_data_ready);
}

extern volatile char PCF8812_buff[PCF8812_BUFSIZ];

void SPI2_Init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	/*
	 * PB13 - SPI2_SCK Pin2 LCD (SCLK)
	 * PB15 - SPI2_MOSI Pin3 LCD (SDIN)
	 */
	GPIOB->MODER |= 0x88000000;//alternate function mode
	GPIOB->OSPEEDR |= 0x88000000;//high speed
//	GPIOB->PUPDR &= ~0xCC000000;//no pull-up & pull-down
	GPIOB->PUPDR |= 0x88000000;//high speed
	GPIOB->AFR[1] |= 0x50500000;//SPI2 alternate function AF5

  SPI2->CR1 = 0;//clear CR1 register
  /**
 * 000 Fpclk/2
 * 001 Fpclk/4
 * 010 Fpclk/8
 * 011 Fpclk/16
 * 100 Fpclk/32
 * 101 Fpclk/64
 * 110 Fpclk/128
 * 111 Fpclk/256
 */
/** select baudrate ***********************************************************/
  SPI2->CR1 |= SPI_CR1_BR_2;
  SPI2->CR1 |= SPI_CR1_BR_1;
  SPI2->CR1 |= SPI_CR1_BR_0;
/** select clock phase & polarity *********************************************/
//  SPI2->CR1 |= SPI_CR1_CPOL;//CPOL = 1
//  SPI2->CR1 |= SPI_CR1_CPHA;//CPHA = 1
/** select 8 bit or 16 bit frame **********************************************/
//  SPI2->CR1 |= SPI_CR1_DFF;//frame 16bit
/** select MSB or LSB transmitted first ***************************************/
//  SPI2->CR1 |= SPI_CR1_LSBFIRST;//LSB transmitted first
/** select slave or master mode ***********************************************/
  SPI2->CR1 |= SPI_CR1_MSTR;//master mode
/** select 2 or 1 data line mode **********************************************/
  SPI1->CR1 |= SPI_CR1_BIDIMODE;//1 line mode
/** receive or transmit enable ************************************************/
  SPI2->CR1 |= SPI_CR1_BIDIOE;//output enable (transmit only)
/** enable slave software slave management ************************************/
  SPI2->CR1 |= SPI_CR1_SSM;//software slave management enable
/** enable external slave select **********************************************/
  SPI2->CR1 |= SPI_CR1_SSI;//external slave select

  SPI2->CR1 |= SPI_CR1_SPE;//SPI enable
}


void SPI2_Send_byte(uint8_t data) {
	PCF8812_CS_ON();
	while (!(SPI2->SR & SPI_SR_TXE));
	SPI2->DR = data;
  while (!(SPI2->SR & SPI_SR_TXE));
	while ((SPI2->SR & SPI_SR_BSY));
	PCF8812_CS_OFF();
}

void SPI2_Send_data(uint8_t* data, uint16_t length) {
  PCF8812_CS_ON();
  while (length) {
    while (!(SPI2->SR & SPI_SR_TXE));
    SPI2->DR = *data;
    data++;
    length--;
//    while (!(SPI2->SR & SPI_SR_TXE));
//    while ((SPI2->SR & SPI_SR_BSY));
  }
  while (!(SPI2->SR & SPI_SR_TXE));
  while ((SPI2->SR & SPI_SR_BSY));
  PCF8812_CS_OFF();
}


void SPI2_DMA_Init(void) {
	SPI2_Init();
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;//DMA1 enable
	/*SPI2 TX DMA1 Stream4 channel0*/
	DMA1_Stream4->CR = 0;//clear control register
	DMA1_Stream4->CR &= ~DMA_SxCR_CHSEL;//channel0 select
	DMA1_Stream4->CR |= DMA_SxCR_PL_0;//medium priority
	DMA1_Stream4->CR &= ~(DMA_SxCR_MSIZE|DMA_SxCR_PSIZE);//memory & peripheral data size 1 byte
	DMA1_Stream4->CR |= DMA_SxCR_MINC;//memory increment mode

#if 0
	DMA1_Stream4->CR |= DMA_SxCR_CIRC;//circular mode
	DMA1_Stream4->M0AR = (uint32_t)(PCF8812_buff);//memory address
	DMA1_Stream4->NDTR = PCF8812_BUFSIZ;//data size
#endif
	DMA1_Stream4->CR |= DMA_SxCR_DIR_0;//memory to peripheral direction
	DMA1_Stream4->CR |= DMA_SxCR_TCIE;//transfer complete interrupt enable
	DMA1_Stream4->PAR = (uint32_t)&(SPI2->DR);//peripheral address

	DMA1_Stream4->FCR = 0;//clear FIFO control register
	SPI2->CR2 |= SPI_CR2_TXDMAEN;//Tx buffer DMA enable

	NVIC_SetPriority(DMA1_Stream4_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);//IRQ handler
}




void SPI2_DMA_Tx(__IO uint8_t* data, uint16_t size) {
  DMA1_Stream4->M0AR = (uint32_t)data;//memory address
  DMA1_Stream4->NDTR = size;//data size
  PCF8812_buff_state = PCF8812_BUSY;
  DMA1_Stream4->CR |= DMA_SxCR_EN;//stream enable
}

void SPI2_Send_buff() {
  PCF8812_CS_ON();
  DMA1_Stream4->M0AR = (uint32_t)PCF8812_buff;//memory address
  DMA1_Stream4->NDTR = PCF8812_BUFSIZ;//data size
  PCF8812_buff_state = PCF8812_BUSY;
  DMA1_Stream4->CR |= DMA_SxCR_EN;//stream enable
}

void I2S3_Init(void) {
  /*
   * PC7  - I2S3_MCK
   * PC10 - I2S3_SCK
   * PC12 - I2S3_SD
   * PA4  - I2S3_WS
   */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOCEN;

#ifdef USE_I2S3_MCLK_ONLY//only PC7 init
  GPIOC->MODER |= GPIO_MODER_MODER7_1;//AF mode
  GPIOC->OTYPER &= ~GPIO_OTYPER_OT_7;//Push-pull
  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR7;//No pull-up & pull-down
  GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_1;//High speed
  GPIOC->AFR[0] |= 0x60000000;//I2S3 AF6

#else
  GPIOC->MODER |= GPIO_MODER_MODER7_1|GPIO_MODER_MODER10_1|GPIO_MODER_MODER12_1;//AF mode
  GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_7|GPIO_OTYPER_OT_10|GPIO_OTYPER_OT_12);//Push-pull
  GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR7|GPIO_PUPDR_PUPDR10|GPIO_PUPDR_PUPDR12);//No pull-up & pull-down
  GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_1|GPIO_OSPEEDER_OSPEEDR10_1|GPIO_OSPEEDER_OSPEEDR12_1;//High speed
  GPIOC->AFR[0] |= 0x60000000;//I2S3 AF6
  GPIOC->AFR[1] |= 0x00060600;//I2S3 AF6

  GPIOA->MODER &= ~(GPIO_MODER_MODER4_1|GPIO_MODER_MODER4_0);//clear bits
  GPIOA->MODER |= GPIO_MODER_MODER4_1;//AF mode
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4;//Push-pull
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4;//No pull-up & pull-down
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4_1;//High speed
  GPIOA->AFR[0] |= 0x00060000;//I2S3 AF6
#endif //USE_I2S3_MCLK_ONLY

  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;//SPI3 clock enable
  SPI3->I2SCFGR = 0;//Clear
  SPI3->I2SPR = 0x0002;//Clear
  /*
   * Audio clock frequency 44.1 kHz
   * PLLI2SN = 271
   * PLLI2SR = 2
   * I2SDIV = 6
   * I2SODD = 0
   */
  SPI3->I2SPR = 0x0006;//I2S Linear prescaler = 6, Odd factor for the prescaler = 0
  SPI3->I2SPR |= SPI_I2SPR_MCKOE;//Master clock output is enabled

  RCC->CFGR &= ~RCC_CFGR_I2SSRC;//PLLI2S clock used as I2S clock source
  RCC->PLLI2SCFGR |= RCC_PLLI2SCFGR_PLLI2SR_1;//PLLI2SR = 2
  RCC->PLLI2SCFGR = (CS43L22_PLLI2S_N << 6)|(CS43L22_PLLI2S_R << 28);//conf. PLLI2S
  RCC->CR |= RCC_CR_PLLI2SON;//PLLI2S ON
  while (!(RCC->CR & RCC_CR_PLLI2SRDY));//wait PLLI2S ready

  SPI3->I2SCFGR &= ~SPI_I2SCFGR_DATLEN;//16-bit data length
  SPI3->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1;//Master transmit
  SPI3->I2SCFGR &= ~SPI_I2SCFGR_I2SSTD;//I2S Philips standard
  SPI3->I2SCFGR &= ~SPI_I2SCFGR_CKPOL;//I2S clock steady state is low level
  SPI3->I2SCFGR |= SPI_I2SCFGR_I2SMOD;//I2S mode is selected

  //SPI3->CR2 |= SPI_CR2_TXEIE;


}

void I2S3_DMA_Init(uint32_t size, int16_t *data) {
  //I2S3_Init();

  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;//DMA1 enable
  /*SPI3 TX DMA1 Stream_7 channel_0*/
  DMA1_Stream7->CR &= ~DMA_SxCR_EN;//DMA stream disable
  DMA1_Stream7->CR = 0;//clear control register
  DMA1_Stream7->CR &= ~DMA_SxCR_CHSEL;//channel0 select
  DMA1_Stream7->CR |= DMA_SxCR_PL_1;//high priority
  DMA1_Stream7->CR |= DMA_SxCR_PSIZE_0|DMA_SxCR_MSIZE_0;//memory & peripheral data size Half-word (16-bit)
  DMA1_Stream7->CR |= DMA_SxCR_MINC;//memory increment mode
  DMA1_Stream7->CR &= ~DMA_SxCR_PINC;//peripheral increment mode disable
  DMA1_Stream7->CR |= DMA_SxCR_DIR_0;//memory to peripheral direction
  DMA1_Stream7->CR &= ~DMA_SxCR_CIRC;//no circular mode
  DMA1_Stream7->CR &= ~(DMA_SxCR_MBURST|DMA_SxCR_PBURST);//memory & peripheral burst single
  DMA1_Stream7->PAR = (uint32_t)&(SPI3->DR);//peripheral address
  DMA1_Stream7->M0AR = (uint32_t)data;//memory address
  DMA1_Stream7->NDTR = size;//data size

  DMA1_Stream7->FCR = 0;//clear FIFO control register
  DMA1_Stream7->CR |= DMA_SxCR_TCIE;//transfer complete interrupt enable
  SPI3->CR2 |= SPI_CR2_TXDMAEN;//Tx buffer DMA enable

  DMA1_Stream7->CR |= DMA_SxCR_EN;

  //NVIC_SetPriorityGrouping(4);//4 field for priority group
  //NVIC_SetPriority(DMA1_Stream7_IRQn, 4);
  //NVIC_EnableIRQ(DMA1_Stream7_IRQn);//IRQ handler
}

void I2S3_DMA_DeInit(void) {
  DMA1_Stream7->CR &= ~((uint32_t)DMA_SxCR_EN);//Disable DMA1 Stream7
  DMA1_Stream7->CR = 0;//Reset DMA1 Stream7 control register
  DMA1_Stream7->NDTR = 0;//Reset DMA1 Stream7 Number of Data to Transfer register
  DMA1_Stream7->PAR = 0;//Reset DMA1 Stream7 peripheral address register
  DMA1_Stream7->M0AR = 0;//Reset DMA1 Stream7 memory 0 address register
  DMA1_Stream7->M1AR = 0;//Reset DMA1 Stream7 memory 1 address register
  DMA1_Stream7->FCR = (uint32_t)0x00000021;//Reset DMAy Streamx FIFO control register
  /* Reset interrupt pending bits for DMA1 Stream7 */
  DMA1->HIFCR &= ~(DMA_HISR_TCIF7|DMA_HISR_HTIF7|DMA_HISR_TEIF7|\
  DMA_HISR_DMEIF7|DMA_HISR_FEIF7);
}

