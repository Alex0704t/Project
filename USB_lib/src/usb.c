/*
 * usb.c
 *
 *  Created on: 27.12.2016
 *      Author: User
 */

#include "usb.h"

USB_OTG_CORE_HANDLE  USB_OTG_dev;
uint8_t InBuffer[63], OutBuffer[63];

void USB_HID_Init(){
  USBD_Init(&USB_OTG_dev,
  USB_OTG_FS_CORE_ID,
  &USR_desc,
  &USBD_HID_cb,
  &USR_cb);
}

uint8_t USB_HID_SendReport(uint8_t *report, uint16_t len){
  return USBD_HID_SendReport(&USB_OTG_dev, report, len);
}

void ClearBuf(uint8_t* data, uint8_t len){
  for(uint8_t i = 0; i < len; i++)
    data[i] = 0x0;
}

uint8_t Write_to_USB(uint8_t* data){
  ClearBuf(InBuffer, 63);
  uint8_t len = 0;
  for(; data[len] != '\0'; len++){
    InBuffer[len] = data[len];
  }
  USB_HID_SendReport(InBuffer, 63);
  return len;
}

/**
* @brief  This Function copies USB OutBuffer to user string or array.
* @param data: pointer of user string or array.
* @retval len: number of received bytes
*/
uint8_t Read_from_USB(uint8_t *data){
  uint8_t len = 0;
  for(; OutBuffer[len]; len++){
    data[len] = OutBuffer[len];
  }
  ClearBuf(OutBuffer, 63);
  return len;
}

void USB_Echo(){
  uint8_t Buffer[63] = {0};
  uint8_t str[PCF8812_STR_SIZ] = "";
  while(1)
    {
    PCF8812_Clear();
    PCF8812_Title("USB ECHO");
    PCF8812_Button("OK", "", "");
    if(Get_Button(user_button))
      break;
    if(Read_from_USB(Buffer)){
      Write_to_USB(Buffer);
      strncpy(str, Buffer, PCF8812_STR_SIZ);
    }
    PCF8812_Putline_Centre(str, 4);
    PCF8812_DELAY;
    ClearBuf(Buffer, 63);
    }
}
