




#include <stdio.h>
#include <string.h>

//#include "board.h"
//#include "sdram.h"
#include "lcd.h"
#include "lcd_gui_sf.h"
#include "FONT.h"

//#include "Display_3D.h" //3D鏄剧ず绯诲垪鐨勫ご鏂囦欢锟�
//#include "front.h"

#define LOGO_WIDTH							110
#define LOGO_HEIGHT							42
#define FRAMEBUFFER_ADDR       0x28000000
#define length					128*128



uint16_t screen[128][128]={0};
	

extern const unsigned char image[];
uint16_t pixel_max_x,pixel_max_y;
///***************************************/
//鍑芥暟锛� putpixel

//杈撳叆锛�
//杈撳嚭锛�

//澶囨敞锛�  杈瑰缂栧啓
//											

//浣滆�咃細	鎵у康鎵ф垬			2017/1/18 19:18
///***************************************/
 void putpixel(uint8_t x, uint8_t y, uint16_t color)
	{	x=x%128;
		y=y%128;		//鐢ㄤ簬瑙ｅ喅鍧愭爣澶т簬鏁扮粍涓嬫爣128鏃剁殑璺戦鐜拌薄
		screen[x][y]=color;
		pixel_max_x=x;
		pixel_max_y=y;
	}
uint16_t getpixel(u8 x,u8 y)
{uint16_t cha;
	//cha=*(uint16_t *)(FRAMEBUFFER_ADDR + x+y*128 );
	cha=	screen[x][y];
	return cha;
}
 
 void load_image(void)
{	
		uint16_t i = 0, j = 0;
		/* Fill NXP logo */
		for(j=0; j<LOGO_HEIGHT; j++)
		{
				for(i=0; i<LOGO_WIDTH; i++)
				{
						putpixel(i, j, image[(i+j*LOGO_WIDTH)+LOGO_WIDTH*LOGO_HEIGHT*2]<<16|image[(i+j*LOGO_WIDTH)+LOGO_WIDTH*LOGO_HEIGHT]<<8 | image[(i+j*LOGO_WIDTH)]);
				}
		}
}

void LCD_ScreenFill(uint16_t color){
	uint8_t pix,piy;
	for(pix=0;pix<128;pix++)
		for(piy=0;piy<128;piy++)
		{
			putpixel(pix,piy,color);
		}

}
void LCD_Screen_Show()
{
	uint8_t x,y;
	if(	(pixel_max_x<64)&&(pixel_max_y<64))
		{
			LCD_SetWindows(0,0,63,63);
			for(x=0;x<64;x++)
				for(y=0;y<64;y++)
					LCD_WR_DATA_16Bit(screen[y][x]);
		}
		else if ((pixel_max_x>63)&&(pixel_max_y<64))
		{
			LCD_SetWindows(0,0,127,63);
			for(x=0;x<128;x++)
				for(y=0;y<64;y++)
					LCD_WR_DATA_16Bit(screen[y][x]);
			

			
		}
		else if ((pixel_max_x<64)&&(pixel_max_y>63))
		{
					
			LCD_SetWindows(0,0,127,63);
			for(x=0;x<128;x++)
				for(y=0;y<64;y++)
					{LCD_WR_DATA_16Bit(screen[y][x]);}
			LCD_SetWindows(0,63,63,127);
			for(x=0;x<64;x++)
				for(y=63;y<127;y++)
					LCD_WR_DATA_16Bit(screen[y][x]);
		}
		else  if ((pixel_max_x>63)&&(pixel_max_y>63))
		{
			LCD_SetWindows(0,0,127,127);
			for(x=0;x<128;x++)
				for(y=0;y<128;y++)
					LCD_WR_DATA_16Bit(screen[y][x]);
		}
		
		

	
	
	

	
	
	
}


void LCD_Show(){
	uint8_t x,y;
		LCD_SetWindows(0,0,127,127);
			for(x=0;x<128;x++)
				for(y=0;y<128;y++)
					//LCD_WR_DATA_16Bit(image_fb[x+y*128]);
	{
		
		LCD_WR_DATA_16Bit(screen[y][x]);	}
}

void LCD_fstry()
	{
	uint8_t x,y;
	
	
				for(x=0;x<128;x++)
				for(y=0;y<128;y++)
					//LCD_WR_DATA_16Bit(image_fb[x+y*128]);
	{
		if(x>y)
			//screen[x][y]=GREEN;
		putpixel(x,y,GREEN);
		else
			//screen[x][y]=BLACK;
			putpixel(x,y,BLACK);
			}
	
		LCD_SetWindows(0,0,127,127);
			for(x=0;x<128;x++)
				for(y=0;y<128;y++)
					//LCD_WR_DATA_16Bit(image_fb[x+y*128]);
	{
		
		LCD_WR_DATA_16Bit(screen[x][y]);	}
	
	
	
	
}
	


void LCD_clearn_fill(uint16_t color)
{uint8_t x,y;
	for(x=0;x<128;x++)
				for(y=0;y<128;y++)
					//LCD_WR_DATA_16Bit(image_fb[x+y*128]);
	{
		
		screen[x][y]=color;	}
}




void LCD_draw_circle_8(uint8_t xc, uint8_t yc, uint8_t x, uint8_t y, uint16_t c)
{
	putpixel(xc + x, yc + y, c);

	putpixel(xc - x, yc + y, c);

	putpixel(xc + x, yc - y, c);

	putpixel(xc - x, yc - y, c);

	putpixel(xc + y, yc + x, c);

	putpixel(xc - y, yc + x, c);

	putpixel(xc + y, yc - x, c);

	putpixel(xc - y, yc - x, c);
}


void LCD_gui_circle(uint8_t xc, uint8_t yc,uint16_t c,uint8_t r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill) 
	{
		// 濡傛灉濉厖锛堢敾瀹炲績鍦嗭級
		while (x <= y) {
			for (yi = x; yi <= y; yi++)
				LCD_draw_circle_8(xc, yc, x, yi, c);

			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	} else 
	{
		// 濡傛灉涓嶅～鍏咃紙鐢荤┖蹇冨渾锛�
		while (x <= y) {
			LCD_draw_circle_8(xc, yc, x, y, c);
			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}


void LCD_showimage(u8 x,u8 y,const unsigned char *p) //鏄剧ず40*40 QQ鍥剧墖
{
  	int i,j,k,m; 
	unsigned char picH,picL;
	//Lcd_Clear(WHITE); //娓呭睆
	

			
			//LCD_SetWindows(x,y,x+40,y+40);//绐楀彛璁剧疆
		    for(i=0;i<40;i++)
			 {	
				 for(m=0;m<40;m++)
			 	picL=*(p+i*2*40+2*m);	//鏁版嵁浣庝綅鍦ㄥ墠
				picH=*(p+i*2*40+2*m+1);
			screen[x+i][y+m]	=picH<<8|picL;//纭欢SPI鐨勬暟鎹濂藉儚鍜屾ā鎷熺殑鏄浉鍙嶇殑锛屾敞鎰忎氦鎹鍜孡
																		//鍗筹細picL<<8|picH
				//LCD_WR_DATA_16Bit(picH<<8|picL);  						
			 }	
	
			
}

void LCD_draw_color_box(u8 x,u8 y,u16 color,u8 B)
{
	u8 i,m;
	//LCD_SetWindows(x,y,x+B,y+B);//绐楀彛璁剧疆
//	for(a=0;a<B*B+B;a++)
//	LCD_WR_DATA_16Bit(color); 

for(i=x;i<x+B;i++)
	for(m=y;m<y+B;m++)
	screen[i][m]=color;
	
}

/***************************************/
//鍑芥暟锛�     show_big_image
//
//杈撳叆锛�		x,y:鍥剧墖宸︿笂瑙掑潗鏍�
//				B  锛氭斁澶у�嶆暟
//杈撳嚭锛�
//
//澶囨敞锛�  杈瑰缂栧啓
//
//浣滆�咃細	鎵у康鎵ф垬			2017/4/4
/***************************************/

void LCD_show_big_image(u8 x,u8 y,const unsigned char *p,u8 B) //鏄剧ず40*40 QQ鍥剧墖
{
  	int i,j,k; 

	unsigned char picH,picL;
	//Lcd_Clear(WHITE); //娓呭睆
	

			
			
		    for(i=0;i<40*40;i++)
			 {	j=i/40;
				 k=i%40;
			 	picL=*(p+i*2);	//鏁版嵁浣庝綅鍦ㄥ墠
				picH=*(p+i*2+1);				
				LCD_draw_color_box(x+k*B,y+j*B,picH<<8|picL,B);//纭欢SPI鐨勬暟鎹濂藉儚鍜屾ā鎷熺殑鏄浉鍙嶇殑锛屾敞鎰忎氦鎹鍜孡
																							//鍗筹細picL<<8|picH
			 }	
	
			 
			
}
/********************************************************/
/*3D鏃嬭浆灞曠ず鍦嗭紝绔嬫柟浣� 鍜� 绗﹀彿
*/
//void show_3D_xuanzhuan(uint8_t XO,uint8_t YO,uint8_t pic)
//	
//{uint16_t turn;
//	
//				for(turn=0;turn<360;turn++)
//		{		
//			
//			
//			
//				if(turn==0)
//						{ RotatePic32X32(&SETICO[pic][0],2,359,4,WHITE,XO,YO);	//褰撹浆瀹屼竴鍦堝悗锛屾秷鍘绘渶鍚庣殑鐐�
//							RateCube(359,359,359,WHITE,XO,YO);	
//							Rotatecircle(0,359,0,30,WHITE,XO,YO);}
//				else  	{
//							RotatePic32X32(&SETICO[pic][0],2,turn-1,4,WHITE,XO,YO);//娑堝幓鍓嶄竴涓偣
//							RateCube(turn-1,turn-1,turn-1,WHITE,XO,YO);
//							Rotatecircle(0,turn-1,0,30,WHITE,XO,YO);}
//					
//				RateCube(turn,turn,turn,BLACK,XO,YO);			//鐢荤偣鍑芥暟銆傛秷鐐瑰嚱鏁版渶濂芥斁鍦ㄧ敾鐐瑰嚱鏁颁箣鍓嶃�傘�傘�傘��
//				RotatePic32X32(&SETICO[pic][0],2,turn,4,RED,XO,YO);
//				Rotatecircle(0,turn,0,30,BLUE,XO,YO);
//				LCD_Show();		
//							
//				//LCD_clearn_fill(WHITE);
//	
//}}

void ShowScreen_SF()
{
	int i,m;
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
	for(i=0;i<128;i++)
		MYShowScreen(&screen[i]);
}


/*************************************************************/
//  画模拟时钟表盘函数
//
/*************************************************************/
void load_clockimage()
{
	int i,m;
	for (i=0;i<128;i++)
		for(m=0;m<128;m++)
			screen[i][m]=gImage_monishizhong[i*128+m];
	ShowScreen_SF();
	}
