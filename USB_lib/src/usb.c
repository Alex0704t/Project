/*
 * usb.c
 *
 *  Created on: 27.12.2016
 *      Author: User
 */

#include "usb.h"

USB_OTG_CORE_HANDLE  USB_OTG_dev;//USB OTG FS device struct
/** buffers for in & out data */

char InBuffer[HID_IN_PACKET], OutBuffer[HID_OUT_PACKET];
/**
* @brief  This Function initialize  USB OTG FS device.
* @param  none.
* @retval none.
*/
void USB_HID_Init() {
  USBD_Init(&USB_OTG_dev,
  USB_OTG_FS_CORE_ID,
  &USR_desc,
  &USBD_HID_cb,
  &USR_cb);
}

/**
* @brief  This Function send report via USB_OTG_dev.
* @param data: pointer of user string.
*         len: number of received bytes.
* @retval    : 0 if OK
*/
uint8_t USB_HID_SendReport(char* report, uint16_t len) {
  return USBD_HID_SendReport(&USB_OTG_dev, (uint8_t*)report, len);
}

/**
* @brief  This Function clear data buffer.
* @param data: pointer of buffer.
*         len: size of buffer.
* @retval none
*/
inline void ClearBuf(char* data, uint8_t len) {
  for (uint8_t i = 0; i < len; i++)
    data[i] = 0x0;
}

/**
* @brief  This Function send user data via USB.
* @param data: pointer of user string or array.
* @retval len: number of transmitted bytes.
*/
uint8_t Write_to_USB(char* data) {
  ClearBuf(InBuffer, HID_IN_PACKET);
  uint8_t len = 0;
  for (; data[len] != '\0'; len++) {
    InBuffer[len] = data[len];
  }
  USB_HID_SendReport(InBuffer, HID_IN_PACKET);
  return len;
}

/**
* @brief  This Function copies USB OutBuffer to user string or array.
* @param data: pointer of user string or array.
* @retval len: number of received bytes.
*/
uint8_t Read_from_USB(char* data) {
  uint8_t len = 0;
  for (; OutBuffer[len]; len++) {
    data[len] = OutBuffer[len];
  }
  ClearBuf(OutBuffer, HID_OUT_PACKET);
  return len;
}

/**
* @brief  This Function send back received data.
* @param  none:
* @retval none:
*/
void USB_Echo() {
  char buffer[HID_IN_PACKET] = {0};
  char str[PCF8812_STR_SIZ] = "";
  Button_Set_Name(user_button, "EXIT");
  while (1) {
    PCF8812_Clear();
    PCF8812_Title("USB ECHO");
    if (Read_from_USB(buffer)) {
      Write_to_USB(buffer);
      strncpy(str, buffer, PCF8812_STR_SIZ);
    }
    PCF8812_Putline_Center(str, 4);
    if (Button_Get(user_button))
      return;
    PCF8812_DELAY;
    ClearBuf(buffer, HID_IN_PACKET);
  }
}

/**
* @brief  This Function send int number via USB
* @param  data: int number.
* @retval none:
*/
void USB_Send_Int(int32_t value) {
  char buffer[HID_IN_PACKET] = {0};
  snprintf(buffer, HID_IN_PACKET, "%ld\n", value);
  Write_to_USB(buffer);
  ClearBuf(buffer, HID_IN_PACKET);
}

/**
* @brief  This Function send counter via USB
* @param  data: period between transmit in ms.
* @retval none:
*/
void USB_Count(uint32_t period_ms) {
  Button_Set_Name(user_button, "EXIT");
  Button_Set_Name(button_2, "CLEAR");
  uint32_t i = 0;
  uint8_t run_flag = 0;
  Button_Set_Name(button_1, "START");
  while (1) {
    PCF8812_Clear();
    PCF8812_Title("USB COUNT");
    PCF8812_UValue("", i, "", 4);
    if (Button_Get(user_button))
      return;
    if (Button_Get(button_1)) {
      run_flag ^= 1;
      if (!run_flag)
        Button_Set_Name(button_1, "START");
      else
        Button_Set_Name(button_1, "STOP");
    }
    if (Button_Get(button_2))
      i = 0;
    if (run_flag && Check_delay_ms(period_ms)) {
      USB_Send_Int(i);
      i++;
      }
    PCF8812_DELAY;
  }
}

/**
* @brief  This Function receive & execute command.
* @param  none:
* @retval none:
*/
void USB_Ctrl() {
  char buffer[HID_IN_PACKET] = "";
  char str[PCF8812_STR_SIZ] = "";
  uint8_t no_resp = 0;
  char* msg = "No response:";
  Button_Set_Name(user_button, "EXIT");
  while(1) {
    PCF8812_Clear();
    PCF8812_Title("USB CONTROL");
    if (Read_from_USB(buffer)) {
      strncpy(str, buffer, PCF8812_STR_SIZ);
      no_resp = 0;
      if (!strncmp(buffer, "green", 5) || !strncmp(buffer, "GREEN", 5))
        LED_TOGGLE(green);
      else if (!strncmp(buffer, "blue", 4) || !strncmp(buffer, "BLUE", 4))
        LED_TOGGLE(blue);
      else if (!strncmp(buffer, "red", 3) || !strncmp(buffer, "RED", 3))
        LED_TOGGLE(red);
      else if (!strncmp(buffer, "orange", 6) || !strncmp(buffer, "ORANGE", 6))
        LED_TOGGLE(blue);
      else
        no_resp = 1;
    }
    if(no_resp)
      PCF8812_Putline_Center(msg, 3);
    PCF8812_Putline_Center(str, 4);
    if (Button_Get(user_button))
      return;
    PCF8812_DELAY;
    ClearBuf(buffer, HID_IN_PACKET);
  }
}
