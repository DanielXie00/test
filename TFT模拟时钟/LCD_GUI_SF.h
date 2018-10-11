#ifndef __LCD_GUI_SF_H_
#define	__LCD_GUI_SF_H_

#include "stdlib.h"
#include "stdint.h" 
#include "wpa/common.h"



 void putpixel(uint8_t x, uint8_t y, uint16_t color);
 void load_image(void);
void LCD_Screen_Show(void);
void LCD_ScreenFill(uint16_t color);
uint16_t getpixel(u8 x,u8 y);
void LCD_fstry(void);
void LCD_Show(void);

void LCD_draw_circle_8(uint8_t xc, uint8_t yc, uint8_t x, uint8_t y, uint16_t c);
void LCD_gui_circle(uint8_t xc, uint8_t yc,uint16_t c,uint8_t r, int fill);
void LCD_clearn_fill(uint16_t color);
void LCD_showimage(u8 x,u8 y,const unsigned char *p); //œ‘ æ40*40 QQÕº∆¨
void LCD_draw_color_box(u8 x,u8 y,uint16_t color,u8 B);

void LCD_show_big_image(u8 x,u8 y,const unsigned char *p,u8 B) ;//œ‘ æ40*40 QQÕº∆¨
void show_3D_xuanzhuan(uint8_t XO,uint8_t YO,uint8_t pic);
void ShowScreen_SF(void);



void load_clockimage();


#endif
