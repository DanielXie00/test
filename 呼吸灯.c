/*-----------------------------------------------
  Project Name:Breathing LED
  Author：shuwei  
  Data:2012-11-17
  Breathing LEDs display during interruptting.
***调试心得(23:09)：
  1).应用了static变量，减少全局变量的使用
  2).标志位变量使用bit位操作定义，节省RAM的空间。但是bit定义变量应该是51所特有的.
  3).修改LED在main函数所赋的初值(0x00-->0xff)，修正了上电LED灯亮度的突变
------------------------------------------------*/

#include<reg52.h>

#define LED		P2 //定义LED灯端口
//#define SPEED	1 //设置每一个明亮级的停留时间 ；也即每个明亮级持续的时间
#define PWM		200 //设置明亮的总级数（级数过大会出现闪烁）（值域：0~65535）
#define MAX		150 //设置最大明亮（值域：小于明亮总级数）


void Init_Timer0(void);


void main (void)  //主程序 
{ 		
	Init_Timer0();
	LED=0xff;
	while(1);
		    
}


void Init_Timer0(void)
	{
		TMOD = 0x01;  //使用模式1，16位定时器
		EA=1;
		ET0=1;
		TR0=1;
	}

void Timer0_isr(void) interrupt 1  //定时器0中断程序，0.05ms中断一次
	{
		static unsigned int t=1;
		static unsigned int i;
		static bit LOOP,DELAY;     //LOOP是循环切换标志变量
		TH0=(65536-50)/256; //重新赋值 0.05ms
		TL0=(65536-50)%256;
		if(LOOP==0)  //变亮循环
		{								
			if(DELAY==0)
			{	
				i++;
				if(i==t)			
				{
					DELAY=1;
					LED=0xff;
					i=0;
				}
			}			
			if(DELAY==1)	
			{
				i++;
				if(i==(PWM-t))			
				{
					DELAY=0;
					LED=0x00;
					i=0;
					t++;
				}
			}			
			if(t>(MAX-2))
			{
				LOOP=1;
			}
		}
		if(LOOP==1)  //变暗循环
		{			
			if(DELAY==0)
			{	
				i++;
				if(i==t)			
				{
					DELAY=1;
					LED=0xff;
					i=0;
				}
			}			
			if(DELAY==1)	
			{
				i++;
				if(i==(PWM-t))			
				{
					DELAY=0;
					LED=0x00;
					i=0;
					t--;
				}
			}			
			if(t<2)
			{
				LOOP=0;
			}
		}		    	

	}	

/**************************************************************************
*函数名称：Timer0Isr()
*功能    ：定时器0中断函数
*入口参数：无
*出口参数：无
*说明    ：实现LED渐亮渐灭
**************************************************************************/
void Timer0Isr(void) interrupt 1
{   
    static uchar PwmCnt=0;       //PWM计数器
    static bit   PwmDirection=0; //亮灭方向标志位
    static char PwmDuty=100;     //占空比初始化为100
    if(++PwmCnt>=100)            //1个周期到，调整一次占空比
    {
         PwmCnt=0;
	
         if(PwmDirection==0)       //从全亮到灭
         {
              if(--PwmDuty<=0)     //占空比减1，为0则改变方向标志位
                  PwmDirection=1;                
         }
         else                      //从灭到全亮
         {
              if(++PwmDuty>=100)   //占空比加1，到100则该变方向标志位
                  PwmDirection=0;
         }  
     }
     if(PwmCnt<=PwmDuty)           //改变输出状态
         LedOn;
     else
         LedOff;
}

#include <reg51.h>
sbit led=P3^0;	 	
unsigned char loop,pwm;	 //loop从0到10循环变化
unsigned int i;
bit f;
int main()
{
	loop=0;
	pwm=4;
	f=0;
	while(1)
	{
		if (loop<pwm) 	//当loop小于pwm时，点亮LED
		{
			led=0;
		}
		else	 //当loop不小于pwm时，熄灭LED
		{
			led=1;
		}
		loop++;
		if(loop>10)
		{
			loop=0;
			i++;
			if(i==500)    //i的大小决定呼吸灯的节奏快慢
			{	
				i=0;
				if(!f) 
				{
					pwm++;
					if(pwm==10)
					{
						f=1;
					}
				}
				else
				{		
					pwm--;
					if(pwm==4)
					{
						f=0;
					}
				}
			}
		}
	}
}
