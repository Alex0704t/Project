/*
 *  pcf8812.c
 *
 *  Created on: 26.07.2015
 *      Author: Aleksey
 */

#include "pcf8812.h"

void PCF8812_Port_Init(void)
{
	/*
	 * PB13 - SPI2_SCK Pin2 LCD (SCLK)
	 * PB15 - SPI2_MOSI Pin3 LCD (SDIN)
	 * PD8 -  Vdd
	 * PD9 -  D/C
	 * PD10 - SCE (active Low) (chip select)
	 * PD11 - RES (active Low) (reset)
	 * PB14 - light
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;//GPIOD clock enable
	GPIOD->MODER |= 0x00550000;//PD8, 9, 10, 11 output
	GPIOD->OSPEEDR &= ~0x00FF0000;//Low speed PD8, 9, 10, 11
	GPIOD->OTYPER &= ~0x0F00;//Push-pull PD8, 9, 10, 11
	GPIOD->PUPDR &= ~0x00FF0000;//No pull-up & pull-down PD8, 9, 10, 11
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;//GPIOB clock enable
  GPIOB->MODER |= GPIO_MODER_MODER14_0;//PB14 output==
  GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR14;//Low speed PB14
  GPIOB->OTYPER &= ~GPIO_OTYPER_OT_14;//Push-pull PB14
  GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR14;//No pull-up & pull-down PB14
}

void PCF8812_Command(uint8_t data) {
	PCF8812_CMD();//reset D/C for command mode
	Send_SPI2_byte(data);//send command
}

void PCF8812_Data(uint8_t data) {
	PCF8812_DATA();//set D/C for data mode
	Send_SPI2_DMA(&data, 1);//send data via DMA
}

void PCF8812_Set() {
  PCF8812_Reset();
	PCF8812_Command(FUNC_SET|FUNC_SET_H);//extended instruction set
	PCF8812_Command(TEMP_CONT_3);//temperature coefficient 3
	PCF8812_Command(VOLT_MUL_4);//voltage multiplier x3
	PCF8812_Command(BIAS_SET|0x06);//set bias system
	PCF8812_Command(VOP_SET|0x4F);//Vlcd programming
	PCF8812_Command(FUNC_SET);//base instruction set
	PCF8812_Command(DISP_NORMAL);//normal mode
}

void PCF8812_Init(void) {
	PCF8812_Port_Init();
	SPI2_DMA_Init();
	PCF8812_Set();
	//PCF8812_ON_flag = SET;
	Tim5_Init(PCF8812_F_RATE);
	PCF8812_Clear();
	PCF8812_LIGHT_ON();
}

void PCF8812_Reset(void)
{
  PCF8812_RES_ON();
	PCF8812_POW_ON();
	delay_ms(1);
	PCF8812_RES_OFF();
}

void PCF8812_Clear(void) {
	for(uint16_t i = 0; i < PCF8812_BUFSIZ; i++)
	  PCF8812_buff[i] = 0x00;
	PCF8812_NEED_FLUSH();
}

void PCF8812_Test(void) {
  for(uint16_t i = 0; i < PCF8812_BUFSIZ; i += 16)
    {
    PCF8812_buff[i +  0] = 0x01;
    PCF8812_buff[i +  1] = 0x01;
    PCF8812_buff[i +  2] = 0x02;
    PCF8812_buff[i +  3] = 0x02;
    PCF8812_buff[i +  4] = 0x04;
    PCF8812_buff[i +  5] = 0x04;
    PCF8812_buff[i +  6] = 0x08;
    PCF8812_buff[i +  7] = 0x08;
    PCF8812_buff[i +  8] = 0x10;
    PCF8812_buff[i +  9] = 0x10;
    PCF8812_buff[i + 10] = 0x20;
    PCF8812_buff[i + 11] = 0x20;
    PCF8812_buff[i + 12] = 0x40;
    PCF8812_buff[i + 13] = 0x40;
    PCF8812_buff[i + 14] = 0x80;
    PCF8812_buff[i + 15] = 0x80;
    }
  delay_ms(1000);
  PCF8812_Putline_Center("Hello, World!", 0);
  PCF8812_Putline_Center("Привет, Мир!", 1);
  PCF8812_Putline_Center("Привіт, Світ!", 2);
  PCF8812_Putline_Center("ARM Cortex M4", 3);
  PCF8812_Putline_Center("STM32F4Discovery", 4);
  PCF8812_Putline_Center("Aleksey Totkal", 5);
  PCF8812_Putline_Center("Алексей Тоткал", 6);
  PCF8812_Putline_Center("Олексій Тоткал", 7);
  delay_ms(2000);
  PCF8812_Clear();
}

void PCF8812_Toggle(void) {
	static uint8_t mode = DISP_NORMAL;
	if(mode == DISP_INVERSE)
	{
		PCF8812_Command(DISP_INVERSE);
		mode = DISP_NORMAL;
	}
	else
	{
		PCF8812_Command(DISP_NORMAL);
		mode = DISP_INVERSE;
	}
}

void PCF8812_Home(void) {
	PCF8812_Command(SET_X);//x = 0
	PCF8812_Command(SET_Y);//y = 0
}

void PCF8812_XY(uint8_t x, uint8_t y) {
	PCF8812_Command(SET_X|x);
	PCF8812_Command(SET_Y|y);
}

void PCF8812_Clr_Line(uint8_t line) {
  for(uint8_t i = 0; i < PCF8812_XSIZE; i++)
    PCF8812_Set_byte(line, i, 0x00);//reset all pixels
}

void PCF8812_Inv_Line(uint8_t line) {
  for(uint8_t i = 0; i < PCF8812_XSIZE; i++)
    PCF8812_Inv_byte(line, i);//inverse all bytes in line
}

uint8_t PCF8812_Decode(uint8_t c) {
	if(' ' <= c && c <= '~')//standard ASCII character
	{
		c -= 32;
	}
	else
	{
		//if(192 <= c && c <= 255)
		if(/*(wchar_t)*/'Ґ' <= c && c <= /*(wchar_t)*/'я')//cyrillic ASCII character
		{
			c -= 70;//97;
		}
		else
		{
			c = 255;
		}
	}
	return c;
}

void PCF8812_Set_Char(uint8_t c, uint8_t line, uint8_t col) {
  c = PCF8812_Decode(c);
  if(c == 255)//no recognise character
    {
     return;
    }
   //set 5x8 font character
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 0] = lcd_font[c][0];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 1] = lcd_font[c][1];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 2] = lcd_font[c][2];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 3] = lcd_font[c][3];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 4] = lcd_font[c][4];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 5] = 0x00;
  PCF8812_NEED_FLUSH();
}

void PCF8812_Set_Symb(symb_ar symb, uint8_t line, uint8_t col) {
  //set some graphic symbol
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 0] = symb[0];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 1] = symb[1];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 2] = symb[2];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 3] = symb[3];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 4] = symb[4];
  PCF8812_buff[PCF8812_DECODE_C(line, col) + 5] = 0x00;
  PCF8812_NEED_FLUSH();
}

void PCF8812_Inv_Char(uint8_t line, uint8_t col) {
  //set inverse value of 5x8 character
  PCF8812_Inv_byte(line, PCF8812_CHAR_WIDE * col + 0);
  PCF8812_Inv_byte(line, PCF8812_CHAR_WIDE * col + 1);
  PCF8812_Inv_byte(line, PCF8812_CHAR_WIDE * col + 2);
  PCF8812_Inv_byte(line, PCF8812_CHAR_WIDE * col + 3);
  PCF8812_Inv_byte(line, PCF8812_CHAR_WIDE * col + 4);
  PCF8812_Inv_byte(line, PCF8812_CHAR_WIDE * col + 5);
}

//Error message in line 0
void PCF8812_Error(char* s) {
  char str[PCF8812_STR_SIZ] = "ERR:";
  strcat(str, s);
  PCF8812_Putline(str, 0);
}

//Message in line 0
void PCF8812_Message(char* s) {
  char str[PCF8812_STR_SIZ] = "MSG:";
  strcat(str, s);
  PCF8812_Putline(str, 0);
}

//View menu name in line 0
void PCF8812_Title(char* s) {
  PCF8812_Putline_Center(s, 0);
}

void PCF8812_Putline(char* s, uint8_t line) {
  char str[PCF8812_STR_SIZ] = "";
  strcpy(str, s);
  strncat(str, EMPTY_STR, (PCF8812_LCD_LINE - strlen(s)));//added spaces for clear end of line
  for(uint8_t i = 0; i < PCF8812_LCD_LINE; i++)
    PCF8812_Set_Char(str[i], line, i);
}

void PCF8812_Putline_Center(char *s, uint8_t line) {
  uint8_t sp_num = PCF8812_LCD_LINE - strlen(s);//number of spaces
  char str[PCF8812_STR_SIZ] = "";
  //insert string in center of line
  strncpy(str, EMPTY_STR, sp_num / 2);
  strcat(str, s);
  strncat(str, EMPTY_STR, sp_num / 2 + sp_num % 2);
  PCF8812_Putline(str, line);
}

void PCF8812_Putline_Right(char* s, uint8_t line) {
  uint8_t sp_num = PCF8812_LCD_LINE - strlen(s);//number of spaces
  char str[PCF8812_STR_SIZ] = "";
  //insert string in end of line
  strncpy(str, EMPTY_STR, sp_num);
  strcat(str, s);
  PCF8812_Putline(str, line);
}

void PCF8812_SValue(char* name, int32_t value, char* unit, uint8_t line) {
  char str[PCF8812_STR_SIZ];
  //call snprintf for calculate wide for name
  uint8_t name_wide = PCF8812_LCD_LINE - snprintf(str, PCF8812_STR_SIZ, "% li%.3s", value, unit);
  snprintf(str, PCF8812_STR_SIZ, "%.*s% li%.3s", name_wide, name, value, unit);
  PCF8812_Putline_Center(str, line);
}

void PCF8812_UValue(char* name, uint32_t value, char* unit, uint8_t line) {
  char str[PCF8812_STR_SIZ];
  //call snprintf for calculate wide for name
  uint8_t name_wide = PCF8812_LCD_LINE - snprintf(str, PCF8812_STR_SIZ, " %lu%.3s", value, unit);
  snprintf(str, PCF8812_STR_SIZ, "%.*s %lu%.3s", name_wide, name, value, unit);
  PCF8812_Putline_Center(str, line);
}

void PCF8812_Hex_Value(char* name, int32_t value, uint8_t line) {
  char str[PCF8812_STR_SIZ];
  //call snprintf for calculate wide for name
  uint8_t name_wide = PCF8812_LCD_LINE - snprintf(str, PCF8812_STR_SIZ, " %#lx", value);
  snprintf(str, PCF8812_STR_SIZ, "%.*s %#lx", name_wide, name, value);
  PCF8812_Putline_Center(str, line);
}
#ifdef USE_SPRINTF
void PCF8812_Float_Value(char* name, double
                         value, char* unit, uint8_t line) {
  char str[PCF8812_STR_SIZ];
  uint8_t float_prec = 6;
  uint8_t name_len = strlen(name);
  //call snprintf for calculate wide for name
  uint8_t name_wide = PCF8812_LCD_LINE - snprintf(str, PCF8812_STR_SIZ, "% .*g%.3s", float_prec, value, unit);
  //name wide less than name len and less than 5
  if((name_wide < name_len) && (name_wide < 5)) {
      name_wide = (name_len > 5) ? 5 : name_len;
      float_prec = 8 - name_wide;//correct float precision
    }
  snprintf(str, PCF8812_STR_SIZ, "%.*s% .*g%.3s", name_wide, name, float_prec, value, unit);
  PCF8812_Putline_Center(str, line);
}
#else

char *  _EXFUN(gcvt,(double,int,char *));
//char *  _EXFUN(gcvtf,(float,int,char *));
//char *  _EXFUN(fcvt,(double,int,int *,int *));
//char *  _EXFUN(fcvtf,(float,int,int *,int *));
//char *  _EXFUN(ecvt,(double,int,int *,int *));
//char *  _EXFUN(ecvtbuf,(double, int, int*, int*, char *));
//char *  _EXFUN(fcvtbuf,(double, int, int*, int*, char *));
//char *  _EXFUN(ecvtf,(float,int,int *,int *));

void PCF8812_Float_Value(char* name, double value, char* unit, uint8_t line) {
  char str[PCF8812_STR_SIZ];
  char f_str[PCF8812_STR_SIZ];
  int prec = 6;
  uint8_t name_len = strlen(name);
  uint8_t name_wide = (name_len > 6) ? 6 : name_len;
  gcvt(value, prec, f_str);
  snprintf(str, PCF8812_STR_SIZ, "%.*s% s%.3s", name_wide, name, f_str, unit);
  PCF8812_Putline_Center(str, line);
}
#endif

void PCF8812_Percent(char* name, int8_t value, uint8_t line) {
  uint8_t column = 0, i = 0;
  char str[PCF8812_STR_SIZ];
  if(value < 0)
    {
      PCF8812_Putline("Negative % value", line);
      return;
    }
  snprintf(str, PCF8812_STR_SIZ, "%.13s %d%%", name, value);
  PCF8812_Putline_Center(str, line);
  column = value;
  PCF8812_Set_byte((line + 1), i++, 0x7F);
  while(i < 100)
    {
      PCF8812_Set_byte((line + 1), i++, 0x41);
    }
  PCF8812_Set_byte((line + 1), i, 0x7F);
  while(column)
    {
      PCF8812_Set_byte((line + 1), column - 1, 0x7F);
      column--;
    }
}

void PCF8812_Set_bit(uint8_t x, uint8_t y) {
  PCF8812_buff[PCF8812_DECODE_XY(x, y)] |= (1 << (y % PCF8812_YSIZE));
  PCF8812_NEED_FLUSH();
}

void PCF8812_Clr_bit(uint8_t x, uint8_t y) {
  PCF8812_buff[PCF8812_DECODE_XY(x, y)] &= ~(1 << (y % PCF8812_YSIZE));
  PCF8812_NEED_FLUSH();
}

uint8_t PCF8812_Chk_bit(uint8_t x, uint8_t y) {
  return (PCF8812_buff[PCF8812_DECODE_XY(x, y)] & (1 << (y % PCF8812_YSIZE)));
  //PCF8812_NEED_FLUSH();
}

inline void PCF8812_Set_byte(uint8_t line, uint8_t x, uint8_t value) {
  PCF8812_buff[PCF8812_DECODE_LINE(line) + x] = value;
  PCF8812_NEED_FLUSH();
}

inline uint8_t PCF8812_Chk_byte(uint8_t line, uint8_t x) {
  return PCF8812_buff[PCF8812_DECODE_LINE(line) + x];
}

inline void PCF8812_Inv_byte(uint8_t line, uint8_t x) {
  PCF8812_Set_byte(line, x, ~PCF8812_Chk_byte(line, x));
  PCF8812_NEED_FLUSH();
}

void PCF8812_Handler(void) {
  if(PCF8812_buff_state == PCF8812_CHANGED/* && PCF8812_ON_flag == SET*/) {
    PCF8812_Home();//set coordinates to 0, 0
    PCF8812_DATA();//data mode
    Send_SPI2_DMA(PCF8812_buff, PCF8812_BUFSIZ);//flush LCD buffer
    PCF8812_buff_state = PCF8812_BUSY;
    }
}

void PCF8812_Point(uint8_t x, uint8_t y) {
  PCF8812_Set_bit(x, y);
}

void PCF8812_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  uint8_t x = 0, y = 0;
  uint8_t len_x = 0, len_y = 0;
  int8_t err = 0;
  int8_t ystep = 0;
  uint8_t swap_xy = 0;
  len_x = (x1 > x2) ? x1 - x2 : x2 - x1;
  len_y = (y1 > y2) ? y1 - y2 : y2 - y1;
  if (len_y > len_x)
  {
    swap_xy = 1;
    SWAP(uint8_t, len_x, len_y);
    SWAP(uint8_t, x1, y1);
    SWAP(uint8_t, x2, y2);
  }
  if (x1 > x2)
  {
    SWAP(uint8_t, x1, x2);
    SWAP(uint8_t, y1, y2);
  }
  err = len_x / 2;
  ystep = (y2 > y1) ? 1 : -1;
  y = y1;
  for(x = x1; x <= x2; x++)
  {
    if (swap_xy == 0)
      PCF8812_Set_bit(x, y);
    else
      PCF8812_Set_bit(y, x);
    err -= (uint8_t)len_y;
    if (err < 0)
    {
      y += (uint8_t)ystep;
      err += (uint8_t)len_x;
    }
  }
}

void PCF8812_Rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill) {
  if(fill == empty)
  {
  PCF8812_Line(x1, y1, x2, y1);
  PCF8812_Line(x2, y1, x2, y2);
  PCF8812_Line(x2, y2, x1, y2);
  PCF8812_Line(x1, y2, x1, y1);
  }
  else
  {
    if(x1 > x2)
      SWAP(uint8_t, x1, x2);
    if(y1 > y2)
      SWAP(uint8_t, y1, y2);
    for(uint8_t i = y1; i <= y2; i++)
      for(uint8_t j = x1; j <= x2; j++)
        {
          PCF8812_Set_bit(j, i);
        }
  }
}

void PCF8812_Triangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3) {
  PCF8812_Line(x1, y1, x2, y2);
  PCF8812_Line(x1, y1, x3, y3);
  PCF8812_Line(x3, y3, x2, y2);
}

void PCF8812_Time(uint8_t view, uint8_t line) {
  char str[PCF8812_STR_SIZ] = "";
  Get_Time_String(NULL, str, view);
  PCF8812_Putline_Center(str, line);
}

void PCF8812_Option(char* option, uint8_t line) {
  PCF8812_Putline(option, line);
}

void PCF8812_Cursor(uint8_t line) {
  if(line < 1 || line > 6)
    {
    PCF8812_Error("cursor out");
    return;
    }
  PCF8812_Inv_Line(line);
}


void PCF8812_Butt_name(uint8_t button, char* name) {
  uint8_t len = 0, start = 0;
  len = strlen(name);
  len = (len > 5) ? 5 : len;
  switch(button)
  {
    case button_1:
      start = 0;
      break;
    case user_button:
      start = BUTTON_NAME_SIZE + 1 + (BUTTON_NAME_SIZE - len) / 2;
      break;
    case button_2:
      start = PCF8812_LCD_LINE - len;
      break;
    default:
      break;
  }
  for(uint8_t i = start; i < (start + len); i++)
      PCF8812_Set_Char(*name++, 7, i);
}

void PCF8812_Butt_ind(uint8_t button) {
  switch(button)
  {
    case button_1:
      PCF8812_Inv_Char(7, 0);
      PCF8812_Inv_Char(7, 1);
      PCF8812_Inv_Char(7, 2);
      PCF8812_Inv_Char(7, 3);
      PCF8812_Inv_Char(7, 4);
      break;
    case user_button:
      PCF8812_Inv_Char(7, 6);
      PCF8812_Inv_Char(7, 7);
      PCF8812_Inv_Char(7, 8);
      PCF8812_Inv_Char(7, 9);
      PCF8812_Inv_Char(7, 10);
      break;
    case button_2:
      PCF8812_Inv_Char(7, 12);
      PCF8812_Inv_Char(7, 13);
      PCF8812_Inv_Char(7, 14);
      PCF8812_Inv_Char(7, 15);
      PCF8812_Inv_Char(7, 16);
      break;
    default:
      break;
  }
}

uint32_t PCF8812_Input_Int(char* name, uint32_t min, uint32_t max) {
  char str[PCF8812_STR_SIZ];
  if(max < min)
    SWAP(uint32_t, min, max);
  uint32_t result = 0;
  uint8_t n_dig = 0;//number of digits
  for(uint32_t base = max; base > 0; base /= 10)//calculate number of digits
    n_dig++;
  int8_t col = n_dig;//selected decimal digit
  uint32_t max_mul = pow(10, n_dig - 1);//max multiplier
  uint32_t mul = max_mul;//current multiplier
  Button_Set_Name(user_button, "OK");
  Button_Set_Name(button_1, " <");
  Button_Set_Name(button_2, "> ");
  while(1)
  {
  PCF8812_Clear();
  PCF8812_Title(name);
  //view value
  sprintf(str, "%.*s%.*u%.*s", (PCF8812_LCD_LINE - n_dig)/2, EMPTY_STR, n_dig, result, \
                               (PCF8812_LCD_LINE - n_dig)/2 + (PCF8812_LCD_LINE - n_dig)%2, EMPTY_STR);
  PCF8812_Putline(str, 3);
  //view selected digit
  PCF8812_Set_Symb(corner_up, 2, (PCF8812_LCD_LINE - n_dig)/2 + n_dig - col);
  PCF8812_Set_Symb(corner_down, 4, (PCF8812_LCD_LINE - n_dig)/2 + n_dig - col);
  if(Button_Get(button_1))//move the input field to the left
    {
      //increase to the maximum and return to a minimum
      mul = (mul < max_mul) ? (mul * 10) : 1;//
      col = (col < n_dig) ? (col + 1) : 1;
    }
  if(Button_Get(button_2))//move the input field to the right
    {
      //decrease to the minimum and return to a maximum
      mul = (mul > 1) ? (mul / 10) : max_mul;
      col = (col > 1) ? (col - 1) : n_dig;
    }
  if(Button_Get(user_button))//return result
    {
      PCF8812_Clear();
      return (result < min) ? min : result;//min limit of value
    }
  int8_t diff = Get_Enc_Diff();//read change of encoder position
  if(diff)//set result value
    {
      result += (mul * diff);
      diff = 0;
      result = (result > max) ? max : result;//max limit of value
    }
  PCF8812_DELAY;
  }
}

uint32_t PCF8812_Set_Param(Par_list* list) {
  RESET_ENC;
  Button_Set_Name(user_button, "OK");
  Button_Set_Name(button_1, "DOWN");
  Button_Set_Name(button_2, "UP");
  for(;;)
    {
      PCF8812_Clear();
      PCF8812_Title(list->name);
      uint8_t i = Get_Enc_Count(list->num - 1);
      if(Button_Get(button_1))
        DECR_ENC(1);
      if(Button_Get(button_2))
        INCR_ENC(1);
      if(Button_Get(user_button))//return result
        return list->item[i].code;
      PCF8812_Putline_Center(list->item[i].name, 4);
      PCF8812_DELAY;
    }
  return 0;
}

void PCF8812_Input_Time() {
  //time structure set to 01.01.00 00:00:00
  rtc_time_s temp = {.date = 1, .month = 1};
  char str[PCF8812_STR_SIZ];
  //array for choice of limit days of month
  uint8_t days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int8_t col = 7;//set cursor to input year
  uint8_t min = 0, max = 0, step = 1;//input digit parameters
  int8_t* ptr = NULL;//pointer for changed data
  Button_Set_Name(user_button, "OK");
  Button_Set_Name(button_1, " <");
  Button_Set_Name(button_2, "> ");
  for(;;)
    {
    PCF8812_Clear();
    PCF8812_Putline_Center("SET DATE & TIME", 0);
    Get_Time_String(&temp, str, view_all);
    PCF8812_Putline_Center(str, 3);
    PCF8812_Putline("dd.mm.yy HH:MM:SS", 5);//view data & time format
    //view selected digit
    PCF8812_Set_Symb(corner_up, 2, col);
    PCF8812_Set_Symb(corner_down, 4, col);
    if(Button_Get(button_1))
      {
        col--;//move the input field to the left
        if(col % 3 == 2)//jump across separator
          col--;
        if(col < 0)//move on circle
          col = PCF8812_LCD_LINE - 1;
      }
    if(Button_Get(button_2))
      {
        col++;//move the input field to the right
        if(col % 3 == 2)//jump across separator
          col++;
        if(col > PCF8812_LCD_LINE - 1)//move on circle
          col = 0;
      }
    if(Button_Get(user_button))//set time
      {
        Set_Time(&temp);//set time
        return;
      }
    switch(col)
    {
      case 0: case 1://change date
        ptr = &temp.date;//set pointer to changed data
        //set limits for changed data
        min = 1;
        max = days[temp.month];
        if(temp.month == 2 && temp.year % 4 == 0)//leap year
          max++;
        step = (col == 1) ? 1 : 10;//set step to 1 or 10 according unit or tens
        break;
      case 3: case 4://change month
        ptr = &temp.month;
        min = 1;
        max = 12;
        step = (col == 4) ? 1 : 10;
        break;
      case 6: case 7://change year
        ptr = &temp.year;
        min = 0;
        max = 99;
        step = (col == 7) ? 1 : 10;
        break;
      case 9: case 10://change hour
        ptr = &temp.hour;
        min = 0;
        max = 23;
        step = (col == 10) ? 1 : 10;
        break;
      case 12: case 13://change minute
        ptr = &temp.min;
        min = 0;
        max = 59;
        step = (col == 13) ? 1 : 10;
        break;
      case 15: case 16://change second
        ptr = &temp.sec;
        min = 0;
        max = 59;
        step = (col == 16) ? 1 : 10;
        break;
    }
    int8_t dir = Get_Enc_Diff();//read encoder rotate
    *ptr = (dir > 0) ? *ptr + step ://increase value
           (dir < 0) ? *ptr - step ://decrease value
            *ptr;
    //transition from min to max and back
    *ptr = (*ptr > max) ? min :
           (*ptr < min) ? max :
            *ptr;
    PCF8812_DELAY;
    }
}

uint32_t PCF8812_Counter = PCF8812_COUNT_MAX;

void PCF8812_On() {
  PCF8812_Counter = PCF8812_COUNT_MAX;
  PCF8812_NEED_FLUSH();
  PCF8812_LIGHT_ON();
}

void PCF8812_Off() {
  PCF8812_LIGHT_OFF();
}

void PCF8812_Count() {
  if(PCF8812_Counter)
      PCF8812_Counter--;
  else
      PCF8812_Off();
}

