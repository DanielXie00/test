////////////////////////////////////////////////////////////
//PROJECT:   	keyboard.c
//AUTHOR:	  	mengjicheng
//EMAIL:		mengjicheng@163.com
//MODULE:    	keyboard.c
//REVISION:		v1
//START DATE:  	2012-5-16 10:59
//FINISH DATA:
//MODIFY:
////////////////////////////////////////////////////////////
#include "stm8l15x.h"
#include "key.h"
#include "device_init.h"
#include "stm8l15x_rtc.h"
#include "stm8sand1640.h"


#define  buzzer_on 	BEEP_Cmd(ENABLE)
#define  buzzer_off 	BEEP_Cmd(DISABLE)
u8 f_on_off=0;
u8 f_outsid_v=1;
u8 f_full_v=0;
u8 f_buzzer_on=0;
u8 f_celar_hcho_vb=0;
u8 f_pm25_hco2=0;//PM25/甲醛判断，为1：PM25,为0：甲醛
//u8 f_bbb_bz=0;
extern u8 LCDRAM[];
//后面加的定义
//u8 delay_back=0;
//u8 f_flash=0;
u8 f_mute=1;//蜂鸣器鸣叫选择位
//u8 time_ntc=0;
u8 f_am_pm_set=0;
extern u8 H2_H3_H4,H6_H7_H8;
u8 tune_mode=0;
//u8 level_led;
//u8 led_level;
//u8 sleep_mode;
u16 buzzer_pwm2;
extern u8 T2;
u16 Key_Touch =0;
u16 Key_Value =0;

////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////*
void in_sleep(void)
{
/*	sim()				//禁止中断,使优先级变为3级(I1=1 I0=1),这样才可设置下面的寄存器
//00：下降沿和低电平触发
//01：仅上升沿触发
//10：仅下降沿触发
//11：上升沿和下降沿触发
//P3IS[1:0]/76  P2IS[1:0]/54  P1IS[1:0]/32  P0IS[1:0]/10
	EXTI_CR1 =2<<4;			//对应每个I/O口0-3位触发沿设置
//P7IS[1:0]/76  P6IS[1:0]/54  P5IS[1:0]/32  P4IS[1:0]/10
	EXTI_CR2 =1<<6;			//对应每个I/O口4-7位触发沿设置
//PFIS[1:0]/76  PEIS[1:0]/54  PDIS[1:0]/32  PBIS[1:0]/10
	EXTI_CR3 =0x00;			//P.fedb下降沿触发
	rim()

//所有不使用的管脚都应当设置为输出低电平。绝对不能设置任何不用管脚为悬空输入状
//态，这会导致不必要的高功耗
	PA_DDR =0<<7|0<<6|1<<5|1<<4|1<<3|0<<2|1<<1|1<<0;//输出/输入设置
	PA_CR1 =1<<7|1<<6|0<<5|0<<4|0<<3|1<<2|0<<1|0<<0;//在输出状是开漏,在输入状是上拉
	PA_CR2 =1<<7|0<<6|0<<5|0<<4|0<<3|1<<2|0<<1|0<<0;//在输出状是速率,在输入状是开启触发中断
	PA_ODR =0<<1;

	PB_DDR =0xff;
	PB_CR1 =0x00;
	PB_CR2 =0x00;
	PB_ODR =0x00;

	PC_DDR =0xff;	//PC3/UART1_TX  PC2/UART1_RX
	PC_CR1 =0x00;
	PC_CR2 =0x00;
	PC_ODR =0x00;

	PD_DDR =0xff;
	PD_CR1 =0x00;
	PD_CR2 =0x00;
	PD_ODR =0x00;

	PE_DDR =0xff;
	PE_CR1 =0x00;
	PE_CR2 =0x00;
	PE_ODR =0x00;

	PF_DDR =1<<0;  	//只有PF0
	PF_CR1 =0<<0;
	PF_CR2 =0<<0;
	PF_ODR =0<<0;

//ARPE/7 CMS[1:0]/65 DIR/4 OPM/3 URS/2 UDIS/1 CEN/0
	TIM2_CR1 &=~(1<<0);
//ARPE/7  OPM  URS  UDIS  CEN
	TIM4_CR1 &=~(1<<0);

//BEEPSEL[1:0]/76  BEEPEN/5  BEEPDIV[4:0]/4-0 公式128khz/(8*4)=4k
	BEEP_CSR2 &=~(1<<5);

//OVERIE/7 RES1.0/65 AWDIE/4 EOCIE/3 CONT/2 START/1 ADON/0
	ADC1_CR1 =0x00; 		//关AD电源
//TIEN/7 TCIEN/6 RIEN/5 ILIEN/4 TEN/3 REN/2 RWU/1 SBK/0
	USART1_CR2 =0x00;		//关串口

//在关时钟源时,先要关掉外设使能位
//DAC/7 BEEP/6 USART1/5 SPI1/4 I2C1/3 TIM4/2 TIM3/1 TIM2/0
	CLK_PCKENR1 =0x00;
//BootROM/7 COMP1&2/5 DMA1/4 LCD/3 RTC/2 TIM1/1 ADC1/0
	CLK_PCKENR2 =0x00;

//BEEPAHALT/6 FHWU/5 SAHALT/4 LSIRDY/3 LSION/2 HSIRDY/1 HSION/0
	CLK_ICKCR |=1<<4;	//关掉 MVR

	IWDG_KR =0xAA;		//清除看门狗,要在选项字节选择IWDG_HALT进入睡眠关闭看门狗
	_asm("halt\n");		//进入停机模式
	IWDG_KR =0xAA;

	Clk_Config();
	Gpio_Config();
	ADC1_Init();
	Uart1_Init();
	Tim_Init();
	f_cover_over =0;
	f_celar_hcho_vb =0;*/
}
////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////
void buzzer_program(void)			//2ms
{	
      static u8 buzzer_pwm;
      u8 temp_reg1;

      if(!f_on_off) //f_on_off=0；关闭蜂鸣器，=1，开启蜂鸣器
      {	
             buzzer_off;
             return;
      }
      if(f_buzzer_on)		//按键音
      {	
        if(++buzzer_pwm ==1)
            buzzer_on;
        if(buzzer_pwm >=50)
        {	buzzer_pwm =0;
                buzzer_off;
                f_buzzer_on =0;
        }
      }
     else
     {
                if(f_pm25_hco2)			//报警音
			temp_reg1 =dust_level;
		else
			temp_reg1 =hco2_level;
		if(temp_reg1 >=1)//超值报警
		{		  
                          if(f_mute)//超标
                          {
                                if(++buzzer_pwm2 ==1)
					buzzer_on ;
				else if(buzzer_pwm2 ==50)
					buzzer_off;

				if(buzzer_pwm2 ==100)
					buzzer_on;
				else if(buzzer_pwm2 ==150)
					buzzer_off;

				if(buzzer_pwm2 ==200)
					buzzer_on;
				else if(buzzer_pwm2 ==250)
					buzzer_off;

				if(buzzer_pwm2 >=2800)//2ms*2500=5s
					buzzer_pwm2 =0;
                          }   
                      else
                         buzzer_off;
                 }
                  else
                  buzzer_off;
     }
       
	
}
////////////////////////////////////////////////////////////
//按键扫描
//功能说明:
//编程说明:
////////////////////////////////////////////////////////////
void key_scannal(void)			//2ms
{	static  u8 Key_Read,Key_State,Key_Backup,Key_Long_Delay;//按键去抖时间计时器
	static  u8 click_degree,click_delay;
	static  u16 Key_Delay=50;
	u8 temp_reg1;

	temp_reg1 =0;
	if(!key_pin1)
		temp_reg1 =KEY_ON;
	if(!key_pin2)
		temp_reg1 =KEY_SET;
	if(!key_pin3)
		temp_reg1 =KEY_UP;
	if(!key_pin4)
		temp_reg1 =KEY_DOW;

 	if(Key_Read !=temp_reg1)	//若按键状态再次改变
	{ 	Key_Read =temp_reg1;	//保存新的按键状态
		Key_Delay =25;		//按键去抖时间初始化,有键值改变从新置初值
	}
	else if(--Key_Delay ==0)	//防抖
	{	
                Key_Delay =99; 			//连调延时50*2ms=100ms
		Key_Touch =Key_Read &(Key_Read ^Key_State);//得出单按键值-次,第二次清掉对应的键值，短按判断看Key_Touch
		Key_State =Key_Read;	//键值保存，长按判断按看Key_State

		if(Key_State ==0)
		{	if(++click_delay >=15)	//在有效的时间内连按了几次
			{	click_delay =0;
				if(Key_Backup ==KEY_UP)
				{	if(click_degree ==5)
					{	Key_Value =KEY_C_HCHO_VB;
						buzzer_pwm2 =0;
						//f_bbb_bz =1;
					}
				}
				click_degree =0;
				Key_Backup =0;
                                tune_mode=0;
			}
			Key_Long_Delay =0;		//没有按键值出口
			if(!f_on_off && !f_outsid_v)//有外置时不关机,因要显示电池充电状
                          Halt_OffDevice();
		}
		else
		{	click_delay =0;
			if(++Key_Long_Delay >=30)
				Key_Long_Delay =30;
			if(Key_Touch ==KEY_ON)//用于完成时间设置
                        {
                          tune_mode=0;
                        }
			if(Key_State ==KEY_ON &&Key_Long_Delay ==5)//200ms*15=3s
                        {Key_Value =VALUE_ON;}

			if(Key_Touch ==KEY_SET)
			{	if(tune_mode)
					Key_Value =VALUE_SET;
			}
			if(Key_State ==KEY_SET &&Key_Long_Delay ==5)//10*0.3s=3s
				Key_Value =VALUE_SET;

			if(Key_Touch ==KEY_UP)			//连按加
			{	Key_Backup =KEY_UP;
				if(!f_pm25_hco2)
				{	if(!tune_mode)				//连击清甲醛
					{	if(click_degree <5)
							++click_degree;
					}
				}
			}
			if(Key_State ==KEY_UP)			//连按加
			{	if(tune_mode)				//有模式选择时处于调整阶段
					Key_Value =VALUE_UP;
			}

			if(Key_State ==KEY_DOW)			//连按减
			{	if(tune_mode)				//有模式选择时处于调整阶段
					Key_Value =VALUE_DOW;
				else if(Key_Long_Delay ==5)//10*0.3s=3s
					Key_Value =VALUE_SOUND;
			}
		}
	}
}
////////////////////////////////////////////////////////////
//void KeyProcess(void)
//功能说明:
//编程说明:
////////////////////////////////////////////////////////////
void key_process(void)		//2ms
{
    u16 temp_reg1;//temp_reg2,temp_reg3;

	if(Key_Value ==0)//按键值为0，就不执行下面的程序，减少不必要的判断，提高效率
		return;

	if(Key_Touch)	//按下按键时，鸣叫
	f_buzzer_on =1;
	switch(Key_Value)
	{
		case VALUE_ON:
			f_buzzer_on =1;
 			if(f_on_off)  //关机
			{	
                          f_on_off =0;
			}
			else     //开机
			{	f_on_off =1;
                                GPIO_SetBits(GPIOB, GPIO_Pin_1);//开启总电
                                TM1640_Init();
                                tm1640_init(0x44,0x8a);         //固定地址模式，0x8c属亮度调节
                                for(temp_reg1=0; temp_reg1 <=16; temp_reg1++)//全显一次，全部清零，避免不必要的杂光
                                { 
                                  LCDRAM[temp_reg1]=0;
                                }
                                Autoadd();
                                LCDRAM[Dis_NUM_line2]=0x7f;//第二行线不显示红色
			}
			break;

		case VALUE_SOUND:
			f_buzzer_on =1;
                        if(f_on_off)//如果f_on_off=1，则启动如下功能
                        {
                            if(f_mute) f_mute=0;
                            else f_mute=1;
                        }
			break;

		case KEY_C_HCHO_VB:
			f_buzzer_on =1;
			if(!f_pm25_hco2)
				f_celar_hcho_vb =1;
			break;

		case VALUE_SET:
			f_buzzer_on =1;
			if(++tune_mode >3)
				tune_mode =0;	//退出时间调整
			break;

		case VALUE_UP:T2=0x00;
			switch(tune_mode)
			{	case 0:
					break;
				case 1:
					f_am_pm_set =1;
					break;
				case 2:
                                   RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);
                                    if(++RTC_TimeStr.RTC_Hours>=24)
                                    {
                                      RTC_TimeStr.RTC_Hours=0;
                                    }
                                    goto time_tune_common;

				case 3:
                                    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);
                                    if(++RTC_TimeStr.RTC_Minutes>=60)
                                    {
                                      RTC_TimeStr.RTC_Minutes=0;
                                    }
				goto time_tune_common;

				default :break;
			}
			break;

		case VALUE_DOW:T2=0x00;
			switch(tune_mode)
			{	case 0:	break;

				case 1:
					f_am_pm_set =0;
					break;
				case 2:
                                   if(RTC_WaitForSynchro() == SUCCESS)
                                   {
                                     RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);//得到的参数：小时、分钟、AM/PM
                                   }
                                   if(--RTC_TimeStr.RTC_Hours>=254)
                                    {
                                      RTC_TimeStr.RTC_Hours=23;
                                    }
                                    goto time_tune_common;

				case 3:
                                   if(RTC_WaitForSynchro() == SUCCESS)
                                   {
                                     RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);//得到的参数：小时、分钟、AM/PM
                                   }
                                  if(--RTC_TimeStr.RTC_Minutes>=254)
                                    {
                                      RTC_TimeStr.RTC_Minutes=59;
                                    }
		time_tune_common:
				RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
                                lcd_write();
                                
					break;

				default :
					break;
			}
			break;
	}
	Key_Value =0;		//清按键值
       
}
////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////
void key_scannal2(void)		//2ms
{	static u8 key_delay1,key_delay2,key_delay3;//key_delay4;

	if(extin_pin)			//外置电源插上检测,
	{	if(++key_delay1 >=20)//检测时间短一点，太长检测不到
		{	
                        key_delay1 =10;
			f_outsid_v =1;	//有外置电源
                        GPIO_SetBits(GPIOB, GPIO_Pin_1);//开启总电
		}
	}
	else if(--key_delay1 <=2)
	{	key_delay1 =120;
		f_outsid_v =0;
	}
	if(chang_pin)			//满电引脚检测
	{	if(++key_delay2 >=250)
		{	key_delay2 =120;
			f_full_v =1;
		}
	}
	else if(--key_delay2 <=2)
	{	key_delay2 =120;
		f_full_v =0;
	}

	if(select_pin)			//PM25/HCO2检测
	{	if(++key_delay3 >=250)
		{	key_delay3 =120;
			f_pm25_hco2 =1;		//PM25
                        H6_H7_H8=0;
                        H2_H3_H4=0x80;
		}
	}
	else if(--key_delay3 <=2)
	{	key_delay3 =120;
		f_pm25_hco2 =0;	//HCO2
                H6_H7_H8=0x80;
                H2_H3_H4=0;
	}
}






