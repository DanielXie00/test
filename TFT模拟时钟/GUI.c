#include "lcd.h"
#include "string.h"
#include "FONT.h"

#include "wpa/common.h"
#include "mydelay.h"
#include "GUI.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STM32F103RBT6,主频72M  单片机工作电压3.3V
//QDtech-TFT液晶驱动 for STM32 IO模拟
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtech.net
//淘宝网站：http://qdtech.taobao.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567 
//手机:15989313508（冯工） 
//邮箱:QDtech2008@gmail.com 
//Skype:QDtech2008
//技术交流QQ群:324828016
//创建日期:2013/5/13
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
//******************************************************************
//函数名：  GUI_DrawPoint
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    GUI描绘一个点
//输入参数：x:光标位置x坐标
//        	y:光标位置y坐标
//			color:要填充的颜色
//返回值：  无
//修改记录：无
//******************************************************************
void GUI_DrawPoint1(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);//设置光标位置 
	LCD_WR_DATA_16Bit(color); 
}
//void LCD_DrawPoint(u16 x,u16 y)
//{
//	LCD_SetCursor(x,y);//设置光标位置
//	LCD_WR_DATA_16Bit(color);
//}
//******************************************************************
//函数名：  LCD_Fill
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    在指定区域内填充颜色
//输入参数：sx:指定区域开始点x坐标
//        	sy:指定区域开始点y坐标
//			ex:指定区域结束点x坐标
//			ey:指定区域结束点y坐标
//        	color:要填充的颜色
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{  	

	u16 i,j;			
	u16 width=ex-sx+1; 		//得到填充的宽度
	u16 height=ey-sy+1;		//高度
	LCD_SetWindows(sx,sy,ex-1,ey-1);//设置显示窗口
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		LCD_WR_DATA_16Bit(color);	//写入数据 	 
	}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口设置为全屏
}

//******************************************************************
//函数名：  LCD_DrawLine
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    GUI画线
//输入参数：x1,y1:起点坐标
//        	x2,y2:终点坐标 
//返回值：  无
//修改记录：无
//****************************************************************** 
void LCD_DrawLine1(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		GUI_DrawPoint1(uRow,uCol,color);//画点
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

//******************************************************************
//函数名：  LCD_DrawRectangle
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    GUI画矩形(非填充)
//输入参数：(x1,y1),(x2,y2):矩形的对角坐标
//返回值：  无
//修改记录：无
//******************************************************************  
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}  

//******************************************************************
//函数名：  LCD_DrawFillRectangle
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    GUI画矩形(填充)
//输入参数：(x1,y1),(x2,y2):矩形的对角坐标
//返回值：  无
//修改记录：无
//******************************************************************   
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_Fill(x1,y1,x2,y2,POINT_COLOR);

}
 
//******************************************************************
//函数名：  _draw_circle_8
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    8对称性画圆算法(内部调用)
//输入参数：(xc,yc) :圆中心坐标
// 			(x,y):光标相对于圆心的坐标
//         	c:填充的颜色
//返回值：  无
//修改记录：无
//******************************************************************  
void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
{
	GUI_DrawPoint1(xc + x, yc + y, c);

	GUI_DrawPoint1(xc - x, yc + y, c);

	GUI_DrawPoint1(xc + x, yc - y, c);

	GUI_DrawPoint1(xc - x, yc - y, c);

	GUI_DrawPoint1(xc + y, yc + x, c);

	GUI_DrawPoint1(xc - y, yc + x, c);

	GUI_DrawPoint1(xc + y, yc - x, c);

	GUI_DrawPoint1(xc - y, yc - x, c);
}

//******************************************************************
//函数名：  gui_circle
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    在指定位置画一个指定大小的圆(填充)
//输入参数：(xc,yc) :圆中心坐标
//         	c:填充的颜色
//		 	r:圆半径
//		 	fill:填充判断标志，1-填充，0-不填充
//返回值：  无
//修改记录：无
//******************************************************************  
void gui_circle(int xc, int yc,u16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill) 
	{
		// 如果填充（画实心圆）
		while (x <= y) {
			for (yi = x; yi <= y; yi++)
				_draw_circle_8(xc, yc, x, yi, c);

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
		// 如果不填充（画空心圆）
		while (x <= y) {
			_draw_circle_8(xc, yc, x, y, c);
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

//******************************************************************
//函数名：  LCD_ShowChar
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示单个英文字符
//输入参数：(x,y):字符显示位置起始坐标
//        	fc:前置画笔颜色
//			bc:背景颜色
//			num:数值（0-94）
//			size:字体大小
//			mode:模式  0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************  
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode)
{  
    u8 temp;
    u8 pos,t;
	u16 colortemp=POINT_COLOR;      
		   
	num=num-' ';//得到偏移后的值
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//设置单个文字显示窗口
	if(!mode) //非叠加方式
	{
		
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)LCD_WR_DATA(fc); 
				else LCD_WR_DATA(bc); 
				temp>>=1; 
				
		    }
			
		}	
	}else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {   
				POINT_COLOR=fc;              
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//画一个点    
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏    	   	 	  
} 

//******************************************************************
//函数名：  LCD_ShowChar
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示单个英文字符
//输入参数：(x,y):字符显示位置起始坐标
//        	fc:前置画笔颜色
//			bc:背景颜色
//			num:数值（0-94）
//			size:字体大小
//			mode:模式  0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************  
void LCD_ShowNum2412(u16 x,u16 y,u16 fc, u16 bc,u8 *p ,u8 size,u8 mode)
{  
    u16 temp;
    u8 pos,t;
	u16 colortemp=POINT_COLOR;      
	u16 x0=x;
	u16 y0=y; 
	u8 num=0;
	

    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {   

		if(x>(lcddev.width-1)||y>(lcddev.height-1)) 
		return;   
		num=*p;
		if(':'==num) //特殊字符":"
		num=10;	
		else if('.'==num)//特殊字符"."
		num=11;	
		else  //纯数字   	
		num=num-'0';   
		x0=x;
		    
		for(pos=0;pos<48;pos++)
		{
			temp=asc2_2412[num][pos];
			for(t=0;t<8;t++)
		    {   
				POINT_COLOR=fc;              
		        if(temp&0x80)LCD_DrawPoint(x,y);//画一个点  
				//else LCD_WR_DATA_16Bit(bc);   
		        temp<<=1; 
				x++;
				if((x-x0)==12)
				{
					x=x0;
					y++;
					break;
				}
		    }
		}
	if(num<10)
	x+=16;	//人为控制字距，使得排版更好看，原值为12
	else
	x+=8; //人为控制字距，使得排版更好看，原值为12

	y=y0;
    p++;
    }  
	POINT_COLOR=colortemp;	 	 	  
} 


//******************************************************************
//函数名：  LCD_ShowString
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示英文字符串
//输入参数：x,y :起点坐标	 
//			size:字体大小
//			*p:字符串起始地址
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************  	  
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode)
{         
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {   
		if(x>(lcddev.width-1)||y>(lcddev.height-1)) 
		return;     
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }  
} 

//******************************************************************
//函数名：  mypow
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    求m的n次方(gui内部调用)
//输入参数：m:乘数
//	        n:幂
//返回值：  m的n次方
//修改记录：无
//******************************************************************  
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//******************************************************************
//函数名：  LCD_ShowNum
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示单个数字变量值
//输入参数：x,y :起点坐标	 
//			len :指定显示数字的位数
//			size:字体大小(12,16)
//			color:颜色
//			num:数值(0~4294967295)
//返回值：  无
//修改记录：无
//******************************************************************  			 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,temp+'0',size,1); 
	}
} 

//******************************************************************
//函数名：  GUI_DrawFont16
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示单个16X16中文字体
//输入参数：x,y :起点坐标
//			fc:前置画笔颜色
//			bc:背景颜色	 
//			s:字符串地址
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//自动统计汉字数目
	
			
	for (k=0;k<HZnum;k++) 
	{
	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
	  { 	LCD_SetWindows(x,y,x+16-1,y+16-1);
		    for(i=0;i<16*2;i++)
		    {
				for(j=0;j<8;j++)
		    	{	
					if(!mode) //非叠加方式
					{
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_WR_DATA_16Bit(fc);
						else LCD_WR_DATA_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
						x++;
						if((x-x0)==16)
						{
							x=x0;
							y++;
							break;
						}
					}

				}
				
			}
			
			
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏  
} 

//******************************************************************
//函数名：  GUI_DrawFont24
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示单个24X24中文字体
//输入参数：x,y :起点坐标
//			fc:前置画笔颜色
//			bc:背景颜色	 
//			s:字符串地址
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//自动统计汉字数目
		
			for (k=0;k<HZnum;k++) 
			{
			  if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+24-1,y+24-1);
				    for(i=0;i<24*3;i++)
				    {
							for(j=0;j<8;j++)
							{
								if(!mode) //非叠加方式
								{
									if(tfont24[k].Msk[i]&(0x80>>j))	LCD_WR_DATA_16Bit(fc);
									else LCD_WR_DATA_16Bit(bc);
								}
							else
							{
								POINT_COLOR=fc;
								if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
								x++;
								if((x-x0)==24)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}
					
					
				}				  	
				continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
			}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏  
}

//******************************************************************
//函数名：  GUI_DrawFont32
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示单个32X32中文字体
//输入参数：x,y :起点坐标
//			fc:前置画笔颜色
//			bc:背景颜色	 
//			s:字符串地址
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//****************************************************************** 
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//自动统计汉字数目
	for (k=0;k<HZnum;k++) 
			{
			  if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+32-1,y+32-1);
				    for(i=0;i<32*4;i++)
				    {
						for(j=0;j<8;j++)
				    	{
							if(!mode) //非叠加方式
							{
								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_WR_DATA_16Bit(fc);
								else LCD_WR_DATA_16Bit(bc);
							}
							else
							{
								POINT_COLOR=fc;
								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
								x++;
								if((x-x0)==32)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}
					
					
				}				  	
				continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
			}
	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏  
} 

//******************************************************************
//函数名：  Show_Str
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示一个字符串,包含中英文显示
//输入参数：x,y :起点坐标
// 			fc:前置画笔颜色
//			bc:背景颜色
//			str :字符串	 
//			size:字体大小
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************    	   		   
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
{					
	u16 x0=x;							  	  
  	u8 bHz=0;     //字符或者中文 
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size)) 
			return; 
	        if(*str>0x80)bHz=1;//中文 
	        else              //字符
	        {          
		        if(*str==0x0D)//换行符号
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else
				{
					if(size==12||size==16)
					{  
					LCD_ShowChar(x,y,fc,bc,*str,size,mode);
					x+=size/2; //字符,为全字的一半 
					}
					else//字库中没有集成16X32的英文字体,用8X16代替
					{
					 	LCD_ShowChar(x,y,fc,bc,*str,16,mode);
						x+=8; //字符,为全字的一半 
					}
				} 
				str++; 
		        
	        }
        }else//中文 
        {   
			if(x>(lcddev.width-size)||y>(lcddev.height-size)) 
			return;  
            bHz=0;//有汉字库    
			if(size==32)
			GUI_DrawFont32(x,y,fc,bc,str,mode);	 	
			else if(size==24)
			GUI_DrawFont24(x,y,fc,bc,str,mode);	
			else
			GUI_DrawFont16(x,y,fc,bc,str,mode);
				
	        str+=2; 
	        x+=size;//下一个汉字偏移	    
        }						 
    }   
}

//******************************************************************
//函数名：  Gui_StrCenter
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    居中显示一个字符串,包含中英文显示
//输入参数：x,y :起点坐标
// 			fc:前置画笔颜色
//			bc:背景颜色
//			str :字符串	 
//			size:字体大小
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************   
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
{
	u16 len=strlen((const char *)str);
	u16 x1=(lcddev.width-len*8)/2;
	Show_Str(x+x1,y,fc,bc,str,size,mode);
} 
 
//******************************************************************
//函数名：  Gui_Drawbmp16
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示一副16位BMP图像
//输入参数：x,y :起点坐标
// 			*p :图像数组起始地址
//返回值：  无
//修改记录：无
//******************************************************************  
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p) //显示40*40 QQ图片
{
  	int i; 
	unsigned char picH,picL; 
	LCD_SetWindows(x,y,x+40-1,y+40-1);//窗口设置
    for(i=0;i<40*40;i++)
	{	
	 	picL=*(p+i*2);	//数据低位在前
		picH=*(p+i*2+1);				
		LCD_WR_DATA_16Bit(picH<<8|picL);  						
	}	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复显示窗口为全屏	

}



unsigned char LCD_SCREEN[128][8]={0};



void LCD_SetDot_FS(unsigned char x,unsigned char y,unsigned char color)
{
	 unsigned char  pos ,bx,temp=0;

       if(x>127||y>63)return;//超出范围

        pos=y/8;

       bx=y%8;

       temp=1<<(bx);

       if(color==1)LCD_SCREEN[x][pos]|=temp;

       else LCD_SCREEN[x][pos]&=~temp;      
}
unsigned char LCD_GetDot_FS(unsigned char  x,unsigned char y)
{
	 unsigned char  pos ,bx,temp=0;

       //if(x>127||y>63)return -1;//超出范围

        pos=y/8;

       bx=y%8;

       temp=1<<(bx);

       if(LCD_SCREEN[x][pos]&temp)
				 return 1;

       else return 0;      
}

void LCD_Screen_Display(int screen)
{
	
	
		unsigned char  m=0,n=0,z=0;
	if(screen==1)
	{LCD_SetWindows(0,0,127,63);//窗口设置
		
		}
else 
	LCD_SetWindows(0,63,127,127);//窗口设置
	

	for (m=0;m<64;m++){			//一直没解决的只花上半部分不画下半部分的问题在加了花括号后解决了，以后要规范写程序
		
		for(n=0;n<128;n++)
			{
			
				
				if(LCD_GetDot_FS(n,m))
					LCD_WR_DATA_16Bit(POINT_COLOR);
				else
					LCD_WR_DATA_16Bit(BACK_COLOR);
				
			}
		
			}			
	
	
}


void LCD_CLS_SF(void)//清屏
{
	unsigned char  m,n;
	for(m=0;m<128;m++)
	for(n=0;n<8;n++)
	
	LCD_SCREEN[m][n]=0;
	
}



void _draw_circle_8_SF(int xc, int yc, int x, int y, u16 c)
{
	LCD_SetDot_FS(xc + x, yc + y, c);

	LCD_SetDot_FS(xc - x, yc + y, c);

	LCD_SetDot_FS(xc + x, yc - y, c);

	LCD_SetDot_FS(xc - x, yc - y, c);

	LCD_SetDot_FS(xc + y, yc + x, c);

	LCD_SetDot_FS(xc - y, yc + x, c);

	LCD_SetDot_FS(xc + y, yc - x, c);

	LCD_SetDot_FS(xc - y, yc - x, c);
}

void gui_circle_SF(int xc, int yc,u16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill) 
	{
		// 如果填充（画实心圆）
		while (x <= y) {
			for (yi = x; yi <= y; yi++)
				_draw_circle_8_SF(xc, yc, x, yi, c);

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
		// 如果不填充（画空心圆）
		while (x <= y) {
			_draw_circle_8_SF(xc, yc, x, y, c);
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




void showimage(u8 x,u8 y,const unsigned char *p) //显示40*40 QQ图片
{
  	int i,j,k; 
	unsigned char picH,picL;
	//Lcd_Clear(WHITE); //清屏  
	

			
			LCD_SetWindows(x,y,x+40,y+40);//窗口设置
		    for(i=0;i<40*40;i++)
			 {	
			 	picL=*(p+i*2);	//数据低位在前
				picH=*(p+i*2+1);				
				LCD_WR_DATA_16Bit(picH<<8|picL);  						
			 }	
	
			
}

void draw_color_box(u8 x,u8 y,u16 color,u8 B)
{
	u8 a;
	LCD_SetWindows(x,y,x+B,y+B);//窗口设置
	for(a=0;a<B*B+B;a++)
	LCD_WR_DATA_16Bit(color);  	
	
}

/***************************************/
//函数：     show_big_image
//
//输入：		x,y:图片左上角坐标
//				B  ：放大倍数
//输出：
//
//备注：  边学编写
//
//作者：	执念执战			2017/4/4
/***************************************/

void show_big_image(u8 x,u8 y,const unsigned char *p,u8 B) //显示40*40 QQ图片
{
  	int i,j,k; 

	unsigned char picH,picL;
	//Lcd_Clear(WHITE); //清屏  
	

			
			
		    for(i=0;i<40*40;i++)
			 {	j=i/40;
				 k=i%40;
			 	picL=*(p+i*2);	//数据低位在前
				picH=*(p+i*2+1);				
				draw_color_box(x+k*B,y+j*B,picH<<8|picL,B);  						
			 }	
	
			 
			
}







/****************************************************************************
* 名称：GUI_HLine()
* 功能：画水平线。
* 入口参数： x0     水平线起点所在列的位置
*           y0      水平线起点所在行的位置
*           x1      水平线终点所在列的位置
*           color   显示颜色(对于黑白色LCM，为0时灭，为1时显示)
* 出口参数：无
* 说明：对于单色、4级灰度的液晶，可通过修改此函数作图提高速度，如单色LCM，可以一次更
*      新8个点，而不需要一个点一个点的写到LCM中。
****************************************************************************/
void  GUI_HLine(u16 x0, u8 y0, u16 x1, u16 color)
{
    u8  temp;
    if(x0>x1)               // 对x0、x1大小进行排列，以便画图
    {
        temp = x1;
        x1 = x0;
        x0 = temp;
    }
    do
    {
    	GUI_DrawPoint1(x0, y0, color);   // 逐点显示，描出垂直线
        x0++;
    }
    while(x1>=x0);
}
/****************************************************************************
* 名称：GUI_RLine()
* 功能：画垂直线。
* 入口参数： x0     垂直线起点所在列的位置
*           y0      垂直线起点所在行的位置
*           y1      垂直线终点所在行的位置
*           color   显示颜色
* 出口参数：无
* 说明：对于单色、4级灰度的液晶，可通过修改此函数作图提高速度，如单色LCM，可以一次更
*      新8个点，而不需要一个点一个点的写到LCM中。
****************************************************************************/
void  GUI_RLine(u16 x0, u8 y0, u8 y1, u16 color)
{
    u8  temp;
    if(y0>y1)       // 对y0、y1大小进行排列，以便画图
    {
        temp = y1;
        y1 = y0;
        y0 = temp;
    }
    do
    {
    	GUI_DrawPoint1(x0, y0, color);   // 逐点显示，描出垂直线
        y0++;
    }
    while(y1>=y0);
}


/****************************************************************************
* 名称：GUI_LineWith()
* 功能：画任意两点之间的直线，并且可设置线的宽度。
* 入口参数： x0		直线起点的x坐标值
*           y0		直线起点的y坐标值
*           x1      直线终点的x坐标值
*           y1      直线终点的y坐标值
*           with    线宽(0-50)
*           color	显示颜色
* 出口参数：无
* 说明：操作失败原因是指定地址超出有效范围。
****************************************************************************/
void  GUI_LineWith(u16 x0, u16 y0, u16 x1, u16 y1, u8 with, u16 color)
{
	//注意数据类型的定义  要是用 u16则会因为无法取负数而陷入死循环出现bug
	s16   dx;						// 直线x轴差值变量
	s16   dy;          			// 直线y轴差值变量
   s8    dx_sym;					// x轴增长方向，为-1时减值方向，为1时增值方向
   s8    dy_sym;					// y轴增长方向，为-1时减值方向，为1时增值方向
   s16   dx_x2;					// dx*2值变量，用于加快运算速度
   s16   dy_x2;					// dy*2值变量，用于加快运算速度
   s16   di;						// 决策变量

   s16   wx, wy;					// 线宽变量
   s16   draw_a, draw_b;

   /* 参数过滤 */
   if(with==0) return;
     if(with>50) with = 50;

     dx = x1-x0;						// 求取两点之间的差值
     dy = y1-y0;

     wx = with/2;
     wy = with-wx-1;

     /* 判断增长方向，或是否为水平线、垂直线、点 */
     if(dx>0)							// 判断x轴方向
     {  dx_sym = 1;					// dx>0，设置dx_sym=1
     }
     else
     {  if(dx<0)
        {  dx_sym = -1;				// dx<0，设置dx_sym=-1
        }
        else
        {  /* dx==0，画垂直线，或一点 */
           wx = x0-wx;
           if(wx<0) wx = 0;
           wy = x0+wy;

           while(1)
           {  x0 = wx;
              GUI_RLine(x0, y0, y1, color);
              if(wx>=wy) break;
              wx++;
           }

        	 return;
        }
     }

     if(dy>0)							// 判断y轴方向
     {  dy_sym = 1;					// dy>0，设置dy_sym=1
     }
     else
     {  if(dy<0)
        {  dy_sym = -1;				// dy<0，设置dy_sym=-1
        }
        else
        {  /* dy==0，画水平线，或一点 */
           wx = y0-wx;
           if(wx<0) wx = 0;
           wy = y0+wy;

           while(1)
           {  y0 = wx;
              GUI_HLine(x0, y0, x1, color);
              if(wx>=wy) break;
              wx++;
           }
        	 return;
        }
     }

     /* 将dx、dy取绝对值 */
     dx = dx_sym * dx;
     dy = dy_sym * dy;

     /* 计算2倍的dx及dy值 */
     dx_x2 = dx*2;
     dy_x2 = dy*2;

     /* 使用Bresenham法进行画直线 */
     if(dx>=dy)						// 对于dx>=dy，则使用x轴为基准
     {  di = dy_x2 - dx;
        while(x0!=x1)
        {  /* x轴向增长，则宽度在y方向，即画垂直线 */
           draw_a = y0-wx;
           if(draw_a<0) draw_a = 0;
           draw_b = y0+wy;
           GUI_RLine(x0, draw_a, draw_b, color);

           x0 += dx_sym;
           if(di<0)
           {  di += dy_x2;			// 计算出下一步的决策值
           }
           else
           {  di += dy_x2 - dx_x2;
              y0 += dy_sym;
           }
        }
        draw_a = y0-wx;
        if(draw_a<0) draw_a = 0;
        draw_b = y0+wy;
        GUI_RLine(x0, draw_a, draw_b, color);
     }
     else								// 对于dx<dy，则使用y轴为基准
     {  di = dx_x2 - dy;
        while(y0!=y1)
        {  /* y轴向增长，则宽度在x方向，即画水平线 */
           draw_a = x0-wx;
           if(draw_a<0) draw_a = 0;
           draw_b = x0+wy;
           GUI_HLine(draw_a, y0, draw_b, color);

           y0 += dy_sym;
           if(di<0)
           {  di += dx_x2;
           }
           else
           {  di += dx_x2 - dy_x2;
              x0 += dx_sym;
           }
        }
        draw_a = x0-wx;
        if(draw_a<0) draw_a = 0;
        draw_b = x0+wy;
        GUI_HLine(draw_a, y0, draw_b, color);
     }

  }


void showiclockimage() //显示模拟时钟表盘图片
{
u32 i,j,k;
	unsigned char picH,picL;
	//Lcd_Clear(WHITE); //清屏



			LCD_SetWindows(0,0,127,127);//窗口设置
		    for(i=0;i<128*128;i++)
			 {
			 	picL=*(gImage_monishizhong+i*2);	//数据低位在前
				picH=*(gImage_monishizhong+i*2+1);
				LCD_WR_DATA_16Bit(picH<<8|picL);
			 }



}



