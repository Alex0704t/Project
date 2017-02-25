//--------------------------------------------------------------
// File       : stm32_sdcard.c
// Date       : 15.02.2017
// Author     : UB
// CPU        : STM32F407VG
// IDE        : Eclipse IDE for C/C++ Developers
//            : Version: Neon.2 Release (4.6.2)
// GCC        : 5.4 2016q3
//
//   PA3 -> ChipSelect = SD-Card CS   (1)
//   PA5 -> SPI-SCK    = SD-Card SCLK (5)
//   PA6 -> SPI-MISO   = SD-Card DO   (7) (*)
//   PA7 -> SPI-MOSI   = SD-Card DI   (2)
//    (*) MISO needs PullUp (internal or external)
//
// mit Detect-Pin :
//
//   PC0  -> SD_Detect-Pin (Hi=ohne SD-Karte)
//
//--------------------------------------------------------------
// This file remade from stm32_ub_sdcard.c
// Author     : UB
// EMail      : mc-4u(@)t-online.de
// Web        : www.mikrocontroller-4u.de
// Project    : Demo_89_FAFTS_SPI
//
//--------------------------------------------------------------
// Copyright (C) 2014, ChaN, all right reserved.
//
// * This software is a free software and there is NO WARRANTY.
// * No restriction on use. You can use, modify and redistribute it for
//   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
// * Redistributions of source code must retain the above copyright notice.
//
//-------------------------------------------------------------------------




//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include <FAT/fatfs/lo_level/stm32_sdcard.h>
#include "spi.h"


//--------------------------------------------------------------
// Global Variable
//--------------------------------------------------------------
static volatile DSTATUS Stat = STA_NOINIT;
static volatile UINT Timer1, Timer2;
static BYTE CardType;



//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void SDCard_CS_Init(void);
inline void SDCard_CS_DeInit(void);
inline void SDCard_CS_Off(void);
inline void SDCard_CS_On(void);
void FCLK_SLOW(void);
void FCLK_FAST(void);
uint8_t SD_Detect(void);
//--------------------------------------------------------------
static BYTE xchg_spi(BYTE data);
static void rcvr_spi_multi(BYTE *buff, UINT btr);
static void xmit_spi_multi(const BYTE *buff, UINT btx);
static int wait_ready(UINT wt);
static void SD_unselect(void);
static int SD_select(void);
static int rcvr_datablock (BYTE *buff, UINT btr);
static int xmit_datablock(const BYTE *buff,BYTE token);
static BYTE send_cmd (BYTE cmd, DWORD arg);
void Disk_Timerproc(void);
//-------------------------------------------------------------- 

 

//--------------------------------------------------------------
// initialization Hardware before using SDCard
//--------------------------------------------------------------
void SDCard_Init(void) {
  SDCard_CS_Init();
  SDCard_CS_Off();
//  SPI1_Init();
  SPI1_DMA_Init();
//  delay_ms(10);
  for(Timer1 = 10; Timer1;); // 10ms
}


//--------------------------------------------------------------
// Check ob Medium eingelegt ist
// Return Wert :
//   > 0  = wenn Medium eingelegt ist
//     0  = wenn kein Medium eingelegt ist
//--------------------------------------------------------------
uint8_t UB_SDCard_CheckMedia(void) 
{
  uint8_t ret_wert=0;
  
  ret_wert=SD_Detect();

  return(ret_wert);
}


//--------------------------------------------------------------
// init der Disk
//-------------------------------------------------------------- 
DSTATUS MMC_disk_initialize(void) {
  BYTE n, cmd, ty, ocr[4];
  for (Timer1 = 10; Timer1;); // 10ms
  if (Stat & STA_NODISK)
    return Stat;
  FCLK_SLOW();
  for (n = 10; n; n--)
    xchg_spi(0xFF);	// Send 80 dummy clocks
  ty = 0;
  if (send_cmd(CMD0, 0) == 1) { //Put the card SPI/Idle state 
    Timer1 = 1000; // Initialization timeout = 1 sec  
    if (send_cmd(CMD8, 0x1AA) == 1) {	// SDv2?  
      for (n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF); // Get 32 bit return value of R7 resp 
      if (ocr[2] == 0x01 && ocr[3] == 0xAA) { // Is the card supports vcc of 2.7-3.6V? 
        while (Timer1 && send_cmd(ACMD41, 1UL << 30)) ; // Wait for end of initialization with ACMD41(HCS) 
        if (Timer1 && send_cmd(CMD58, 0) == 0) { // Check CCS bit in the OCR 
          for (n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF);
          ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2; // Card id SDv2 
        }
      }
    }
    else { // Not SDv2 card 
      if (send_cmd(ACMD41, 0) <= 1) { // SDv1 or MMC? 
        ty = CT_SD1; cmd = ACMD41; //SDv1 (ACMD41(0)) 
      } else {
        ty = CT_MMC; cmd = CMD1; // MMCv3 (CMD1(0)) 
      }
      while (Timer1 && send_cmd(cmd, 0)) ; // Wait for end of initialization 
      if (!Timer1 || send_cmd(CMD16, 512) != 0)
        ty = 0; // Set block length: 512
    }
  }
  CardType = ty; // Card type 
  SD_unselect();
  if (ty) { // OK 
    FCLK_FAST(); // Set fast clock 
    Stat &= ~STA_NOINIT; // Clear STA_NOINIT flag 
  }
  else { // Failed
    Stat = STA_NOINIT;
  }
  return Stat;
}


//--------------------------------------------------------------
// Disk Status abfragen
//-------------------------------------------------------------- 
DSTATUS MMC_disk_status(void) {
  return Stat;
}


//--------------------------------------------------------------
// READ-Funktion
// buff : Pointer to the data buffer to store read data
// sector : Start sector number (LBA)
// count : Number of sectors to read (1..128)
//--------------------------------------------------------------
DRESULT MMC_disk_read(BYTE *buff, DWORD sector, UINT count) {
  if (!count)
    return RES_PARERR;
  if (Stat & STA_NOINIT)
    return RES_NOTRDY;
  if (!(CardType & CT_BLOCK))
    sector *= 512; // LBA ot BA conversion (byte addressing cards)
  if (count == 1) { // Single sector read 
    // READ_SINGLE_BLOCK
    if ((send_cmd(CMD17, sector) == 0) && rcvr_datablock(buff, 512))
      count = 0;
  }
  else { // Multiple sector read 
    if (send_cmd(CMD18, sector) == 0) { // READ_MULTIPLE_BLOCK 
      do {
        if (!rcvr_datablock(buff, 512))
          break;
        buff += 512;
      } while (--count);
      send_cmd(CMD12, 0); // STOP_TRANSMISSION 
    }
  }
  SD_unselect();
  return count ? RES_ERROR : RES_OK;
}


//--------------------------------------------------------------
// WRITE-Funktion
// buff : Ponter to the data to write
// sector : Start sector number (LBA)
// count : Number of sectors to write (1..128)
//--------------------------------------------------------------
#if _USE_WRITE
DRESULT MMC_disk_write(const BYTE *buff, DWORD sector,	UINT count) {
  if (!count)
    return RES_PARERR;
  if (Stat & STA_NOINIT)
    return RES_NOTRDY;
  if (Stat & STA_PROTECT) return
      RES_WRPRT;
  if (!(CardType & CT_BLOCK))
    sector *= 512; // LBA ==> BA conversion (byte addressing cards)

  if (count == 1) { // Single sector write 
    // WRITE_BLOCK
    if ((send_cmd(CMD24, sector) == 0) && xmit_datablock(buff, 0xFE)) count = 0;
  }
  else { // Multiple sector write 
    if (CardType & CT_SDC) send_cmd(ACMD23, count); // Predefine number of sectors 
    if (send_cmd(CMD25, sector) == 0) { // WRITE_MULTIPLE_BLOCK
      do {
        if (!xmit_datablock(buff, 0xFC)) break;
        buff += 512;
      } while (--count);
      //STOP_TRAN token
      if (!xmit_datablock(0, 0xFD)) count = 1;
    }
  }
  SD_unselect();
  return count ? RES_ERROR : RES_OK;
}
#endif


//--------------------------------------------------------------
// IOCTL-Funktion
// cmd : Control command code
// buff : Pointer to the conrtol data
//--------------------------------------------------------------
#if _USE_IOCTL
DRESULT MMC_disk_ioctl(BYTE cmd,void *buff) {
  DRESULT res;
  BYTE n, csd[16];
  DWORD *dp, st, ed, csize;
  if (Stat & STA_NOINIT)
    return RES_NOTRDY;
  res = RES_ERROR;
  switch (cmd) {
    case CTRL_SYNC : // Wait for end of internal write process of the drive 
      if (SD_select())
        res = RES_OK;
    break;
    case GET_SECTOR_COUNT : // Get drive capacity in unit of sector (DWORD) 
      if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
        if ((csd[0] >> 6) == 1) { // SDC ver 2.00 
          csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
          *(DWORD*)buff = csize << 10;
        }
        else { // SDC ver 1.XX or MMC ver 3
          n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
          csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
          *(DWORD*)buff = csize << (n - 9);
        }
        res = RES_OK;
      }
    break;
    case GET_BLOCK_SIZE : // Get erase block size in unit of sector (DWORD) 
      if (CardType & CT_SD2) { // SDC ver 2.00 
        if (send_cmd(ACMD13, 0) == 0) { // Read SD status
          xchg_spi(0xFF);
          if (rcvr_datablock(csd, 16)) { // Read partial block 
            for (n = 64 - 16; n; n--)
              xchg_spi(0xFF);	// Purge trailing data
            *(DWORD*)buff = 16UL << (csd[10] >> 4);
            res = RES_OK;
          }
        }
      } else { // SDC ver 1.XX or MMC 
        if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) { // Read CSD 
          if (CardType & CT_SD1) { // SDC ver 1.XX 
            *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
          } else { // MMC 
            *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
          }
          res = RES_OK;
        }
      }
    break;
    case CTRL_TRIM : // Erase a block of sectors (used when _USE_ERASE == 1) 
      if (!(CardType & CT_SDC))
        break; // Check if the card is SDC
      if (MMC_disk_ioctl(MMC_GET_CSD, csd)!=0)
        break; // Get CSD
      if (!(csd[0] >> 6) && !(csd[10] & 0x40))
        break; // Check if sector erase can be applied to the card
      dp = buff; st = dp[0]; ed = dp[1]; // Load sector block 
      if (!(CardType & CT_BLOCK)) {
        st *= 512; ed *= 512;
      }
      // Erase sector block 
      if (send_cmd(CMD32, st) == 0 && send_cmd(CMD33, ed) == 0 && send_cmd(CMD38, 0) == 0 && wait_ready(30000))	     
        res = RES_OK; // FatFs does not check result of this command 
    break;
    default:
      res = RES_PARERR;
  }
  SD_unselect();
  return res;
}
#endif

void SDCard_CS_Init(void) {
#if (USE_DETECT_PIN == 1)
//    if need insert according GPIO pin initialization
#else
  /*PA3 chip select for SDCard*/
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  GPIOA->MODER |= GPIO_MODER_MODER3_0;//PE3 output
  GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR3;//low speed
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT_3;//push-pull
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR3;//no pull-up & pull-down
#endif
}

inline void SDCard_CS_DeInit(void) {
  GPIOA->MODER &= ~(GPIO_MODER_MODER3_0|GPIO_MODER_MODER3_1);//PA3 reset state
}

//--------------------------------------------------------------
// spi init
// (set internal PullUp for MISO)
//--------------------------------------------------------------
inline void SDCard_CS_Off(void) {
  GPIOA->BSRR = GPIO_BSRR_BS_3;
}

inline void SDCard_CS_On(void) {
  GPIOA->BSRR = GPIO_BSRR_BR_3;
}

//--------------------------------------------------------------
// SPI-Frq=Slow
//--------------------------------------------------------------
void FCLK_SLOW(void) {
  uint16_t tmpreg = 0;
  tmpreg = SPI1->CR1;
  tmpreg &= ~SPI_CR1_BR;
  tmpreg |= SD_SPI_CLK_FAST;
//  tmpreg |= SD_SPI_CLK_SLOW;
  SPI1->CR1 = tmpreg;
}


//--------------------------------------------------------------
// SPI-Frq=Fast
//--------------------------------------------------------------
void FCLK_FAST(void) {
  uint16_t tmpreg = 0;
  tmpreg = SPI1->CR1;
  tmpreg &= ~SPI_CR1_BR;
  tmpreg |= SD_SPI_CLK_FAST;
  SPI1->CR1 = tmpreg;
}


//--------------------------------------------------------------
// check ob Karte vorhanden
// ret_wert : 0=ohne Karte
//            1=mit Karte
//--------------------------------------------------------------
uint8_t SD_Detect(void) {
  uint8_t ret =1 ;
  #if (USE_DETECT_PIN == 1)
//    insert according code if need
//    if (DETECT_PIN == )) {
//      //no one card insert
//      ret = 0;
//    }
  #endif
  return ret;
}


//--------------------------------------------------------------
// Exchange a byte
// data : Data to send
//--------------------------------------------------------------
static BYTE xchg_spi(BYTE data) {
#if 0
  return SPI1_TxRxByte(data);
#else
  uint8_t temp = 0;
//  SPI1_TxRx(&data, &temp, 1);
  SPI1_DMA_TxRx(&data, &temp, 1);
  return temp;
#endif
}
//--------------------------------------------------------------
// Receive multiple byte
// buff : Pointer to data buffer
// btr : Number of bytes to receive (even number)
//--------------------------------------------------------------
static void rcvr_spi_multi(BYTE *buff, UINT btr) {
#if 0
  uint8_t *temp = (uint8_t *)malloc(btr*sizeof(UINT));
//  for(uint8_t i = 0; i < btr; i++)
//    *temp++ = 0xFF;
  SPI1_DMA_TxRx(temp, buff, btr);
//  SPI1_TxRx(temp, buff, btr);
#else
  BYTE b;
  do {
    b = SPI1_TxRxByte(0xFF);
    *buff = b;
    buff++;
    btr--;
  } while (btr != 0);
#endif
}


//--------------------------------------------------------------
#if _USE_WRITE
// Send multiple byte
// buf : Pointer to the data
// btx : Number of bytes to send (even number)
//--------------------------------------------------------------
static void xmit_spi_multi(const BYTE *buff, UINT btx) {
#if 0
  SPI1_TxRx(buff, NULL, btx);
#else
  BYTE b;
  do {
    b = *buff;
    SPI1_TxRxByte(b);
    buff++;
    btx--;
  } while (btx != 0);
#endif
}
#endif


//--------------------------------------------------------------
// Wait for card ready    
// wt :  Timeout [ms]
// ret_wert : 1:Ready, 0:Timeout
//--------------------------------------------------------------
static int wait_ready(UINT wt) {
  BYTE d;
  Timer2 = wt;
  do {
    d = xchg_spi(0xFF);
    // This loop takes a time. Insert rot_rdq() here for multitask envilonment.
  } while (d != 0xFF && Timer2); // Wait for card goes ready or timeout
  return (d == 0xFF) ? 1 : 0;
}


//--------------------------------------------------------------
// Deselect card and release SPI           
//--------------------------------------------------------------
static void SD_unselect(void) {
  SDCard_CS_Off();
  xchg_spi(0xFF); // Dummy clock (force DO hi-z for multiple slave SPI)
}


//--------------------------------------------------------------
// Select card and wait for ready  
// ret_wert :  1:OK, 0:Timeout 
//--------------------------------------------------------------
static int SD_select(void) {
  SDCard_CS_On();
  xchg_spi(0xFF); // Dummy clock (force DO enabled)
  if (wait_ready(500)) return 1; // OK
  SD_unselect();
  return 0; // Timeout
}


//--------------------------------------------------------------
// Receive a data packet from the MMC  
// buf : Data buffer
// btr : Data block length (byte)
// ret_wert : 1:OK, 0:Error 
//--------------------------------------------------------------
static int rcvr_datablock (BYTE *buff, UINT btr) {
  BYTE token;
  Timer1 = 200;
  do { // Wait for DataStart token in timeout of 200ms
    token = xchg_spi(0xFF);
    // This loop will take a time. Insert rot_rdq() here for multitask envilonment.
  } while ((token == 0xFF) && Timer1);
  if (token != 0xFE)
    return 0; // Function fails if invalid DataStart token or timeout
  rcvr_spi_multi(buff, btr); // Store trailing data to the buffer
  xchg_spi(0xFF);
  xchg_spi(0xFF); // Discard CRC
  return 1; // Function succeeded
}


//--------------------------------------------------------------
// Send a data packet to the MMC  
// buf : Ponter to 512 byte data to be sent
// token : Token
// ret_wert : 1:OK, 0:Failed
//--------------------------------------------------------------
#if _USE_WRITE
static int xmit_datablock(const BYTE *buff, BYTE token) {
  BYTE resp;
  if (!wait_ready(500))
    return 0; // Wait for card ready
  xchg_spi(token); // Send token
  if (token != 0xFD) { // Send data if token is other than StopTran
    xmit_spi_multi(buff, 512); // Data
    xchg_spi(0xFF);
    xchg_spi(0xFF); // Dummy CRC
    resp = xchg_spi(0xFF); // Receive data resp
    // Function fails if the data packet was not accepted
    if ((resp & 0x1F) != 0x05)
      return 0;
  }
  return 1;
}
#endif


//--------------------------------------------------------------
// Send a command packet to the MMC 
// cmd : Command index
// arg : Argument
// ret_wert : R1 resp (bit7==1:Failed to send)
//--------------------------------------------------------------
static BYTE send_cmd (BYTE cmd, DWORD arg) {
  BYTE n, res;
  if (cmd & 0x80) { // Send a CMD55 prior to ACMD<n>
    cmd &= 0x7F;
    res = send_cmd(CMD55, 0);
    if (res > 1) return res;
  }
  // Select the card and wait for ready except to stop multiple block read
  if (cmd != CMD12) {
    SD_unselect();
    if (!SD_select()) return 0xFF;
  }
  // Send command packet
  xchg_spi(0x40 | cmd); // Start + command index
  xchg_spi((BYTE)(arg >> 24)); // Argument[31..24]
  xchg_spi((BYTE)(arg >> 16)); // Argument[23..16]
  xchg_spi((BYTE)(arg >> 8)); // Argument[15..8]
  xchg_spi((BYTE)arg); // Argument[7..0]
  n = 0x01; // Dummy CRC + Stop
  if (cmd == CMD0)
    n = 0x95; // Valid CRC for CMD0(0)
  if (cmd == CMD8)
    n = 0x87; // Valid CRC for CMD8(0x1AA)
  xchg_spi(n);
  // Receive command resp
  if (cmd == CMD12)
    xchg_spi(0xFF); // Diacard following one byte when CMD12
  n = 10; // Wait for response (10 bytes max)
  do {
    res = xchg_spi(0xFF);
  } while ((res & 0x80) && --n);
  return res; // Return received response
}


//--------------------------------------------------------------
// Device timer function     
// This function must be called from timer interrupt routine in period
// of 1 ms to generate card control timing.
//--------------------------------------------------------------
void Disk_Timerproc (void) {
  WORD n;
  BYTE s;
  n = Timer1;
  if (n)
    Timer1 = --n;
  n = Timer2;
  if (n)
    Timer2 = --n;
  s = Stat;
  if (USE_WRITE_PROTECTION)
    s |= STA_PROTECT;
  else
    s &= ~STA_PROTECT;
  if (SD_Detect()!=0)
    s &= ~STA_NODISK;
  else
    s |= (STA_NODISK | STA_NOINIT);
  Stat = s;
}


