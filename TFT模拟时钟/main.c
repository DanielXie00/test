/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "wpa/common.h"
#include "lcd.h"
#include "GUI.h"
#include "LCD_GUI_SF.h"
//#include "FONT.h"
#include "mydelay.h"
#include "clock_try.h"


#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "timer_clock.h"

//const unsigned char gImage_qq23[3200] = { /*0X00,0X10,0X28,0X00,0X28,0X00,0X01,0X1B,*/
//0XDB,0XDE,0X5D,0XEF,0X5D,0XEF,0X7D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,
//0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0XBE,0XF7,
//0XFF,0XFF,0XDE,0XFF,0X38,0XC6,0X92,0X8C,0X8E,0X6B,0X6E,0X6B,0X10,0X7C,0X96,0XAD,
//0X3C,0XE7,0XFF,0XFF,0XFF,0XFF,0X7D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,
//0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,0X5D,0XEF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X5D,0XEF,
//0X15,0X9D,0X4F,0X63,0X6C,0X42,0X0A,0X32,0X88,0X29,0X46,0X19,0X25,0X19,0X45,0X21,
//0XE8,0X31,0X8E,0X6B,0X18,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0XFF,0XF5,0X9C,0X0F,0X53,
//0X10,0X4B,0X51,0X53,0X0F,0X4B,0X6C,0X3A,0XE9,0X31,0X67,0X21,0X25,0X19,0XE4,0X10,
//0XA3,0X08,0X62,0X00,0X83,0X08,0X8A,0X4A,0X59,0XC6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X1C,0XE7,0X4F,0X5B,0XB3,0X63,0XB8,0X7C,
//0XF5,0X63,0X11,0X43,0X4D,0X32,0XEA,0X29,0X88,0X21,0X26,0X19,0X05,0X19,0X05,0X19,
//0X04,0X11,0X04,0X11,0XE4,0X10,0X83,0X00,0XA3,0X08,0X72,0X8C,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDB,0XDE,0X4B,0X3A,0XF0,0X42,0X35,0X6C,0X54,0X4B,
//0XB1,0X32,0X2E,0X2A,0XEB,0X21,0XA9,0X21,0X67,0X19,0X05,0X19,0X04,0X11,0X04,0X11,
//0X04,0X11,0X04,0X11,0X04,0X11,0X05,0X19,0XE4,0X10,0X42,0X00,0XAF,0X73,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0X5D,0XEF,0X09,0X32,0X4C,0X32,0X10,0X4B,0X8F,0X32,0X4F,0X2A,
//0X2E,0X2A,0XCC,0X19,0X89,0X19,0X89,0X21,0X47,0X19,0X05,0X19,0X04,0X11,0X04,0X11,
//0XC4,0X10,0XC4,0X10,0X04,0X11,0X04,0X11,0X04,0X11,0XE4,0X10,0X42,0X00,0X31,0X84,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XEC,0X52,0X47,0X19,0X4C,0X32,0X0B,0X2A,0XEC,0X21,0XEC,0X21,
//0X0C,0X22,0X91,0X5B,0XEE,0X4A,0X06,0X11,0X26,0X19,0X04,0X19,0XE4,0X10,0XE4,0X10,
//0XA7,0X29,0X66,0X21,0XA3,0X08,0X05,0X19,0X04,0X11,0X04,0X11,0XE4,0X10,0X82,0X00,
//0XF7,0XBD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0X35,0XA5,0X83,0X08,0X88,0X21,0X88,0X21,0X89,0X21,0XAA,0X21,0X8A,0X21,
//0X6B,0X42,0X71,0X8C,0XFF,0XFF,0X72,0X8C,0X83,0X08,0X04,0X11,0XC4,0X08,0X29,0X42,
//0XFB,0XDE,0X5D,0XEF,0XEC,0X5A,0X83,0X08,0X04,0X11,0X04,0X11,0X04,0X11,0X83,0X08,
//0XE8,0X31,0XDF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XBE,0XF7,0XC7,0X31,0XC4,0X10,0X25,0X19,0X26,0X19,0X47,0X19,0X47,0X19,0XA8,0X29,
//0X8A,0X52,0X28,0X4A,0X55,0XAD,0XFF,0XFF,0XE8,0X31,0XA3,0X08,0X05,0X19,0X4D,0X6B,
//0X4D,0X6B,0XFF,0XFF,0X7D,0XEF,0X45,0X21,0XC4,0X10,0X04,0X11,0X04,0X11,0X04,0X11,
//0X62,0X00,0X76,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X96,0XB5,0X62,0X00,0X04,0X11,0X04,0X19,0X05,0X11,0X05,0X19,0XC4,0X08,0X8B,0X4A,
//0XB6,0XB5,0X5D,0XEF,0XF7,0XBD,0XFF,0XFF,0X8E,0X6B,0X62,0X00,0X29,0X42,0XAA,0X5A,
//0X08,0X42,0XFF,0XFF,0XFF,0XFF,0XCC,0X52,0X83,0X08,0X04,0X11,0X04,0X11,0X04,0X11,
//0XA3,0X08,0XAD,0X52,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XBE,0XF7,0XFF,0XFF,0XDF,0XFF,0XDB,0XDE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X4E,0X63,0X62,0X00,0X04,0X11,0X04,0X11,0X04,0X11,0XE4,0X10,0X62,0X00,0X8E,0X63,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCF,0X73,0X01,0X00,0XF3,0X9C,0X2C,0X63,
//0X96,0XB5,0XFF,0XFF,0XFF,0XFF,0X2D,0X5B,0X83,0X00,0X04,0X11,0X04,0X11,0X04,0X11,
//0XE4,0X10,0X67,0X21,0X3D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XB7,0XB5,0X76,0XAD,0X7D,0XEF,0XCB,0X52,0XB3,0X94,0XFF,0XFF,0XFF,0XFF,0XDF,0XFF,
//0XE8,0X31,0XA3,0X08,0X04,0X11,0X04,0X11,0X04,0X11,0X04,0X11,0XA3,0X08,0X49,0X42,
//0XFF,0XF7,0XFF,0XF7,0XFF,0XFF,0XFF,0XFF,0X6A,0X4A,0X01,0X00,0X72,0X84,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XF7,0XDF,0XEF,0X09,0X3A,0XA3,0X08,0X04,0X11,0X04,0X11,0X04,0X11,
//0X04,0X11,0X05,0X11,0X18,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XF0,0X7B,0X62,0X00,0XE8,0X31,0XC7,0X31,0X41,0X00,0X35,0XA5,0XFF,0XFF,0X5D,0XEF,
//0X46,0X21,0XC4,0X10,0X04,0X11,0X04,0X11,0X04,0X11,0X04,0X11,0XE4,0X10,0XA3,0X08,
//0X76,0X9D,0XFF,0XF7,0XFF,0XFF,0XB7,0XAD,0XA3,0X08,0XA3,0X08,0XC7,0X31,0X9E,0XE7,
//0XFF,0XF7,0XFF,0XF7,0X76,0XA5,0XA3,0X08,0XE4,0X10,0X04,0X11,0X04,0X11,0X04,0X11,
//0X05,0X11,0X05,0X11,0X35,0XA5,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XDB,0XDE,0XA7,0X29,0X83,0X00,0XC4,0X10,0XC4,0X10,0XC4,0X10,0X1C,0XE7,0X9E,0XEF,
//0X05,0X11,0XE4,0X10,0X04,0X11,0X04,0X11,0X04,0X11,0X04,0X11,0X04,0X19,0XC4,0X08,
//0XE5,0X10,0XD1,0X6B,0XD1,0X6B,0XC5,0X08,0X64,0X00,0XA5,0X08,0X43,0X00,0X2B,0X32,
//0X77,0X9D,0XB3,0X84,0X25,0X19,0XC4,0X10,0X04,0X11,0X04,0X11,0X04,0X11,0X04,0X11,
//0X25,0X19,0X26,0X09,0X35,0X9D,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XAF,0X73,0X62,0X00,0X04,0X19,0X05,0X19,0X82,0X00,0X0D,0X5B,0X8E,0X9B,
//0X62,0X10,0X05,0X11,0X04,0X11,0X04,0X11,0X04,0X19,0XE4,0X10,0X85,0X00,0X05,0X11,
//0XC4,0X39,0X81,0X5A,0X40,0X7B,0X22,0X9C,0X43,0XAC,0X03,0XA4,0X83,0X9B,0X82,0X72,
//0X82,0X49,0XC2,0X18,0XA4,0X00,0XC5,0X00,0XE4,0X10,0X04,0X19,0X04,0X11,0X05,0X19,
//0X47,0X19,0X67,0X11,0XEC,0X5A,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XDF,0XFF,0XDB,0XD6,0XC4,0X10,0XE4,0X10,0X04,0X11,0X05,0X11,0XA4,0X18,0X01,0XC0,
//0X83,0X88,0XE4,0X00,0X05,0X19,0X04,0X19,0XC5,0X08,0X44,0X21,0X43,0X83,0X23,0XD5,
//0X42,0XFE,0XE4,0XFE,0X27,0XFF,0X07,0XFF,0XA4,0XFE,0X64,0XFE,0X03,0XFE,0XA3,0XFD,
//0XE2,0XFC,0X42,0XEC,0X83,0XB3,0X24,0X62,0XE5,0X10,0XC4,0X08,0X04,0X19,0X26,0X19,
//0XA8,0X19,0X87,0X21,0X00,0X90,0XD3,0XBC,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XF0,0X7B,0X42,0X00,0X05,0X19,0X05,0X11,0X83,0X28,0X01,0XD0,
//0X44,0XF8,0XA3,0X48,0XE4,0X00,0XC5,0X08,0X44,0X5A,0X02,0XED,0XE2,0XFD,0X02,0XFE,
//0X66,0XFE,0X74,0XFF,0XB8,0XFF,0X73,0XFF,0XE7,0XF6,0XA6,0XF6,0X45,0XF6,0XA4,0XF5,
//0XC3,0XFC,0X62,0XFC,0XC2,0XFC,0XC2,0XFC,0XE3,0XCB,0XC4,0X49,0X06,0X11,0X88,0X19,
//0X87,0X01,0XA4,0X90,0X01,0XF8,0XEC,0X9A,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0X9E,0XF7,0XE8,0X31,0X83,0X00,0X05,0X09,0X82,0X40,0X01,0XC0,
//0X23,0XF8,0X85,0XF0,0XA3,0X48,0XA4,0X00,0X44,0X5A,0X02,0XFD,0X23,0XCC,0XC2,0XDC,
//0X04,0XFE,0X28,0XFE,0X48,0XF6,0X46,0XF6,0X24,0XF6,0XE4,0XF5,0X64,0XFD,0XE3,0XFC,
//0X62,0XFC,0XC2,0XFC,0X02,0XE4,0X02,0XDC,0XE2,0XFC,0XA4,0X7A,0X48,0X01,0X67,0X01,
//0XC4,0X78,0X24,0XF8,0X02,0XF8,0X84,0XB0,0X7D,0XE7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XDB,0XDE,0X25,0X19,0XA3,0X00,0XC4,0X38,0X02,0XE0,
//0X22,0XD8,0X44,0XF8,0XA6,0XF8,0XA4,0X78,0X63,0X00,0X43,0X21,0X83,0X72,0X83,0X39,
//0X82,0X9B,0X21,0XF5,0X61,0XFD,0X22,0XFD,0XE2,0XFC,0XA2,0XFC,0X42,0XFC,0X42,0XFC,
//0X42,0XFC,0X22,0XAB,0X83,0X41,0XC3,0X92,0X04,0X52,0X26,0X01,0X25,0X19,0XA4,0X98,
//0X44,0XF8,0X23,0XF8,0X02,0XF8,0XA4,0XD0,0X9E,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X9A,0XD6,0X87,0X29,0XA5,0X00,0X43,0XB8,
//0X22,0XF8,0X23,0XE0,0X65,0XF8,0XE8,0XF8,0X07,0XC9,0X83,0X48,0X42,0X00,0XA3,0X00,
//0X84,0X00,0X63,0X29,0XA2,0X7A,0X62,0XB3,0XA2,0XCB,0X62,0XD3,0X02,0XBB,0X82,0X8A,
//0X83,0X39,0XA4,0X00,0XE5,0X00,0XE5,0X00,0XE5,0X08,0XC4,0X60,0X64,0XD8,0X44,0XF8,
//0X24,0XF8,0X23,0XF8,0X02,0XF8,0X83,0X88,0XDB,0XC6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X3D,0XE7,0X50,0X5B,0X08,0X31,
//0X23,0XE8,0X43,0XF8,0X44,0XF0,0X65,0XF8,0X09,0XF9,0XAB,0XF9,0X89,0XD1,0X06,0X89,
//0XA3,0X48,0X42,0X18,0X02,0X00,0X42,0X00,0X61,0X00,0X82,0X00,0X62,0X00,0X62,0X00,
//0X83,0X00,0XA3,0X20,0XC4,0X50,0XA5,0X88,0X85,0XD8,0X65,0XF8,0X44,0XF8,0X44,0XF8,
//0X23,0XF8,0X23,0XF8,0X03,0XD0,0X82,0X10,0XC7,0X29,0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X6C,0X32,
//0XA5,0X38,0X02,0XD8,0X23,0XF8,0X65,0XF8,0X66,0XF8,0XA7,0XF8,0X4A,0XF9,0X0C,0XFA,
//0X4D,0XFA,0X4C,0XEA,0X0B,0XD2,0XA9,0XB9,0X68,0XB1,0X47,0XA9,0X27,0XB1,0X07,0XB9,
//0X07,0XD1,0XE7,0XE8,0XC7,0XF8,0XA7,0XF8,0X65,0XF8,0X65,0XF8,0X44,0XF8,0X23,0XF8,
//0X03,0XF8,0X02,0XD0,0XA3,0X28,0X05,0X09,0XC4,0X08,0XEC,0X5A,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFB,0XDE,0X05,0X19,
//0XC4,0X00,0XA7,0X41,0XE6,0XC0,0X03,0XF8,0X86,0XF8,0XA7,0XF8,0X87,0XF8,0X86,0XF8,
//0XC7,0XF8,0X29,0XF9,0X8A,0XF9,0XAB,0XF9,0XAB,0XF9,0X8B,0XF9,0X6A,0XF9,0X29,0XF9,
//0X08,0XF9,0XC7,0XF8,0XA6,0XF8,0X86,0XF8,0X65,0XF8,0X64,0XF8,0X23,0XF8,0X02,0XF0,
//0X06,0XB1,0X25,0X29,0XE4,0X00,0XE4,0X10,0X25,0X19,0X25,0X19,0X14,0X9D,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X96,0XAD,0X62,0X00,
//0X82,0X08,0X35,0X95,0XBA,0XCE,0X8B,0XA2,0X44,0XD0,0X25,0XF8,0X87,0XF8,0XA7,0XF8,
//0XC7,0XF8,0XA7,0XF8,0X87,0XF8,0X86,0XF8,0X86,0XF8,0X86,0XF8,0X87,0XF8,0XA7,0XF8,
//0XA7,0XF8,0XA6,0XF8,0X85,0XF8,0X65,0XF8,0X64,0XF8,0X24,0XF0,0X64,0XB8,0X0D,0X93,
//0XBB,0XB6,0XCF,0X63,0X83,0X08,0X04,0X11,0XE4,0X10,0X66,0X21,0X49,0X3A,0X5D,0XEF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XD3,0X94,0X42,0X00,
//0XE4,0X10,0XBB,0XCE,0XFF,0XFF,0XBE,0XE7,0X76,0XB5,0XCC,0XAA,0X07,0XC1,0X45,0XE0,
//0X45,0XF8,0X46,0XF8,0X66,0XF8,0X86,0XF8,0X86,0XF8,0X86,0XF8,0X86,0XF8,0X65,0XF8,
//0X45,0XF8,0X65,0XF8,0X65,0XE8,0X44,0XD0,0X43,0XA8,0X01,0X88,0X82,0X90,0X3C,0XD7,
//0XFF,0XEF,0X55,0X95,0X83,0X08,0X04,0X11,0X04,0X11,0X05,0X19,0X46,0X19,0XB3,0X94,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB3,0X94,0X41,0X00,
//0X86,0X21,0X5D,0XDF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0XE7,0X7A,0XC6,0XD3,0XB4,
//0X4E,0XB3,0X2A,0XC2,0X68,0XD1,0XE6,0XE0,0XA6,0XE8,0XA5,0XE8,0XA5,0XE8,0XE6,0XD8,
//0X88,0XC9,0X06,0XA9,0X22,0XA8,0X02,0XA8,0X00,0XA0,0X00,0XC8,0X00,0XD8,0XF7,0XE5,
//0XFF,0XE7,0XF8,0XAD,0XC4,0X10,0XE4,0X10,0X04,0X11,0XE4,0X10,0X05,0X11,0X8B,0X4A,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X55,0XA5,0X41,0X00,
//0XA7,0X29,0X5D,0XDF,0XFF,0XF7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XEF,0X7D,0XDF,0XDB,0XCE,0X59,0XCE,0XF8,0XCD,0XD7,0XCD,0XF7,0XC5,0X79,0XCE,
//0XFB,0XBE,0XAB,0XA2,0X03,0XF0,0X45,0XF8,0X42,0XD0,0X43,0XE8,0X00,0XF0,0X72,0XD4,
//0XFF,0XDF,0X39,0XAE,0XE4,0X10,0XE4,0X10,0X04,0X11,0XE4,0X10,0X05,0X11,0X87,0X29,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X59,0XCE,0X83,0X08,
//0X46,0X21,0X1C,0XD7,0XFF,0XF7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XEF,0X8B,0XBA,0X04,0XF8,0X45,0XF8,0X62,0XE0,0X44,0XF0,0X00,0XF8,0X8E,0XDB,
//0XFF,0XDF,0XF8,0XA5,0XC4,0X10,0XE4,0X10,0XE4,0X10,0X04,0X11,0XE4,0X10,0X25,0X19,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBE,0XF7,0X87,0X29,
//0X83,0X08,0X39,0XB6,0XFF,0XF7,0XDF,0XF7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XBE,0XE7,0X4A,0XBA,0X03,0XF8,0X45,0XF8,0X64,0XF8,0X44,0XF8,0X00,0XF8,0X6E,0XE3,
//0XFF,0XD7,0XF4,0X8C,0X83,0X08,0X04,0X11,0XE4,0X10,0XE4,0X10,0XE4,0X10,0X05,0X19,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X73,
//0X00,0X00,0X72,0X84,0XFF,0XEF,0XBE,0XEF,0XDF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XDF,0XE7,0X8B,0XBA,0X03,0XF8,0X45,0XF8,0X45,0XF8,0X23,0XF8,0X00,0XF8,0XD3,0XD4,
//0XFF,0XD7,0X4E,0X5B,0X21,0X00,0X29,0X3A,0X55,0XA5,0X83,0X08,0XC4,0X10,0X25,0X19,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFB,0XDE,
//0XA3,0X08,0XE8,0X31,0X9E,0XDF,0X9E,0XE7,0XBF,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XF7,0X51,0XBC,0X02,0XE0,0X03,0XF8,0X03,0XF0,0X43,0XE0,0XEC,0XC2,0X7E,0XCF,
//0XFC,0XBE,0X46,0X21,0X21,0X00,0XD3,0X94,0XFF,0XFF,0X51,0X84,0X00,0X00,0X87,0X29,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X51,0X84,0X00,0X00,0XF4,0X8C,0XFF,0XEF,0X9E,0XE7,0XBF,0XEF,0XDF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0X3D,0XDF,0X55,0XBD,0X52,0XBC,0X72,0XBC,0XB7,0XB5,0X5D,0XC7,0XFF,0XDF,
//0XF0,0X6B,0X00,0X00,0X09,0X3A,0XBF,0XF7,0XFF,0XFF,0XFF,0XFF,0X14,0X9D,0X55,0XA5,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XFF,0XFF,0XAC,0X4A,0XA4,0X08,0XBB,0XBE,0XDF,0XE7,0X7E,0XE7,0XBE,0XEF,0XDF,0XF7,
//0XDF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0XDF,0XFF,0XDF,0XF7,0XFF,0XEF,0XDF,0XDF,0XBF,0XD7,0X9E,0XD7,0XDF,0XDF,0XD8,0XA5,
//0X83,0X08,0X26,0X11,0XDB,0XD6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X79,0XEE,0X8B,0XDC,0X21,0X31,0XA9,0X21,0X3D,0XCF,0XBF,0XDF,0X7E,0XDF,0X9E,0XE7,
//0XBE,0XEF,0XBF,0XEF,0XDF,0XF7,0XDF,0XF7,0XDF,0XF7,0XDF,0XF7,0XDF,0XF7,0XBF,0XEF,
//0XBE,0XEF,0X9E,0XE7,0X7E,0XDF,0X5E,0XD7,0X5E,0XD7,0XDF,0XDF,0X9A,0XB6,0X26,0X19,
//0X42,0X08,0XED,0XA3,0XBF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X74,0XDD,
//0XC0,0XDB,0X00,0XFE,0X42,0XEE,0X02,0X42,0X89,0X21,0X7B,0XB6,0XDF,0XDF,0X7E,0XD7,
//0X7E,0XDF,0X7E,0XDF,0X9E,0XE7,0X9E,0XE7,0X9E,0XE7,0X9E,0XE7,0X9E,0XE7,0X7E,0XDF,
//0X7E,0XDF,0X5D,0XD7,0X5D,0XD7,0X9E,0XDF,0XFF,0XE7,0XF8,0XA5,0X07,0X11,0XE3,0X18,
//0X02,0XC5,0X60,0XFD,0XE6,0XD3,0XDB,0XEE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X9E,0XF7,0X84,0XBA,
//0XC1,0XFC,0X42,0XFE,0X82,0XFE,0XA2,0XFE,0X81,0X83,0X45,0X21,0X74,0X74,0X5E,0XC7,
//0XDF,0XDF,0X7E,0XD7,0X5E,0XD7,0X5D,0XD7,0X5E,0XD7,0X5E,0XD7,0X5D,0XD7,0X5D,0XD7,
//0X5E,0XD7,0X9E,0XDF,0XFF,0XE7,0X3D,0XC7,0XF1,0X63,0X84,0X08,0X42,0X52,0X26,0XE6,
//0X29,0XFF,0X86,0XFE,0XE0,0XF3,0X6A,0XC3,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X18,0XDE,0XC1,0XD2,
//0XA2,0XFD,0X22,0XFE,0X42,0XFE,0X42,0XFE,0X62,0XFE,0XE2,0XD4,0X41,0X6A,0X49,0X42,
//0X53,0X74,0X3B,0XA6,0X3E,0XC7,0XBF,0XD7,0XBF,0XD7,0XBF,0XD7,0XBF,0XDF,0XBF,0XD7,
//0X3E,0XC7,0X1A,0XA6,0XF2,0X63,0XA7,0X29,0X82,0X41,0X22,0XB4,0X62,0XFE,0X83,0XFE,
//0XAA,0XFE,0X0F,0XFF,0X67,0XFD,0X63,0XBA,0X3C,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X9A,0XE6,0X80,0XD2,
//0X21,0XFD,0XC2,0XFD,0XE2,0XF5,0XC2,0XF5,0X82,0XF5,0X82,0XFD,0X62,0XFD,0X61,0XDC,
//0X21,0X9B,0X84,0X6A,0XE9,0X6A,0X2C,0X63,0XAF,0X63,0X11,0X74,0X6E,0X63,0X2C,0X63,
//0X89,0X5A,0X04,0X52,0X81,0X7A,0XC2,0XCB,0XE2,0XFC,0X62,0XFD,0X82,0XFD,0XC2,0XFD,
//0XC2,0XFD,0XE4,0XFD,0X24,0XFD,0X62,0XCA,0X1C,0XE7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB1,0XCC,
//0X81,0XD2,0XC0,0XF3,0XC1,0XFC,0X02,0XFD,0X02,0XFD,0XE2,0XFC,0XC2,0XFC,0XC2,0XFC,
//0X81,0XFC,0X80,0XFB,0XC0,0XC9,0XA4,0X81,0X35,0XAD,0X59,0XCE,0X71,0X9C,0X21,0X81,
//0X00,0XDA,0XA1,0XFB,0X82,0XFC,0XA2,0XFC,0X82,0XFC,0XA2,0XFC,0X02,0XFD,0X22,0XFD,
//0XE2,0XFC,0X00,0XFC,0X60,0XDA,0X90,0XCC,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X5D,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//0X59,0XDE,0X0D,0XC4,0X06,0XCB,0XE4,0XD2,0X03,0XDB,0X03,0XDB,0XE3,0XDA,0XC3,0XD2,
//0XA4,0XC2,0X09,0XB3,0XD2,0XBC,0X9E,0XF7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFB,0XE6,
//0X0E,0XB4,0XA6,0XBA,0X83,0XD2,0XE3,0XE2,0X02,0XEB,0X22,0XEB,0X22,0XE3,0X03,0XDB,
//0XE4,0XD2,0X6A,0XC3,0XB6,0XD5,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
//};



#define GPIO_OUTPUT_IO_LED   2
#define GPIO_OUTPUT_PIN_SEL  ((1<<GPIO_OUTPUT_IO_LED) )
//#define GPIO_INPUT_IO_TEST   5
//#define GPIO_INPUT_PIN_TEST  ((1<<GPIO_INPUT_IO_TEST) )

int i;

//
//
//
//static void timer_example_evt_task(void *arg)
//{
//    while(1) {
//        timer_event_t evt;
//        xQueueReceive(timer_queue, &evt, portMAX_DELAY);
//        if(evt.type == TEST_WITHOUT_RELOAD) {
//            printf("\n\n   example of count-up-timer \n");
//        } else if(evt.type == TEST_WITH_RELOAD) {
//            printf("\n\n   example of reload-timer \n");
//
//        }
//        /*Show timer event from interrupt*/
//        printf("-------INTR TIME EVT--------\n");
//        printf("TG[%d] timer[%d] alarm evt\n", evt.group, evt.idx);
//        printf("reg: ");
//        print_u64(evt.counter_val);
//        printf("time: %.8f S\n", evt.time_sec);
//        /*Read timer value from task*/
//        printf("======TASK TIME======\n");
//        uint64_t timer_val;
//        timer_get_counter_value(evt.group, evt.idx, &timer_val);
//        double time;
//        timer_get_counter_time_sec(evt.group, evt.idx, &time);
//        printf("TG[%d] timer[%d] alarm evt\n", evt.group, evt.idx);
//        printf("reg: ");
//        print_u64(timer_val);
//        printf("time: %.8f S\n", time);
//    }
//}







void app_main()
{	   // vTaskDelay(100/1);

	 //初始化GPIO引脚
	  gpio_config_t io_conf;
	    //disable interrupt
	    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
	    //set as output mode
	    io_conf.mode = GPIO_MODE_OUTPUT;
	    //bit mask of the pins that you want to set,e.g.GPIO18/19
	    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	    //disable pull-down mode
	    io_conf.pull_down_en = 0;
	    //disable pull-up mode
	    io_conf.pull_up_en = 0;
	    //configure GPIO with the given settings
	    gpio_config(&io_conf);
	    printf("LED init OK!\n");


	    LCD_Init();
	    LCD_Clear(RED);

//	    while(1)
//	    	{
//	    	if((i%2)==0){
//	    	LCD_Clear(GREEN);
//	    	printf("GREEN \n");}
//	    	else
//	    	{
//	    		LCD_Clear(RED);
//	    		printf("RED \n");
//	    	}
//	    	i++;
//	    	if(i==20)i=0;
//	    }
//

	    u8 x,y,z;
	    unsigned int num=0;
	    	char number[3];


	    	//delay_init(72);	     //延时初始化
	    	//LCD_Init();	   //液晶屏初始化
	    	LCD_Clear(BLACK); //清屏
	    	gui_circle(128/2,128/2,WHITE,128/2-2, 0); //画圆
	    	gui_circle(128/2,128/2,WHITE,128/2-3, 0);//画圆

	    	Gui_StrCenter(0,10,BLUE,YELLOW,&"SysParam",16,1);//居中显示
	    	Gui_StrCenter(0,28,RED,BLUE,&"Temp:32℃",16,1);//居中显示
	    	Gui_StrCenter(0,46,YELLOW,BLUE,&"Hum:20%",16,1);//居中显示
	    	Gui_StrCenter(0,64,WHITE,BLUE,&"VOL:3.2V",16,1);//居中显示
	    	Gui_StrCenter(0,82,GREEN,BLUE,&"I:1.25mA",16,1);//居中显示
	    	Gui_StrCenter(0,100,RED,BLUE,&"12:20AM",16,1);//居中显示
	    	//Set_time_now();
	    	rtc_date_set(2017,8,18,13,30,30);
	    	//LCD_Clear(WHITE);
//	    	GUI_LineWith(0,5,50,50,3,RED);
//	    	GUI_LineWith(64,5,30,70,15,GREEN);
	    	//IM_clock(64,64,64);
	    	showiclockimage();
	    	moniclock_set();
	    	while(1){

//	    		LCD_show_big_image(num,20,gImage_qq23,2);
//	    		load_image();
//	    		LCD_show_big_image(num,20,gImage_qq23,1);
//
//	    		LCD_Show();
//	    		LCD_clearn_fill(BLACK);

	    		//Get_time_now();
	    		//IMclock_move(64,64,64);
	    		delay_ms(50);
	    	num++;
	    		if (num>40)
	    			num=0;
	    	};



}




