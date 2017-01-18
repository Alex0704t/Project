/*
 * usb.h
 *
 *  Created on: 27.12.2016
 *        Author: User
 */

#ifndef USB_LIB_INC_USB_H_
#define USB_LIB_INC_USB_H_

#include "main.h"
#include "user.h"
#include <string.h>
#include "../STM32_USB_Device_Library/Class/hid_custom/inc/usbd_hid_core.h"
#include "../STM32_USB_Device_Library/Core/inc/usbd_usr.h"
#include "../inc/usbd_desc.h"
#include "usbd_def.h"
#include "usbd_conf.h"
#include "usb_core.h"

void USB_HID_Init();
uint8_t USB_HID_SendReport(char* report, uint16_t len);
inline void ClearBuf(char* data, uint8_t len);
uint8_t Write_to_USB(char* data);
uint8_t Read_from_USB(char* data);
void USB_Echo();
void USB_Send_Int(int32_t value);
void USB_Count(uint32_t period_ms);
void USB_Ctrl();

#endif /* USB_LIB_INC_USB_H_ */
