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
u8 f_pm25_hco2=0;//PM25/��ȩ�жϣ�Ϊ1��PM25,Ϊ0����ȩ
//u8 f_bbb_bz=0;
extern u8 LCDRAM[];
//����ӵĶ���
//u8 delay_back=0;
//u8 f_flash=0;
u8 f_mute=1;//����������ѡ��λ
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
/*	sim()				//��ֹ�ж�,ʹ���ȼ���Ϊ3��(I1=1 I0=1),�����ſ���������ļĴ���
//00���½��غ͵͵�ƽ����
//01���������ش���
//10�����½��ش���
//11�������غ��½��ش���
//P3IS[1:0]/76  P2IS[1:0]/54  P1IS[1:0]/32  P0IS[1:0]/10
	EXTI_CR1 =2<<4;			//��Ӧÿ��I/O��0-3λ����������
//P7IS[1:0]/76  P6IS[1:0]/54  P5IS[1:0]/32  P4IS[1:0]/10
	EXTI_CR2 =1<<6;			//��Ӧÿ��I/O��4-7λ����������
//PFIS[1:0]/76  PEIS[1:0]/54  PDIS[1:0]/32  PBIS[1:0]/10
	EXTI_CR3 =0x00;			//P.fedb�½��ش���
	rim()

//���в�ʹ�õĹܽŶ�Ӧ������Ϊ����͵�ƽ�����Բ��������κβ��ùܽ�Ϊ��������״
//̬����ᵼ�²���Ҫ�ĸ߹���
	PA_DDR =0<<7|0<<6|1<<5|1<<4|1<<3|0<<2|1<<1|1<<0;//���/��������
	PA_CR1 =1<<7|1<<6|0<<5|0<<4|0<<3|1<<2|0<<1|0<<0;//�����״�ǿ�©,������״������
	PA_CR2 =1<<7|0<<6|0<<5|0<<4|0<<3|1<<2|0<<1|0<<0;//�����״������,������״�ǿ��������ж�
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

	PF_DDR =1<<0;  	//ֻ��PF0
	PF_CR1 =0<<0;
	PF_CR2 =0<<0;
	PF_ODR =0<<0;

//ARPE/7 CMS[1:0]/65 DIR/4 OPM/3 URS/2 UDIS/1 CEN/0
	TIM2_CR1 &=~(1<<0);
//ARPE/7  OPM  URS  UDIS  CEN
	TIM4_CR1 &=~(1<<0);

//BEEPSEL[1:0]/76  BEEPEN/5  BEEPDIV[4:0]/4-0 ��ʽ128khz/(8*4)=4k
	BEEP_CSR2 &=~(1<<5);

//OVERIE/7 RES1.0/65 AWDIE/4 EOCIE/3 CONT/2 START/1 ADON/0
	ADC1_CR1 =0x00; 		//��AD��Դ
//TIEN/7 TCIEN/6 RIEN/5 ILIEN/4 TEN/3 REN/2 RWU/1 SBK/0
	USART1_CR2 =0x00;		//�ش���

//�ڹ�ʱ��Դʱ,��Ҫ�ص�����ʹ��λ
//DAC/7 BEEP/6 USART1/5 SPI1/4 I2C1/3 TIM4/2 TIM3/1 TIM2/0
	CLK_PCKENR1 =0x00;
//BootROM/7 COMP1&2/5 DMA1/4 LCD/3 RTC/2 TIM1/1 ADC1/0
	CLK_PCKENR2 =0x00;

//BEEPAHALT/6 FHWU/5 SAHALT/4 LSIRDY/3 LSION/2 HSIRDY/1 HSION/0
	CLK_ICKCR |=1<<4;	//�ص� MVR

	IWDG_KR =0xAA;		//������Ź�,Ҫ��ѡ���ֽ�ѡ��IWDG_HALT����˯�߹رտ��Ź�
	_asm("halt\n");		//����ͣ��ģʽ
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

      if(!f_on_off) //f_on_off=0���رշ�������=1������������
      {	
             buzzer_off;
             return;
      }
      if(f_buzzer_on)		//������
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
                if(f_pm25_hco2)			//������
			temp_reg1 =dust_level;
		else
			temp_reg1 =hco2_level;
		if(temp_reg1 >=1)//��ֵ����
		{		  
                          if(f_mute)//����
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
//����ɨ��
//����˵��:
//���˵��:
////////////////////////////////////////////////////////////
void key_scannal(void)			//2ms
{	static  u8 Key_Read,Key_State,Key_Backup,Key_Long_Delay;//����ȥ��ʱ���ʱ��
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

 	if(Key_Read !=temp_reg1)	//������״̬�ٴθı�
	{ 	Key_Read =temp_reg1;	//�����µİ���״̬
		Key_Delay =25;		//����ȥ��ʱ���ʼ��,�м�ֵ�ı�����ó�ֵ
	}
	else if(--Key_Delay ==0)	//����
	{	
                Key_Delay =99; 			//������ʱ50*2ms=100ms
		Key_Touch =Key_Read &(Key_Read ^Key_State);//�ó�������ֵ-��,�ڶ��������Ӧ�ļ�ֵ���̰��жϿ�Key_Touch
		Key_State =Key_Read;	//��ֵ���棬�����жϰ���Key_State

		if(Key_State ==0)
		{	if(++click_delay >=15)	//����Ч��ʱ���������˼���
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
			Key_Long_Delay =0;		//û�а���ֵ����
			if(!f_on_off && !f_outsid_v)//������ʱ���ػ�,��Ҫ��ʾ��س��״
                          Halt_OffDevice();
		}
		else
		{	click_delay =0;
			if(++Key_Long_Delay >=30)
				Key_Long_Delay =30;
			if(Key_Touch ==KEY_ON)//�������ʱ������
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

			if(Key_Touch ==KEY_UP)			//������
			{	Key_Backup =KEY_UP;
				if(!f_pm25_hco2)
				{	if(!tune_mode)				//�������ȩ
					{	if(click_degree <5)
							++click_degree;
					}
				}
			}
			if(Key_State ==KEY_UP)			//������
			{	if(tune_mode)				//��ģʽѡ��ʱ���ڵ����׶�
					Key_Value =VALUE_UP;
			}

			if(Key_State ==KEY_DOW)			//������
			{	if(tune_mode)				//��ģʽѡ��ʱ���ڵ����׶�
					Key_Value =VALUE_DOW;
				else if(Key_Long_Delay ==5)//10*0.3s=3s
					Key_Value =VALUE_SOUND;
			}
		}
	}
}
////////////////////////////////////////////////////////////
//void KeyProcess(void)
//����˵��:
//���˵��:
////////////////////////////////////////////////////////////
void key_process(void)		//2ms
{
    u16 temp_reg1;//temp_reg2,temp_reg3;

	if(Key_Value ==0)//����ֵΪ0���Ͳ�ִ������ĳ��򣬼��ٲ���Ҫ���жϣ����Ч��
		return;

	if(Key_Touch)	//���°���ʱ������
	f_buzzer_on =1;
	switch(Key_Value)
	{
		case VALUE_ON:
			f_buzzer_on =1;
 			if(f_on_off)  //�ػ�
			{	
                          f_on_off =0;
			}
			else     //����
			{	f_on_off =1;
                                GPIO_SetBits(GPIOB, GPIO_Pin_1);//�����ܵ�
                                TM1640_Init();
                                tm1640_init(0x44,0x8a);         //�̶���ַģʽ��0x8c�����ȵ���
                                for(temp_reg1=0; temp_reg1 <=16; temp_reg1++)//ȫ��һ�Σ�ȫ�����㣬���ⲻ��Ҫ���ӹ�
                                { 
                                  LCDRAM[temp_reg1]=0;
                                }
                                Autoadd();
                                LCDRAM[Dis_NUM_line2]=0x7f;//�ڶ����߲���ʾ��ɫ
			}
			break;

		case VALUE_SOUND:
			f_buzzer_on =1;
                        if(f_on_off)//���f_on_off=1�����������¹���
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
				tune_mode =0;	//�˳�ʱ�����
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
                                     RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);//�õ��Ĳ�����Сʱ�����ӡ�AM/PM
                                   }
                                   if(--RTC_TimeStr.RTC_Hours>=254)
                                    {
                                      RTC_TimeStr.RTC_Hours=23;
                                    }
                                    goto time_tune_common;

				case 3:
                                   if(RTC_WaitForSynchro() == SUCCESS)
                                   {
                                     RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);//�õ��Ĳ�����Сʱ�����ӡ�AM/PM
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
	Key_Value =0;		//�尴��ֵ
       
}
////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////
void key_scannal2(void)		//2ms
{	static u8 key_delay1,key_delay2,key_delay3;//key_delay4;

	if(extin_pin)			//���õ�Դ���ϼ��,
	{	if(++key_delay1 >=20)//���ʱ���һ�㣬̫����ⲻ��
		{	
                        key_delay1 =10;
			f_outsid_v =1;	//�����õ�Դ
                        GPIO_SetBits(GPIOB, GPIO_Pin_1);//�����ܵ�
		}
	}
	else if(--key_delay1 <=2)
	{	key_delay1 =120;
		f_outsid_v =0;
	}
	if(chang_pin)			//�������ż��
	{	if(++key_delay2 >=250)
		{	key_delay2 =120;
			f_full_v =1;
		}
	}
	else if(--key_delay2 <=2)
	{	key_delay2 =120;
		f_full_v =0;
	}

	if(select_pin)			//PM25/HCO2���
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






