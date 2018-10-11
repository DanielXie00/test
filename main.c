/* /* /* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_uart_over_ble_main main.c
 * @{
 * @ingroup  ble_sdk_app_nus_eval
 * @brief    UART over BLE application main file.
 *
 * This file contains thes source code for a sample application that uses the Nordic UART service.
 * This application uses the @ref srvlib_conn_params module.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf51_bitfields.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_nus.h"
#include "simple_uart.h"
#include "boards.h"
#include "ble_error_log.h"
#include "ble_debug_assert_handler.h"

#include "Typedef.h"
//#include "cw2015_battery.h"
#include "nrf_delay.h"
#include "led.h"
#include "app.h"
#include "adc.h"
#include "timerx.h"
#include "pwm.h"
#include "music.h"
#include "DATAFLASH.h"
#include "ble_bondmngr.h"
#include "pstorage.h"
#include "app_button.h"
#include "main.h"
#include "temp.h"
#include "scom.h"


#include "cw2015_battery.h"
#include "ble_nus.h"
#include "smoke.h"
#include "KeyScanM.h"
#include "bd_wdt.h"

extern void InitFuel(void);

void cw_bat_init(void);
void lcdIO_init(void);
void cw_bat_work(void);
void iic_init(void);


extern uint8_t cTimer0Flag;
extern uint16_t pwm_value;
extern volatile uint16_t pwm_cnt;

uint32_t delay_start_timer2_cnt = 15;

UINT8 SecondCnt = 0;


extern  STRUCT_CW_BATTERY   cw_bat;

extern BYTE xdata KeyChange;  
extern unsigned char cTimer0Flag;
extern bit SPIRcvByteStatus;	  
extern bit RcvOk;
extern BIT	firstSetTemp;
extern BIT firstSetTime;
extern BIT StartSetTime;
extern BIT StartSetTemp;
extern BIT	Flag1s;

extern BIT StartAddPower;

extern BYTE SKeyValue;
extern UINT8 TempLevelNumber;
extern volatile BYTE WhiteLedControl[4];
extern BYTE OneSecondCnt;
extern BIT reset_mcu;
extern BIT fuel_time_tick;
extern bit CHARGE; //是否接充电器标志位，1为接充电器，0为拔出充电器
extern UINT8 upadate_temp_time_cnt;
extern UINT8 set_update_temp_cnt;
extern UINT8 upadate_temp_delay;

UINT32 temperature1AdcResult;
UINT32 temperature2AdcResult;

UINT16 TempPwm;
UINT8 AdcCnt;

UINT8 TempLevel = TEMP_LEVEL_ONE;
UINT8 AppTempLevel = TEMP_LEVEL_EIGHT;

UINT8 xdata ChargeOkCnt = 0;
BIT StartAddPower = 0;
BIT TempIsOk = 0;
BIT Enable_White_Led_Toggle;
BIT ChargeCplFlag;
BIT	HotCplFlag;
BIT	T1_to_T2_flag;
BIT charge_ok_flag;
UINT8 my_charge_ok_for_led;
BIT highest_temp_need_restart;
BIT charge_open_uint_flag;

UINT8 bat_low_flag = 0;

UINT8 T1_delay_cnt = 0;
volatile UINT8 IsCharge = IS_UP_POWER;
UINT8 ChargePowerCutCnt;


UINT8 DisplayBatTimeCnt = 0;
UINT16 TempIsOkCnt;

UINT16 E_k_1 = 20;
UINT16 E_k = 21;

UINT16 xdata StartWorkCnt = 0;
UINT16 SetTimeOverCnt = 0;
UINT16 SetTempOverCnt = 0;

UINT16 DelayUseCnt;

UINT8 CurrentSenorTemp1 = 0;
UINT8 CurrentSenorTemp2 = 0;

volatile UINT8 HotTimeNumber;

UINT16 SystemIdleTimeCnt = 0;

UINT16 hout_deley_time_cnt = 0;
UINT8 display_bat_status;


UINT8 index = 6;

extern UINT8 start_charge_flag;
extern WORD xdata charge_quit_enable_hot_cnt;
extern BIT new_hot_work_time_flag;
extern UINT8 code Table[];
extern uint8_t *p_write_buffer;
extern uint8_t *p_read_buffer;
extern uint32_t write_buffer[2]; 
extern uint32_t read_buffer[2]; 
extern UINT16 hot_ok_ledg_on_time;

BIT hot_restart_flag;


UINT8 code TIME_TABLE[]={47,47,47,47,47,47,47,42,37,37};//单位秒
UINT8 code HOT_OK_LEDG_ON_TIME[]={60,60,120,180,240,240};//单位秒

//uint8_t  m_boot_settings[CODE_PAGE_SIZE] __attribute__((at(BOOTLOADER_SETTINGS_ADDRESS))) = {BANK_VALID_APP};
uint8_t const FWVERSION __attribute__((at(FWVERSION_ADDR))) = (uint8_t)FWVERSION_VALUE;      /**< This variable ensures that the linker script will write the bootloader start address to the UICR register. This value will be written in the HEX file and thus written to UICR when the bootloader is flashed into the chip. */

uint8_t fw_version;

UINT8 cap_move_time = 0;

UINT16 hot_work_time = 0;

UINT16 hot_ok_ledg_on_time = 0;
UINT16 temp_hot_ok_time = 0;


BIT new_hot_work_time_flag = 0;
//BIT first_hot_flag = 0;
UINT8 set_inc_dec_temp_cnt;
UINT8 highest_temp_need_restart_cnt;



uint16_t check_hot_over_time_cnt = 0;
UINT8 bluetooth_status = DISCONNETED;

uint8_t rcv_buffer[50] = {0};

UINT8 first_start;
UINT8 display_temp_cnt = 0;
uint8_t inc_temp = 0;

bool bootloader_ok;
uint8_t custom_flag = 0;
static uint8_t bt_connect_cnt = 0;
//static uint8_t protect_temp = 0;
uint8_t lock = UNLOCK;
uint8_t custom_index = WORK_ON_NORMAL;


uint8_t motor_enable = ON;

extern UINT8 BTTempLevelNumber;

extern UINT16 set_time_ok_quit_cnt;
extern ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
extern uint16_t smoke_cnt;
extern uint8_t write_eeprom_flag;
extern uint8_t  TableValue[];
extern UINT8 const TableValueConst[];

void POWER_OFF(void)				
{
  

	if (write_eeprom_flag)
	{
		write_eeprom_flag = 0;		
		write_eeprom();	
	}

	HW_POWER_OFF();
}

void NO_SAVE_POWER_OFF(void)				
{
	HW_POWER_OFF();
}

void DelayMS(unsigned int ch)
{
	nrf_delay_ms(ch);		
}
 
//prosess of ADC
unsigned int Adc_Sampling(unsigned char ADCChannel)
{
	
	unsigned int iAdcResult;

	iAdcResult = get_current_temp(ADCChannel);
	
	return  iAdcResult;
}

float calculateResistance(UINT32 voltage) 
{
	return ((float)RESISRTOR0 * (float)voltage)/((float)0x3ff - (float)voltage);
}

UINT16 calculateTemp(UINT32 resistance) 
{
	UINT8 i; 
	if (resistance > TempTable[0])
	{
		return 0;
	}

	for (i = 0; i< 250;i++)
	{
		if (resistance == TempTable[i]) 
		{
		 	return i;
		}	

		if (resistance == TempTable[i+1]) 
		{
			return i + 1;
		}
		
		if (resistance < TempTable[i] && resistance > TempTable[i+1])
		{
		 	return i;
		}	
		
	}	   

//	if (resistance > TempTable[249])
//	{
//		return 0xff;
//	}

	return 0xff;	 
} 
 
UINT8 controlTemp(UINT8 targetTemp,UINT8 currentTemp) 
{
 	INT16 U_k;	

//	Send_Data_To_iPhone(0XA1);
//	Send_Data_To_iPhone(targetTemp);
//	Send_Data_To_iPhone(currentTemp);

	E_k = targetTemp - currentTemp;
	
//	protect_temp = targetTemp;
	
	if (E_k < 250)//限制幅度
	//if (E_k < 240)//限制幅度
	{ 
		U_k = Kp * E_k + Kd * (E_k - E_k_1);
	
		E_k_1 = E_k;
		  
		if (U_k <= 0)
		{		   
			U_k = 0;  
			TempPwm = U_k;
		}
		else if (U_k >= MAXPWM)
		{
			TempPwm = MAXPWM;
		}
		else
		{	 		
		 	TempPwm = U_k;
		} 
	}
	else
	{
		TempPwm = 0; 
	}

	

		
	if ((3*currentTemp) < (2*targetTemp))
	{			
		Set_Period_Pwm(MAX_ALL_SPEED_PWM);					
	}	
	else if ((32*currentTemp) < ((26*targetTemp)))
	{					
		Set_Period_Pwm(ALL_SPEED_PWM);	
		

//		Send_Data_To_iPhone(0XB1);
//		Send_Data_To_iPhone(currentTemp);
	}   	
	else
	{  

		if (currentTemp >= targetTemp)
		{
			TempPwm = 0;
		}
		
		Set_Period_Pwm(TempPwm);
		
		
//		Send_Data_To_iPhone(0XB3);
//		Send_Data_To_iPhone(targetTemp);

//		Send_Data_To_iPhone(currentTemp);	
//		Send_Data_To_iPhone(TempPwm>>8);	
//		Send_Data_To_iPhone(TempPwm);	
			 
		if ((currentTemp > (targetTemp - TEMP_TH)) && (currentTemp != 0xFF))
		{
			//温度到达后延迟一段时间后显示OK  
			DelayUseCnt++;
			if (DelayUseCnt >= DELAY_USE_OVER_TIME)
			{
				DelayUseCnt = 0;

 

				//Send_Data_To_iPhone(0XD1);
			 
				IOCTRL(MOTOR,MOTOR_OFF);
				LoadLedTempLevelToBuffer(); 		
			}  				
		}  
	}	

 	return 0xff;    
}

void ProcessTemp(void)
{
//	UINT8 i;
	
	float TempAdc;	
	
	UINT16 temp_result;
	   

	//霍尔器件闭合时才能加热
	//电池电压正常时加热	  
	if ((SKeyValue & HOUT ) && (display_bat_status >= ENABLE_HOT_LEVEL))
	//if ((display_bat_status >= ENABLE_HOT_LEVEL))	
	{	  	

		AdcCnt++;
	
		if(AdcCnt <= 16)
		{
			temp_result = Adc_Sampling(ADC_TEMP_CHANNEL1);
			temperature1AdcResult += temp_result;
			//Send_Data_To_iPhone(0x02);			
			//Send_Data_To_iPhone((temp_result)>>8);
			//Send_Data_To_iPhone(temp_result & 0XFF);
		
			
			temp_result = Adc_Sampling(ADC_TEMP_CHANNEL2);
			temperature2AdcResult += temp_result;				
			//Send_Data_To_iPhone(0x02);			
			//Send_Data_To_iPhone((temp_result)>>8);
			//Send_Data_To_iPhone(temp_result & 0XFF);
		
		}
		
		if (AdcCnt > 16)
		{
			AdcCnt = 0;

			//温度传感器1
			temperature1AdcResult >>= 4;
			temperature2AdcResult >>= 4;

			//Send_Data_To_iPhone(0x02);			
			//Send_Data_To_iPhone((temperature2AdcResult )>>8);
			//Send_Data_To_iPhone(temperature2AdcResult & 0XFF);
		
			//短路和开路
			if ((temperature1AdcResult == 0) || (temperature1AdcResult == 0x3ff))
			{
				temperature1AdcResult = 0;
				CurrentSenorTemp1 = 0;
			}
			if ((temperature2AdcResult == 0) || (temperature2AdcResult == 0x3ff))
			{
				temperature2AdcResult = 0;
				CurrentSenorTemp2 = 0;
			}
			
			//过热
			if ((temperature1AdcResult <= TEMP_HIGH))
			{
				temperature1AdcResult = 0;
				CurrentSenorTemp1 = 0;
			}			

			if ((temperature2AdcResult <= TEMP_HIGH))
			{
				temperature2AdcResult = 0;
				CurrentSenorTemp2 = 0;
			}				
				
			if ((temperature1AdcResult > 0) && (temperature1AdcResult < 0x3ff))
			{
				TempAdc = calculateResistance(temperature1AdcResult);//0-1022000		

			
				temperature1AdcResult = (UINT32)TempAdc;
				temperature1AdcResult = calculateTemp(temperature1AdcResult);

//				Send_Data_To_iPhone(0x03);
//				Send_Data_To_iPhone(0x03);
//				Send_Data_To_iPhone(temperature1AdcResult);
				
				if ((temperature1AdcResult != 0xff))
				{						 
					CurrentSenorTemp1 = (UINT8)temperature1AdcResult;		
//					if(TempLevel > PROTECT_TEMP_MAX)
//					{
//						controlTemp(PROTECT_TEMP_MAX,CurrentSenorTemp1);//时间长						
//					}
//					else
					{
						controlTemp(TempLevel,CurrentSenorTemp1);//时间长						
					}
				}
				
				//查找不到温度
				if ((temperature1AdcResult == 0xff))
				{
					CurrentSenorTemp1 = 0;
				}
			
			}
			
			
			
			//侧边SENSOR
			if ((temperature2AdcResult > 0) && (temperature2AdcResult < 0x3ff))
			{
				TempAdc = calculateResistance(temperature2AdcResult);//0-1022000		

			
				temperature2AdcResult = (UINT32)TempAdc;
				temperature2AdcResult = calculateTemp(temperature2AdcResult);
				
				if ((temperature2AdcResult != 0xff))
				{						 
					CurrentSenorTemp2 = (UINT8)temperature2AdcResult;										
				}	

				//查找不到温度
				if ((temperature2AdcResult == 0xff))
				{
					CurrentSenorTemp2 = 0;
				}				
			}			
			
//			Send_Data_To_iPhone(0x01);			
//			Send_Data_To_iPhone(CurrentSenorTemp1);
//			Send_Data_To_iPhone(CurrentSenorTemp2);
			
			if ((CurrentSenorTemp1 != 0) && (CurrentSenorTemp2 != 0))
			{			
				if (
					((INT16)CurrentSenorTemp1 - (INT16)CurrentSenorTemp2 > ((INT16)DELTA_TH)) || \
					((INT16)CurrentSenorTemp2 - (INT16)CurrentSenorTemp1 > ((INT16)DELTA_TH))
				   )			
				{
//					Send_Data_To_iPhone(CurrentSenorTemp1 - CurrentSenorTemp2);
					Send_Data_To_iPhone(0XEE);
//					Send_Data_To_iPhone(0XEE);

//					
//					IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);
//					for (i = 0; i < 6; i++)
//					{
//						error_led_on_off();
//					}  	
//				
//					if ((SKeyValue & CHARGE_IN_VALUE))
//					{			  			 
//						Set_Period_Pwm(0); 
//						control_motor(POWER_OFF_MOTOR_TIMES);		  			 
//						POWER_OFF(); 
//					}	
//					else
//					{
//						
//						reset_work_value();
//					}
				
				}
				
			}
			
			temperature1AdcResult = 0;
			temperature2AdcResult = 0;
		}
		
	}
	else
	{
		//充电时过放
		if (!(display_bat_status >= ENABLE_HOT_LEVEL))
		{
			if (StartAddPower)
			{
 
				IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);				
 
				
				WHITE_RED_LED_ALL_OFF(); 			
				
				

				Send_Data_To_iPhone(0x88);
				Send_Data_To_iPhone(0x88);
	
				flash_error_led(ERROR_LED_TIMES);
	
				if ((SKeyValue & CHARGE_IN_VALUE))
				{			  			 
					Set_Period_Pwm(0); 
					control_motor(POWER_OFF_MOTOR_TIMES);		  			 
					POWER_OFF(); 
				}	
				else
				{
					
					reset_work_value();
				}
			}

		}
		

		AdcCnt = 0;
	
		temperature1AdcResult = 0;
		temperature2AdcResult = 0;
		
		Set_Period_Pwm(0); 		 
	}



}

//-----------------------------------------------------------------------------------------------------------
void main2(void)
{
//	uint16_t temp_result;
//	uint8_t data_array[2]={"WW"};

	UINT8 i;
	UINT8 idx; 
	
//	lock = UNLOCK;//TEST
 
 
	
	bootloader_ok = check_bootloader();
	
	fw_version = FWVERSION;
	

	if ((TempLevelNumber == 0) || (TempLevelNumber > TEMP_INDEX_8))
	{		   	
		TempLevelNumber = TEMP_INDEX_8;	
				 
	}		
	
	LoadLedTempLevelToBuffer();	    	
				   
 	IOCTRL(MOTOR,MOTOR_OFF);
//	DelayMS(LED_FLASH_OVER_TIME);  
	PWM_Init();
  
		 
	Set_Period_Pwm(0); 	   
 
  	
 
	if (bat_low_flag == 0xff)
	{
		bat_low_flag = 0;
	}		


	if (my_charge_ok_for_led == 0xff)
	{
		my_charge_ok_for_led = 0;	

	}

	start_charge_flag = 0;
	charge_open_uint_flag = 0;
	charge_quit_enable_hot_cnt = 0;
	display_temp_cnt = 0;
 
	while(1)
	{ 
		if (cTimer0Flag)
		{ 
			cTimer0Flag = 0;

			StartWorkCnt++;
			if (StartWorkCnt >= START_WORK_OVER_TIME)
			{
				StartWorkCnt = 0;	

				while(1)
				{	 	 
					//if (IO_KEY_TEMP_INC && IO_KEY_TEMP_DEC)
					if (IO_KEY_POWER)
					{
						break;
					}

					if (IO_KEY_CHARGE_IN)//未上盖充电,开机
					{
						StartWorkCnt = 0;
						DelayMS(KEY_PUSH_OVER_TIME);			
						if (IO_KEY_CHARGE_IN)//未上盖充电,开机
						{
							POWER_ON();
							start_charge_flag = 1;
							charge_open_uint_flag = 1;
							goto MYSTART;
						}
					}
							  						
				}
				//Send_Data_To_iPhone(0XD1);
				NO_SAVE_POWER_OFF(); 	   
			}


			i = CheckPowerOnOff();

			if (i)
			{
				StartWorkCnt = 0; 
				break;
			}		
			 
			if (!IO_KEY_HOUT)//盖上盖子
			{		
				hout_deley_time_cnt = 0;
			}
			else
			{
				hout_deley_time_cnt++;
				if (hout_deley_time_cnt > 5)
				{
					hout_deley_time_cnt = 0;
										 				
					while(index)
					{	 
						index--;  				
						//error_led_on_off();	  						
						no_cup_led_on_off();	  						
					}

					while(1)
					{	 	 
						//if (IO_KEY_TEMP_INC && IO_KEY_TEMP_DEC)
						if (IO_KEY_POWER)
						{
							break;
						}  
												
						if (IO_KEY_CHARGE_IN)//未上盖充电,开机
						{
							StartWorkCnt = 0;
							DelayMS(KEY_PUSH_OVER_TIME);			
							if (IO_KEY_CHARGE_IN)//未上盖充电,开机
							{
								POWER_ON();
								start_charge_flag = 1;
								charge_open_uint_flag = 1;
								goto MYSTART;
							}
						}
					}
					//Send_Data_To_iPhone(0XD2);

					NO_SAVE_POWER_OFF(); 
				}   
			}	
		} 

		if (IO_KEY_CHARGE_IN)//未上盖充电,开机
		{
			StartWorkCnt = 0;

			DelayMS(KEY_PUSH_OVER_TIME);			
			if (IO_KEY_CHARGE_IN)//未上盖充电,开机
			{
				POWER_ON(); 
				start_charge_flag = 1;
				charge_open_uint_flag = 1;
				break;
			}
		}
	}		 

MYSTART:
	POWER_ON(); 


		
	DelayMS(LED_FLASH_OVER_TIME);	  
	Read_Byte_Data_Flash();
	
	IOCTRL(MOTOR,MOTOR_ON); 	
	
	IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);
	
	DelayMS(LED_FLASH_OVER_TIME);	 	
	IOCTRL(MOTOR,MOTOR_OFF); 

	WHITELEDCON(0xFF); 		
	REDLEDCON(0XFF); 
	 	
		 
	SKeyValue = 0;	   
	i = 6;
	SystemIdleTimeCnt = 0;	
	hout_deley_time_cnt = 0;
 	charge_ok_flag = 0;
//	my_charge_ok_for_led = 0;
	temperature1AdcResult = 0;
	hot_restart_flag = 0;
	cap_move_time = 0;
	hot_work_time = 0;	  
	new_hot_work_time_flag = 0;
	

	set_inc_dec_temp_cnt = 0;
	highest_temp_need_restart_cnt = 0;
	highest_temp_need_restart = 0;

	check_hot_over_time_cnt = 0;
	upadate_temp_time_cnt = 0;

	set_update_temp_cnt = 0;
	upadate_temp_delay = 0;
	IsCharge = IS_UP_POWER;	
	
	//开机加热
	first_start = 0;

	AdcCnt = 0;	
	temperature1AdcResult = 0;
	temperature2AdcResult = 0;
	
//	E_k_1 = 20;
//	E_k = 21;

	//初始化,启动问题




	cw_bat_init(); 

//	//InitFuel();


	cw_bat_work(); 
	
//	nrf_delay_ms(1000);
//	cw_bat_work();

//	display_bat_status = *(UINT8 code *)(BAT_VOL_ADDR);
 	if (display_bat_status == 0xff)
	{		   	
		display_bat_status = shake_voltage();
		  	
		if (bat_low_flag)
		{
			display_bat_status = SUPER_VERY_BAT_LOW; 
		}

		//Write_Byte_Data_Flash(BAT_VOL_ADDR,display_bat_status);			
	}
	else
	{
 
	}



	//未充电开机,电量低时关机
	if (!IO_KEY_CHARGE_IN && (cw_bat.capacity < CHARGE_FIRST_GRID_TH))
	{											
		display_bat_status = SUPER_VERY_BAT_LOW;	

		advertising_stop(); 		
 			
		IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);				
 
				
		WHITE_RED_LED_ALL_OFF(); 			
		
		flash_error_led(ERROR_LED_TIMES);  
		
		Set_Period_Pwm(0);
		control_motor(POWER_OFF_MOTOR_TIMES);

		while(1)
		{
			IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);	
			//set_WDCLR;
			WHITE_RED_LED_ALL_OFF();

			nrf_delay_ms(10);
			//if (IO_KEY_TEMP_INC && IO_KEY_TEMP_DEC)
			if (IO_KEY_POWER)
			{
			 	break;
			}
			
 
		}

		Set_Period_Pwm(0); 
		
		POWER_OFF(); 
	}

 	
	check_hot_over_time_cnt = 0;
 

	if (StartAddPower || (cw_bat.capacity < BAT_FULL_CAP_MIN))
	{

		my_charge_ok_for_led = 0; 
		
	}
 
//	read_eeprom();

	wdt_init();

	if (bootloader_ok)
	{
		DisplayBatTimeCnt = DEFAULT_DISP_BAT_TIME; 
		display_now_bat();	
	}

	
 	wdt_start();
	
	while(1)
	{
 
			    
 		//设置温度和未充电时才能加热
// 		if (StartAddPower)// && (SKeyValue & CHARGE_IN_VALUE))
//		{	
//			ProcessTemp(); 	
//		}  
				  
		
		
		//加热后盖子掉下关闭	
		if (IO_KEY_HOUT)
		{ 

			hout_deley_time_cnt++;

			if (hout_deley_time_cnt > 10000)
			{
				hout_deley_time_cnt = 0;  				

				if (IO_KEY_HOUT)
				{
 	
					
					if (StartAddPower || TempIsOk)
					{
 
						
						IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);						
					} 		 
						
					//StartAddPower = 0; 		
		

					if (StartAddPower)
					{
						hot_restart_flag = 1;
						if (!StartSetTime)
						{
							Enable_White_Led_Toggle = 1; 					
						}
					}

					Set_Period_Pwm(0); 	
 	   				
 
				}
				else
				{
					i = 6;
					hout_deley_time_cnt = 0;
					hot_restart_flag = 0;
					cap_move_time = 0;
				}
			} 				
		}
		else
		{
 
			
			i = 6;
			hout_deley_time_cnt = 0;

			if ((upadate_temp_time_cnt == 0) && (upadate_temp_delay == 0))
			{
				if (TempIsOk)
				{
 
					IOCTRL_LED(OFF,ON,OFF,LED_MODE_NORMAL);
				}
			}
				
			if (hot_restart_flag)
			{

				hot_restart_flag = 0;

				Enable_White_Led_Toggle = 0; 
 

				
						
				//盖子超时退出
				if (cap_move_time >= CAP_MOVE_OVER_TIME)
				{						
					reset_work_value();											
				}
			
			}
		}	   		 

		//if (!StartAddPower)
		{
			wdt_feed();
		}
			
		if (cTimer0Flag)
		{ 
			cTimer0Flag = 0;
			
			//if (!StartAddPower)
			{
				wdt_feed();
			}
			
			if (custom_flag)
			{
				custom_flag--;
				
				if (custom_flag == 0)
				{					 		
					bluetooth_send_custom();
				}
			}				
			
			if (StartAddPower)// && (SKeyValue & CHARGE_IN_VALUE))
			{	
				ProcessTemp(); 		
			} 

			bt_connect_indicate();
			
			KeyEventProcess(); 

			reset_app_boost();
			
			if (bootloader_ok)
			{
				SetHot(); 	  
				SetTemp();				 	
			}
			
			LowBatOverTimeClosePower();	  

			CheckSensorError();	 				
			//set_WDCLR;
			

			if (fuel_time_tick)
			{
				fuel_time_tick = 0;


//				Send_Data_To_iPhone(TempLevel);
//				Send_Data_To_iPhone(CurrentSenorTemp1);
//				Send_Data_To_iPhone(CurrentSenorTemp2);
//				Send_Data_To_iPhone(set_update_temp_cnt);
				
//				restore_temp();
				
				restore_display_temp();

				if (IO_KEY_CHARGE_IN)
				{
					CHARGE = 1;
				}
				else
				{
					CHARGE = 0;
				}

				//延时保存数据
				if (write_eeprom_flag)
				{
					write_eeprom_flag--;
					if (write_eeprom_flag == 0)
					{
						write_eeprom();
					}
					
				}
				cw_bat_work();

//				simple_uart_put(0XAA);
//				simple_uart_put(get_bat_percent());
//				simple_uart_put(0XBB);
//				Send_Data_To_iPhone(display_bat_status);
//				Send_Data_To_iPhone(cw_bat.capacity & 0XFF);
//				temp_result = Adc_Sampling(ADC_CONFIG_PSEL_AnalogInput5);
//				simple_uart_put(temp_result>>8);				
//				simple_uart_put(temp_result & 0xff);
//				
//				temp_result = Adc_Sampling(ADC_CONFIG_PSEL_AnalogInput6);
//				simple_uart_put(temp_result>>8);				
//				simple_uart_put(temp_result & 0xff);				
				


//				Send_Data_To_iPhone(CurrentSenorTemp1);
//				Send_Data_To_iPhone(SKeyValue);
//				Send_Data_To_iPhone(StartAddPower);
				
				bluetooth_send_process(NORMAL_SEND);
				
 
				
				if (IO_KEY_CHARGE_IN)
				{ 					

					if (cw_bat.capacity < CHARGE_FIRST_GRID_TH)
					{
						display_bat_status = SUPER_VERY_BAT_LOW;											
					}
					else
					{
						display_bat_status = shake_voltage();					
					}
				}
				else
				{
					display_bat_status = shake_voltage();

					if (bat_low_flag)
					{
						display_bat_status = SUPER_VERY_BAT_LOW; 
					}				
				}
			}

			SecondCnt++;	
			if (SecondCnt >= 45) // 0.5s
			{
				SecondCnt = 0;



				restore_temp();
//				Send_Data_To_iPhone(0X33);
//				Send_Data_To_iPhone(display_bat_status);
//				Send_Data_To_iPhone(cw_bat.capacity>>8);
//				Send_Data_To_iPhone(cw_bat.capacity & 0XFF);

//				Send_Data_To_iPhone(TempLevel);		
				
//				Send_Data_To_iPhone(CurrentSenorTemp1);
//				Send_Data_To_iPhone(SKeyValue);
//				
//				Send_Data_To_iPhone(IsCharge);

//				Send_Data_To_iPhone(TempLevelNumber);	
//				Send_Data_To_iPhone(TempLevel);	
				
				CheckSystemIdle();	  

				//未充电完成
				if ((!(SKeyValue & CHARGE_IN_VALUE)) && (((SKeyValue & CHARGE_OUT_VALUE))))
 				{
 

					IsCharge = IS_CHARGING;
					ChargePowerCutCnt = 0;	
					Send_Data_To_iPhone(0X31);

					if (!StartAddPower && !StartSetTime && !StartSetTemp && (DisplayBatTimeCnt == 0) && (display_temp_cnt == 0))//加热,设置时,不显示电量
					{	   						
						ChargeCplFlag = ~ChargeCplFlag;
						//电池电量进度指示		  
						//Send_Data_To_iPhone(0X01);

						if (display_bat_status == SUPER_VERY_BAT_LOW)
						{
							if (ChargeCplFlag)
							{
 								REDLEDCON(0XFF);
							}
							else
							{
								REDLEDCON(0X00);	 
							}
						}					

						if (display_bat_status == VERY_BAT_LOW)
						{
							if (ChargeCplFlag)
							{
 								REDLEDCON(0X07);
							}
							else
							{
								REDLEDCON(0X00);	 
							}
 						}
						else if (display_bat_status == BAT_LOW)
						{ 	
							if (ChargeCplFlag)
							{
 								REDLEDCON(0X03);
							}
							else
							{
								REDLEDCON(0X00);	 
							}
 						}
		
						else if (display_bat_status == BAT_MID) 
						{  
							if (ChargeCplFlag)
							{
 								REDLEDCON(0X01);
							}
							else
							{
								REDLEDCON(0X00);	 
							}
 						}
						else if (display_bat_status == BAT_NORMAL)	
						{
						
							if (!my_charge_ok_for_led)
							{
								if (ChargeCplFlag)
								{
	 								REDLEDCON(0X01);
								}
								else
								{
									REDLEDCON(0X00);	 
								}
							}
							else
							{
								REDLEDCON(0X00);

								IOCTRL_LED(OFF,ON,OFF,LED_MODE_NORMAL);	
 

							}

 						}						
					} 
 				}
				
				//充电完成
				if(!StartAddPower)
				{
	
					//充电电量满足或充电IC的IO指示完成
					//if (my_charge_ok_for_led || ((!(SKeyValue & CHARGE_IN_VALUE)) && (!(SKeyValue & CHARGE_OUT_VALUE))))
					if ((my_charge_ok_for_led || 
					((!(SKeyValue & CHARGE_IN_VALUE))&&((cw_bat.capacity >= BAT_FULL_FLAG_MIN) || (!(SKeyValue & CHARGE_OUT_VALUE))))) &&					
					 (display_temp_cnt == 0))
					{
						ChargeOkCnt++; 
						if(ChargeOkCnt > 10)
						{
							ChargeOkCnt = 0; 
	
							//充电OK后若无操作则显示充电OK和电量
							if (!StartSetTime && !StartSetTemp)
							{
								
								//充电电量满足或充电IC的IO指示完成
								//if (!my_charge_ok_for_led || ((!(SKeyValue & CHARGE_IN_VALUE)) && (!(SKeyValue & CHARGE_OUT_VALUE))))
								if (!my_charge_ok_for_led || 
									((!(SKeyValue & CHARGE_IN_VALUE)) && 
								((cw_bat.capacity >= BAT_FULL_FLAG_MIN) || (!(SKeyValue & CHARGE_OUT_VALUE)))))
								
								{
 
									
									IOCTRL_LED(OFF,ON,OFF,LED_MODE_NORMAL);	
			
									HIGH_BAT_CON();
									//REDLEDCON(0XFF);
									WHITELEDCON(0XFF);
									
									Send_Data_To_iPhone(0XA1);
									Send_Data_To_iPhone(0XA1);

//									Send_Data_To_iPhone(display_bat_status);
//									Send_Data_To_iPhone(cw_bat.capacity>>8);
//									Send_Data_To_iPhone(cw_bat.capacity & 0XFF);

									
								}
							}
	
							IsCharge = IS_CHARGING;
							//ChargePowerCutCnt = 0;	
							Send_Data_To_iPhone(0X32);
	
							//if (display_bat_status == (BAT_NORMAL-1))
							{
								display_bat_status = BAT_NORMAL;
							}
	
							charge_ok_flag = 1;
	
							my_charge_ok_for_led = 1; 

						
						}					 
	 				}
					else
					{
					 	ChargeOkCnt = 0;   
	//					my_charge_ok_for_led = 0;
					}
				}
				else
				{
					ChargeOkCnt = 0;   
				}
				
				//不充电
				if (is_charge_change()) 	
				{
					//第一次上电
					if (IsCharge == IS_UP_POWER)
					{	
	 					 
						//工作和加热未完成亮蓝灯
						//if (!TempIsOk)
						
						if (!StartAddPower)
						{
							if (!(display_bat_status < VERY_BAT_LOW))
							{							
								 
							}
						}	 						
					}

					Send_Data_To_iPhone(0X15);
					Send_Data_To_iPhone(IsCharge);
					Send_Data_To_iPhone(ChargePowerCutCnt);

					if (IsCharge == IS_CHARGING)
					{
						IsCharge = IS_DISCHARGING;
						REDLEDCON(0XFF);  						

						bat_low_flag = 0;

						Send_Data_To_iPhone(0X12);
						Send_Data_To_iPhone(IS_BT_CONNETED());


						//掉电时在加热处理

						if (!IS_BT_CONNETED())
						{
								
							if (ChargePowerCutCnt == 0)
							{
								ChargePowerCutCnt = CHAREG_POWER_CUT_OVER_TIME;	 
							}
	
							
							if (StartAddPower || TempIsOk)// || IS_BT_CONNETED())
							{
								ChargePowerCutCnt = 0;
								Send_Data_To_iPhone(0X33);
							} 
						}
						else
						{
							ChargePowerCutCnt = 0;
							Send_Data_To_iPhone(0X34);
						}

	 					 
						
						if (TempIsOkCnt >= (hot_work_time - temp_hot_ok_time))// || (set_inc_dec_temp_cnt == DEC_TEMP_OVER_TIME))
						{
							if (TempIsOk)
							{
								//NO BOOST
								if ((upadate_temp_time_cnt == 0) && (upadate_temp_delay == 0))
								{
	 
									
									IOCTRL_LED(OFF,ON,OFF,LED_MODE_NORMAL);
								}
							}
							else 
							{
								IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL); 
							}
						
						}
						else 
						{
							IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL); 
						}

						charge_ok_flag = 0;			

						//显示最后的电量   											
					} 
 				}
					
				if ((bluetooth_status == CONNETED))
				{
					ChargePowerCutCnt = 0;	
					//Send_Data_To_iPhone(0X35);
				}				

				if (ChargePowerCutCnt)
				{  	
					Send_Data_To_iPhone(0X13);
					ChargePowerCutCnt--;
					if (ChargePowerCutCnt == 0 )
					{
						Send_Data_To_iPhone(0X14);
						Set_Period_Pwm(0);
						control_motor(POWER_OFF_MOTOR_TIMES);
						POWER_OFF();					
					}
				}
				

 

				//开始加热未达到温度
				if ((TempIsOk == 0) && StartAddPower)
				{	
					//加热未完成时关闭电量显示 
					if (!StartSetTemp && !StartSetTime)
					{
						REDLEDCON(0XFF);

					}

					
 					if (!IO_KEY_HOUT)
					{
						if ((!StartSetTime) && (!StartSetTemp))//加热时设置时间时,不显示加热温度
						{
							if ((DisplayBatTimeCnt == 0))// && (display_temp_cnt == 0))
							{								
							
								if (custom_index == WORK_ON_NORMAL)
								{
									IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);
								}
								else//custom模式混色
								{
									IOCTRL_LED(OFF,ON,ON,LED_MODE_NORMAL);									
								}
								
								controlHotLed(IS_BT_CONNETED());
							}
						}
 					} 
					i = 6;
				}

				//T1 超时
				if (T1_delay_cnt)
				{
					T1_delay_cnt--;
					if (T1_delay_cnt == 0)
					{
						T1_to_T2_flag = 0;
					}
				}


				//显示电量超时
				if (DisplayBatTimeCnt)
				{
					DisplayBatTimeCnt--; 
					display_now_bat();
	 
	 				//Send_Data_To_iPhone(0XC1);
					if (DisplayBatTimeCnt == 0 && (!StartSetTime))
					{
						REDLEDCON(0XFF); 
						//Send_Data_To_iPhone(0XC2);
					}
					
					//第一次加热
					if ((DisplayBatTimeCnt == 0))
					{

						if (!IO_KEY_CHARGE_IN && (display_bat_status >= ENABLE_HOT_LEVEL) && (!charge_open_uint_flag))						
						{
							Enable_White_Led_Toggle = 0; 
							TempIsOkCnt = 0;
							DelayUseCnt = 0; 		
							TempIsOk = 0;
							
							read_eeprom();
							
							LoadLedTempLevelToBuffer();	 
							
							if((SKeyValue & HOUT) && bootloader_ok && (lock == UNLOCK))
							{
								StartAddPower = 1;	
							}
							
							check_hot_over_time_cnt = 0;	
						}						
					}
				}
				
				//不充电退出电量显示
				if (!IO_KEY_CHARGE_IN && !(IO_KEY_CHARGE_OUT))
				{					
					if (DisplayBatTimeCnt == 0 && (!StartSetTime))
					{
						REDLEDCON(0XFF); 						
					}				
				}			 	 
			}

			//加热超时退出
			//if (TempIsOk)
			if (StartAddPower)
			{ 
				if (Flag1s)
				{
					Flag1s = 0;

					if (hot_restart_flag)
					{
						if (cap_move_time < 240)
						{
							cap_move_time++;
							//盖子超时退出
							if (cap_move_time >= CAP_MOVE_OVER_TIME)
							{ 
								reset_work_value();						
							}
						}
					}
				    
					//没有设置温度和开盖子计时否侧停止计时
					//if(!StartSetTemp && (SKeyValue & HOUT ))
					
					if(SKeyValue & HOUT )
					{
						TempIsOkCnt++;
					}					
					
				}	  

				if (TempIsOk)
				{
					if (!StartSetTime)
					{
						Enable_White_Led_Toggle = 1;//OK 后显示当前温度					
					}
			
				}

   				
				if (set_inc_dec_temp_cnt == 0)
				{

					if (!T1_to_T2_flag)
					{
						//正常加热时间
						if (!new_hot_work_time_flag)
						{
							if (first_start && (TempLevelNumber >= 7) && IS_BT_CONNETED())
							{
								//第一次加热，温度太低时间按加长
								hot_work_time = (FIRST_HOT_TIME) + hot_ok_ledg_on_time;
							}
							else
							{
								hot_work_time = TIME_TABLE[TempLevelNumber] + hot_ok_ledg_on_time;
							}
						}
						else
						{
							hot_work_time = TIME_TABLE[TempLevelNumber] + hot_ok_ledg_on_time + (set_update_temp_cnt * (UPDATE_TIME_OVER_TIME/2));
						}
						
					}
					else
					{
						//30s内加热时间
						if (!new_hot_work_time_flag)
						{
							hot_work_time = (TIME_TABLE[TempLevelNumber]/3 + TIME_TABLE_TH) + hot_ok_ledg_on_time;
						}
						else
						{
							hot_work_time = (TIME_TABLE[TempLevelNumber]/3 + TIME_TABLE_TH) + hot_ok_ledg_on_time + (set_update_temp_cnt * (UPDATE_TIME_OVER_TIME/2));
						}
						
					}
				}
				else
				{	
					//设置温度后加热时间
								
					if (!new_hot_work_time_flag)
					{
						hot_work_time = set_inc_dec_temp_cnt + hot_ok_ledg_on_time;	
					}
					else
					{
						hot_work_time = set_inc_dec_temp_cnt + hot_ok_ledg_on_time + (set_update_temp_cnt * (UPDATE_TIME_OVER_TIME/2));
					}						
				}			
					
				temp_hot_ok_time = hot_ok_ledg_on_time;
									
				if ((TempIsOkCnt >= (hot_work_time - (temp_hot_ok_time)) && 
					TempIsOkCnt < (hot_work_time - MOTOR_OFF_TIME))
				)
				{
					TempIsOk = 1;//OK 后显示当前温度
					if (!StartSetTime)
					{
						Enable_White_Led_Toggle = 1;//OK 后显示当前温度	
					}

					
					
					
					{
						
						{
							highest_temp_need_restart = 1;
						}
					}

					if (!hot_restart_flag)
					{
						if ((upadate_temp_time_cnt == 0) && (upadate_temp_delay == 0))
						{
 
							
							IOCTRL_LED(OFF,ON,OFF,LED_MODE_NORMAL);
						}
						
						
						if (TempIsOkCnt == (hot_work_time - (temp_hot_ok_time)))
						{
							//不是设置降温则震动电机
							
							//if (set_inc_dec_temp_cnt != DEC_TEMP_OVER_TIME)
							//第一次加热OK才震动
							if (!new_hot_work_time_flag)								
							{

								for (idx = 0; idx < 3; idx++)
								{							
									IOCTRL(MOTOR,MOTOR_ON); 						
									DelayMS(LED_FLASH_OVER_TIME);							
									IOCTRL(MOTOR,MOTOR_OFF); 						
									DelayMS(LED_FLASH_OVER_TIME);
								}
							}
						}	  

					}					
				}	 
 

				if (TempIsOkCnt > hot_work_time) 
				{		
					//1.等待更新温度时间到之后才关闭加热
					if ((!new_hot_work_time_flag) && (upadate_temp_time_cnt == 0) && (upadate_temp_delay == 0))
					{
						//绿灯后，退出standby
						if (TempIsOk)
						{
							control_motor(GOTO_STANDBY_MOTOR_TIMES);
						}
						
						reset_work_value();
												
						//开始T2过程
						T1_to_T2_flag = 1; 
						//if (T1_delay_cnt == 0)
						{
							T1_delay_cnt = T1_DELAY_OVER_TIME;	   				
						}						
					}					

					//2.更新温度温度后时间到
					if ((new_hot_work_time_flag) && 
						(upadate_temp_time_cnt == 0) && (upadate_temp_delay == 0))
					{
						//没有按键后者超出更新次数关闭加热
						if ((!(SKeyValue & TEMP_INC_VALUE)) || (set_update_temp_cnt >= SET_UPDATE_TEMP_MAX))
						{	
							//绿灯后，退出standby
							if (TempIsOk)
							{
								control_motor(GOTO_STANDBY_MOTOR_TIMES);
							}
						
							reset_work_value();
													
							//开始T2过程
							T1_to_T2_flag = 1; 
							//if (T1_delay_cnt == 0)
							{
								T1_delay_cnt = T1_DELAY_OVER_TIME;	   				
							}	
						
						}
					}					
				}
			}
			else
			{		
 
			}     	  

  		}	
    }
}


void LowBatOverTimeClosePower(void)
{
	static WORD TimeCnt = 0;

	//没有充电
	if ((SKeyValue & CHARGE_IN_VALUE))
	{
		if (display_bat_status < VERY_BAT_LOW)
		{
			TimeCnt++;
			if (TimeCnt > BAT_LOW_LEVEL_TIME)
			{
				TimeCnt = 0;
				Set_Period_Pwm(0); 
				//EA = 0;	
 
				IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);	
				WHITE_RED_LED_ALL_OFF();

				 
				Set_Period_Pwm(0);
				control_motor(POWER_OFF_MOTOR_TIMES);

				Send_Data_To_iPhone(0X44);
				Send_Data_To_iPhone(0X44);
				
				flash_error_led(ERROR_LED_TIMES);
				
				send_status_to_app(STATUS_POWER_OFF,SEND_DELAY_LONG_TIME);
				POWER_OFF(); 
			}	
		}
		else
		{
		 	TimeCnt = 0;	
		}	 
	}	 
}

 

void CheckSensorError(void)
{
	static WORD SensorTimeCnt = 0;
	WORD sensor_error_cnt = 0;
	
	BYTE i = 0;
	
	//等待加热一段时间后检测
//	if (TempIsOkCnt >= WAIT_SENSOR_TIME)
	{
		
	//加热一段时间后,没有温度变化关闭电源
		if (StartAddPower && (SKeyValue & HOUT ))
		//if (StartAddPower)
		{  
			SensorTimeCnt++;   

			//电池电压低加热时间长
			if (display_bat_status <= VERY_BAT_LOW)
			{
				sensor_error_cnt = SENSOR_EEROR_LONG_OVER_TIME;
			}
			else
			{
				sensor_error_cnt = SENSOR_EEROR_SHORT_OVER_TIME;			
			}
			
			if (SensorTimeCnt > sensor_error_cnt)
			{ 	
				
				if((CurrentSenorTemp1 < HOT_MIN_TH) || (CurrentSenorTemp2 < HOT_MIN_TH))				
				//if(CurrentSenorTemp1 < HOT_MIN_TH) 
				{
 
					
					IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);
					
					Send_Data_To_iPhone(0X33);
					Send_Data_To_iPhone(0X33);
					
					for (i = 0; i < 6; i++)
					{
						error_led_on_off();
					}  				

					reset_work_value();

					//if (!IO_KEY_CHARGE_IN)
					{
						Set_Period_Pwm(0);
						//control_motor(POWER_OFF_MOTOR_TIMES);
						send_status_to_app(STATUS_ERROR,SEND_DELAY_LONG_TIME);
						//POWER_OFF(); 
					}

				}
				else if (CurrentSenorTemp1 < FIRST_HOT_MIN_TH)
				{
					first_start = 1;
				}
					
			}  	
			else if (SensorTimeCnt > ALL_HOT_OVER_TIME)
			{
				SensorTimeCnt = 0;
			}	
		}
		else
		{
			SensorTimeCnt = 0;	
		}
	}
//	else
//	{
//		SensorTimeCnt = 0;	
//	}
}

//系统超时睡眠
void CheckSystemIdle(void)
{

	//没有充电,蓝牙未连接
	if ((SKeyValue & CHARGE_IN_VALUE) && (!IS_BT_CONNETED()))
	{
	 	
	 	if (!StartAddPower)
		{  
			SystemIdleTimeCnt++;   
			if (SystemIdleTimeCnt > SYSTEM_IDLE_OVER_TIME)
			{      				
				SystemIdleTimeCnt = 0;
				Set_Period_Pwm(0); 	
				//EA = 0;	
				WHITE_RED_LED_ALL_OFF();
				control_motor(POWER_OFF_MOTOR_TIMES);							   	
				POWER_OFF();  
			}	  
		}
		else
		{
			SystemIdleTimeCnt = 0;	
		}
	}
	else
	{
		SystemIdleTimeCnt = 0;	
	}

}

void controlHotLed(uint8_t bt) 
{


	HotCplFlag = ~HotCplFlag; 
 
 

	
	if (TempLevelNumber == 1 || TempLevelNumber == 2)
	{
		if (HotCplFlag)
		{
			WHITELEDCON(0X07);
		}
		else
		{
			WHITELEDCON(0X0F);	 
		}	 
	}
 
	if (TempLevelNumber == 3 || TempLevelNumber == 4)
	{
		if (HotCplFlag)
		{
			WHITELEDCON(0X0B);
		}
		else
		{
			WHITELEDCON(0X0F);	 
		}	
	}

	if (TempLevelNumber == 5 || TempLevelNumber == 6)
	{
		if (HotCplFlag)
		{
			WHITELEDCON(0X0D);
		}
		else
		{
			WHITELEDCON(0X0F);	 
		}	
	}				

	if (TempLevelNumber == 7 || TempLevelNumber == 8)
	{
		if (HotCplFlag)
		{
			WHITELEDCON(0X0E);
		}
		else
		{
			WHITELEDCON(0X0F);	 
		}	
	}	
		
 

}
 
void reset_work_value(void)
{
	//超时退出BOOST
	if ((upadate_temp_time_cnt != 0) || (upadate_temp_delay != 0))
	{
		send_status_to_app(STATUS_EXIT_BOOST,SEND_DELAY_SHORT_TIME);
	}
	
	first_start = 0;

	inc_temp = 0;
	upadate_temp_delay = 0;
	set_update_temp_cnt = 0;
	new_hot_work_time_flag = 0;
	upadate_temp_time_cnt = 0;

	AdcCnt = 0;	
	temperature1AdcResult = 0;
	temperature2AdcResult = 0;	
	CurrentSenorTemp1 = 0;
	CurrentSenorTemp2 = 0;
	
	check_hot_over_time_cnt = 0;
	
	highest_temp_need_restart_cnt = 0;
	highest_temp_need_restart = 0;
	Enable_White_Led_Toggle = 0; 
	TempIsOkCnt = 0;
	DelayUseCnt = 0;
	cap_move_time = 0;
	hot_restart_flag = 0;
	set_inc_dec_temp_cnt = 0;
	
	TempIsOk = 0;
	StartAddPower = 0;

	Set_Period_Pwm(0); 
	WHITE_RED_LED_ALL_OFF();
	IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);
	IOCTRL(MOTOR,MOTOR_OFF);	 
}

void my_reset(void)
{
	//超时退出BOOST
	if ((upadate_temp_time_cnt != 0) || (upadate_temp_delay != 0))
	{
		send_status_to_app(STATUS_EXIT_BOOST,SEND_DELAY_SHORT_TIME);
	}
	
	first_start = 0;

	inc_temp = 0;
	upadate_temp_delay = 0;
	set_update_temp_cnt = 0;
	new_hot_work_time_flag = 0;
	upadate_temp_time_cnt = 0;

	AdcCnt = 0;	
	temperature1AdcResult = 0;
	temperature2AdcResult = 0;	
	CurrentSenorTemp1 = 0;
	CurrentSenorTemp2 = 0;
	
	check_hot_over_time_cnt = 0;
	
	highest_temp_need_restart_cnt = 0;
	highest_temp_need_restart = 0;
	Enable_White_Led_Toggle = 0; 
	TempIsOkCnt = 0;
	DelayUseCnt = 0;
	cap_move_time = 0;
	hot_restart_flag = 0;
	set_inc_dec_temp_cnt = 0;  
	
	TempIsOk = 0;
	StartAddPower = 0;
 
 	Set_Period_Pwm(0); 
	WHITE_RED_LED_ALL_OFF();
	IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);
	IOCTRL(MOTOR,MOTOR_OFF);
	HW_POWER_OFF();


}

void hot_over_time_reset(void)
{
	//超时退出BOOST
	if ((upadate_temp_time_cnt != 0) || (upadate_temp_delay != 0))
	{
		send_status_to_app(STATUS_EXIT_BOOST,SEND_DELAY_SHORT_TIME);
	}
	
	first_start = 0;

	inc_temp = 0;
	upadate_temp_delay = 0;
	set_update_temp_cnt = 0;
	new_hot_work_time_flag = 0;
	upadate_temp_time_cnt = 0;

	AdcCnt = 0;	
	temperature1AdcResult = 0;
	temperature2AdcResult = 0;	
	CurrentSenorTemp1 = 0;
	CurrentSenorTemp2 = 0;
	
	check_hot_over_time_cnt = 0;
	
	highest_temp_need_restart_cnt = 0;
	highest_temp_need_restart = 0;
	Enable_White_Led_Toggle = 0; 
	TempIsOkCnt = 0;
	DelayUseCnt = 0;
	cap_move_time = 0;
	hot_restart_flag = 0;
	set_inc_dec_temp_cnt = 0;  
	
	TempIsOk = 0;
	StartAddPower = 0;
 
 	Set_Period_Pwm(0); 
	WHITE_RED_LED_ALL_OFF();
	IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);
	IOCTRL(MOTOR,MOTOR_OFF);
	HW_POWER_OFF();
	
	
}


void error_led_on_off(void)
{
	
	IOCTRL_LED(ON,OFF,OFF,LED_MODE_NORMAL);	
	DelayMS(ERROR_DELAY_OVER_TIME);	  
	IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);	
	DelayMS(ERROR_DELAY_OVER_TIME);	   
}

void no_cup_led_on_off(void)
{
	
	IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);
	DelayMS(ERROR_DELAY_OVER_TIME);	  
	IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);
	DelayMS(ERROR_DELAY_OVER_TIME);	   
}

UINT8 shake_voltage(void)
{
 
	UINT8 temp;	

 
	if (cw_bat.capacity <= 8)//3
	{
		temp = SUPER_VERY_BAT_LOW; 	
	} 
	else if (cw_bat.capacity <= 35)
	{
		temp = VERY_BAT_LOW;
	}
	else if (cw_bat.capacity <= 62)//73
	{
		temp = BAT_LOW;
	}
	else if (cw_bat.capacity <= 83)//87
	{
		temp = BAT_MID;

	}
	else if (cw_bat.capacity <= 100)
	{
		temp = BAT_NORMAL;		
	}

	if (cw_bat.capacity == 0xFF)
	{
		temp = SUPER_VERY_BAT_LOW; 	
	}

	//TEST
	//temp = BAT_NORMAL;

	return temp;

}

void flash_error_led(UINT8 times)
{	
 	while(times--)
	{
		error_led_on_off();	
		DelayMS(LED_FLASH_OVER_TIME);	  
	}  
}

void control_motor(UINT8 times)
{	

	UINT8 i;

	for (i = 0; i < times; i++)
	{	
		IOCTRL(MOTOR,MOTOR_ON); 
		DelayMS(LED_FLASH_OVER_TIME);	  
 		
		IOCTRL(MOTOR,MOTOR_OFF); 
		DelayMS(LED_FLASH_OVER_TIME);
 	}

}


void app_init(void)
{
	//LED_Init();

	iic_init();

	cw_bat_init();

//	InitFuel();
	
	adc_start(ADC_CONFIG_PSEL_AnalogInput6);
	
	app_tick_start();
	
	
	//timer2_init(50000);
	
	
	PWM_Init();
	
	Write_Byte_Data_Flash();
	
}


void app_process(void)
{
	main2();
}


void bluetooth_rcv_process(uint8_t *pdata,uint16_t len)
{
	uint8 i = 0;
	uint8 sum = 0;
	uint8_t change_flag = 0;
 
	
	
	if (((pdata[0] == ONE_HEAD) && (pdata[1] == TWO_HEAD_APP2MCU) &&
		(pdata[2] == LEN) && (pdata[3] == ID_HIGH) && (pdata[4] == ID_LOW))	
		||((pdata[0] == ONE_HEAD) && (pdata[1] == TWO_HEAD_CUSTOM_WRITE)) 
	)			
	{
		//flash_error_led(ERROR_LED_TIMES);//TEST
		
		
		
		//第一个数据包
		if (len == LEN)
		{			
			for (i = 0;i < LEN;i++)
			{
				rcv_buffer[i] = pdata[i];
			}
			
			
		}
	}
	else//读取剩下字节
	{
		
		//设置custom模式
		if ((rcv_buffer[0] == ONE_HEAD) && (rcv_buffer[1] == TWO_HEAD_CUSTOM_WRITE))
		{
			
			//第二个数据包
			if (len == LEN_NEXT)
			{		
				for (i = 0;i < LEN_NEXT;i++)
				{
					rcv_buffer[LEN+i] = pdata[i];
				}	
				
				//校验
				for (i = 2;i < (LEN + LEN_NEXT - 1);i++)
				{
					sum += rcv_buffer[i];					
				}					
			}
			
			if ((len == LEN_NEXT) && (sum == rcv_buffer[LEN + LEN_NEXT - 1]))
			{
				
				 		
				
				//提取温度值
				for(i = 0; i < 8;i++)
				{
					TableValue[1 + i] = rcv_buffer[3 + 2 * i + 1 ];
				}					
				
			
				
				TempLevelNumber = rcv_buffer[R3_INDEX];
				
				TempLevel = TableValue[TempLevelNumber];		 			 	

				AppTempLevel = TempLevel; 
				write_eeprom_flag = WRITE_EEPROM_SHORT_OVER_TIME;
				
				//恢复出产设置
				//if ((TempLevelNumber == TEMP_INDEX_8) && (WORK_ON_NORMAL == check_temp_default(TableValue,TableValueConst)))
				
				if (WORK_ON_NORMAL == check_temp_default(TableValue,TableValueConst))
				{
					custom_index = WORK_ON_NORMAL;					
				}
				else
				{
					custom_index = WORK_ON_CUSTOM;
				}				
				
				
				//flash_error_led(ERROR_LED_TIMES);//TEST
			}
			
		}
		
		if ((rcv_buffer[0] == ONE_HEAD) && (rcv_buffer[1] == TWO_HEAD_APP2MCU) &&
			(rcv_buffer[2] == LEN) && (rcv_buffer[3] == ID_HIGH) && (rcv_buffer[4] == ID_LOW)
		)		
		{

			//第二个数据包
			if (len == LEN_NEXT)
			{		
				for (i = 0;i < LEN_NEXT;i++)
				{
					rcv_buffer[LEN+i] = pdata[i];
				}	
				
				//校验
				for (i = 2;i < (LEN + LEN_NEXT - 1);i++)
				{
					sum += rcv_buffer[i];					
				}					
			}
			
			//第二个数据包			
			if ((len == LEN_NEXT) && (sum == rcv_buffer[LEN + LEN_NEXT - 1]))
			{		
				for (i = 0;i < LEN_NEXT;i++)
				{
					rcv_buffer[LEN+i] = pdata[i];
				}	
				
				//flash_error_led(ERROR_LED_TIMES);//TEST
				
				if (rcv_buffer[R1_INDEX] == CMD_READ_CUSTOM_TEMP)
				{
					 
					custom_flag = 1;
 			
				}				
				else if (rcv_buffer[R1_INDEX] == CMD_SET)
				{	
					//flash_error_led(ERROR_LED_TIMES);//TEST
					
//					change_flag = ((rcv_buffer[R2_INDEX] & BIT_CUSTOMISE_TEMP ) == BIT_CUSTOMISE_TEMP);
//					
//					if (change_flag != custom_index)
//					{		
//						if (!change_flag)
//						{
//							custom_index = WORK_ON_NORMAL;						
//						}				
//						else
//						{					
//							custom_index = WORK_ON_CUSTOM;
//						}			
//						
//						write_eeprom_flag = WRITE_EEPROM_OVER_TIME;
//					}	
					
					change_flag = ((rcv_buffer[R2_INDEX] & BIT_LOCK ) == BIT_LOCK);
					
					if (change_flag != lock)
					{		
						if (change_flag)
						{
							lock = LOCK;						
						}				
						else
						{					
							lock = UNLOCK;
						}						
						write_eeprom_flag = WRITE_EEPROM_SHORT_OVER_TIME;
					}					
				}
				
				else if (rcv_buffer[R1_INDEX] == CMD_NULL)//正常命令
				{
				
					//设置温度
					if ((display_bat_status >= ENABLE_HOT_LEVEL) && (rcv_buffer[11] >= SET_TEMP_MIN) && (rcv_buffer[11] <= SET_TEMP_MAX))
					{
						//if (!StartAddPower && (set_time_ok_quit_cnt == 0))
						if ((!StartSetTime && !StartSetTemp) && (set_time_ok_quit_cnt == 0) && bootloader_ok)
						{
							if (AppTempLevel != rcv_buffer[11])
							{					
								bt_connect_cnt = 0;
								
 
								IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);	
								
								WHITE_RED_LED_ALL_OFF();
								
								//退出BOOST
								if ((upadate_temp_time_cnt != 0) || (upadate_temp_delay != 0))
								{
									send_status_to_app(STATUS_EXIT_BOOST,SEND_DELAY_SHORT_TIME);
								}

						
								//加热后再次设置温度
								if (StartAddPower)
								{
									if (highest_temp_need_restart)
									{
										set_inc_dec_temp_cnt = DEC_TEMP_OVER_TIME;
										TempIsOkCnt = 0;
										DelayUseCnt = 0; 
										TempIsOk = 0; 
									}
									else
									{
										set_inc_dec_temp_cnt = INC_TEMP_OVER_TIME;
									}
								}
																				
								//重新计时
								check_hot_over_time_cnt = 0;							
								Enable_White_Led_Toggle = 0; 
								TempIsOkCnt = 0;
								DelayUseCnt = 0; 		
								TempIsOk = 0;


								//清除更新温度
								upadate_temp_delay = 0;
								set_update_temp_cnt = 0;
								new_hot_work_time_flag = 0;
								upadate_temp_time_cnt = 0;
								//check_hot_over_time_cnt = 0;					
								SetTempOverCnt = 0;
								SystemIdleTimeCnt = 0;
								
								//清除BOOST升温
								inc_temp = 0;

								
								
								custom_index = WORK_ON_NORMAL;
								
								
								AppTempLevel = rcv_buffer[11];					
								
								TempLevel = AppTempLevel;
							
								
								TempLevelNumber = getBTTempLevelNumber(AppTempLevel);			
								write_eeprom_flag = WRITE_EEPROM_OVER_TIME;


								//DisplayBatTimeCnt = DEFAULT_DISP_BAT_TIME; 
								//display_now_bat();	
								
								//显示加热温度					
								LoadLedTempLevelToBuffer();	
								Enable_White_Led_Toggle = 1;
								
								if (display_temp_cnt == 0)
								{
									display_temp_cnt = DISPLAY_TEMP_OVER_TIME;
								}

							}
							
						}
					}		

 
					
					else if ((rcv_buffer[11] == CMD_POWER_OFF))
					{
						//超时退出BOOST
						if ((upadate_temp_time_cnt != 0) || (upadate_temp_delay != 0))
						{
							send_status_to_app(STATUS_EXIT_BOOST,SEND_DELAY_SHORT_TIME);
						}
				
						//不充电才能关机
						if (!IO_KEY_CHARGE_IN)
						{
								
							Set_Period_Pwm(0);
							
							control_motor(POWER_OFF_MOTOR_TIMES);	
							
							send_status_to_app(STATUS_POWER_OFF,SEND_DELAY_LONG_TIME);
							POWER_OFF();
							
						}
					
							
					}
					//加热				
					else if (((rcv_buffer[11] == CMD_START_HEAT)))
					{
						if(SKeyValue & HOUT )
						{
							if ((display_bat_status >= ENABLE_HOT_LEVEL) && (!StartAddPower) && bootloader_ok && (lock == UNLOCK))
							{
								
								IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);	
								
								Enable_White_Led_Toggle = 0; 
								TempIsOkCnt = 0;
								DelayUseCnt = 0; 		
								TempIsOk = 0;
								StartAddPower = 1;

								check_hot_over_time_cnt = 0;
								my_charge_ok_for_led = 0; 						
				
 	

					
								if (T1_delay_cnt)
								{
									T1_delay_cnt = 0;
									T1_to_T2_flag = 1; 	
									
								}
						
								SystemIdleTimeCnt = 0;	
				
								read_eeprom();
								
								//显示加热温度
								
							
//								if (custom_index == WORK_ON_NORMAL)
//								{
//									TempLevelNumber = getBTTempLevelNumber(AppTempLevel);			
//								}
								
								LoadLedTempLevelToBuffer();	 	
						
						
								DisplayBatTimeCnt = DEFAULT_DISP_BAT_TIME; 
								display_now_bat();	
								
								IOCTRL(MOTOR,MOTOR_ON);
								DelayMS(LED_FLASH_OVER_TIME);
								IOCTRL(MOTOR,MOTOR_OFF); 
									
								bluetooth_send_process(NORMAL_SEND);
							}
							
							if (lock == LOCK)
							{
								flash_lock(SET_LOCK_CNT,SET_LOCK_FLASH_TIME); 
								IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);
							}
						}
						else
						{
 
							IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);	
							
							if (lock == UNLOCK)
							{
								for (i = 0;i < 6;i++)
								{
									no_cup_led_on_off();
								}
							}
							else if (lock == LOCK)
							{
								flash_lock(SET_LOCK_CNT,SET_LOCK_FLASH_TIME); 							
							}						
							
							IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);
						}
						
					}
					
					else if (((rcv_buffer[11] == CMD_STOP_HEAT)))//不加热				
					{
						//超时退出BOOST
						if ((upadate_temp_time_cnt != 0) || (upadate_temp_delay != 0))
						{
							send_status_to_app(STATUS_EXIT_BOOST,SEND_DELAY_SHORT_TIME);
						}
						
						//绿灯后，退出standby
						if (TempIsOk)
						{
							control_motor(GOTO_STANDBY_MOTOR_TIMES);
						}
							
						reset_work_value();												
					}
					
					else if (((rcv_buffer[11] == CMD_BOOST_HEAT)))//加升温度
					{
						if (TempIsOk)
						{					
							if (SKeyValue & HOUT )
							{
								update_temp();					
							}				
						}			
					}		
					
					if ((display_bat_status >= ENABLE_HOT_LEVEL) && (rcv_buffer[9] >= HOT_OK_TIME_MIN) && (rcv_buffer[9] <= HOT_OK_TIME_MAX))
					{
						if ((!StartAddPower) && (display_temp_cnt == 0) && (!StartSetTime) && bootloader_ok)
						{					
							if (hot_ok_ledg_on_time != rcv_buffer[9])
							{
								
								hot_ok_ledg_on_time = rcv_buffer[9];
								
									
								StartSetTime = 1;	
								Enable_White_Led_Toggle = 0;  
								WHITE_RED_LED_ALL_OFF();
								set_time_ok_quit_cnt = APP_SET_TIME_OK_QUIT_INIT_TIME;
								HotTimeNumber = getHotTimeNumber(hot_ok_ledg_on_time);
								display_now_time();
								
 
								IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);	
								
								write_eeprom_flag = WRITE_EEPROM_OVER_TIME;

								
							}
						}
					}	

					
	//				for (i = 0;i < (LEN + LEN_NEXT);i++)
	//				{
	//					Send_Data_To_iPhone(rcv_buffer[i]);
	//				}

					for (i = 0;i < (LEN + LEN_NEXT);i++)
					{
						rcv_buffer[i] = 0;
					}
				}
			}
		
		}
		
	}
		
}


UINT8 status = STATUS_STANDBY;

void set_status(uint8_t my_status)
{
	if (my_status == NORMAL_SEND)
	{		
		//没有加热	
		if (!StartAddPower)
		{
			//未充电	
			if ((SKeyValue & CHARGE_IN_VALUE))
			{
				status = STATUS_STANDBY;
			}		
			//充电
			if (!(SKeyValue & CHARGE_IN_VALUE))
			{
				status = STATUS_CHARGING;
			}
		

			//在充电而且充电完成了
			if ((!(SKeyValue & CHARGE_IN_VALUE)) 
			 && ((!(SKeyValue & CHARGE_OUT_VALUE)) || my_charge_ok_for_led ))
			{
				status = STATUS_CHARGE_OK;
			}
		
		}
		//加热
		else
		{
			
			//正在加热和未充电
			if ((!TempIsOk) && (SKeyValue & CHARGE_IN_VALUE))
			{
				status = STATUS_HOTTING;
			}		
			//加热OK和未充电
			if ((TempIsOk) && (SKeyValue & CHARGE_IN_VALUE))
			{
				status = STATUS_HOT_OK;
			}

			
			
			//加热未完成而且在充电
			if ((!TempIsOk) && (!(SKeyValue & CHARGE_IN_VALUE)))
			{
				status = STATUS_HOTTING_AND_CHARGING;
			}
			//加热完成而且在充电
			if ((TempIsOk) && (!(SKeyValue & CHARGE_IN_VALUE)))
			{
				status = STATUS_HOT_OK_AND_CHARGING;
			}
			
		}
	}
	else if (my_status == STATUS_POWER_OFF)
	{
		status = STATUS_POWER_OFF;		
	}
	else if (my_status == STATUS_ERROR)
	{
		status = STATUS_ERROR;			
	}
	else if (my_status == STATUS_BOOST)
	{
		status = STATUS_BOOST; 
	}
	else if (my_status == STATUS_EXIT_BOOST)
	{
		status = STATUS_EXIT_BOOST; 
	}
 
}
	


uint8_t send_buffer[20] = {0};

void bluetooth_send_process(uint8_t my_status)
{
	uint32_t err_code;

	uint8_t i;
	uint8_t sum = 0;
 	
	if (bluetooth_status == CONNETED)
	{	
		set_status(my_status); 
		
		send_buffer[0] = ONE_HEAD;
		send_buffer[1] = TWO_HEAD_MCU2APP;
		send_buffer[2] = LEN;		
		send_buffer[3] = ID_HIGH;			
		send_buffer[4] = ID_LOW;
		//send_buffer[5] = *(uint8_t volatile *)FWVERSION_ADDR;
		send_buffer[5] = fw_version;
		
		
		send_buffer[6] = status;
		send_buffer[7] = SKeyValue;
		send_buffer[8] = SECTION_ONE_TIME;
		send_buffer[9] = hot_ok_ledg_on_time;
		send_buffer[10] = TempLevelNumber;
		send_buffer[11] = AppTempLevel;
		
		if (StartAddPower)
		{
			if (TempIsOk)
			{	
				if (upadate_temp_time_cnt == 0)
				{
					if (inc_temp)
					{
						inc_temp--;						
					}
					
 
					
					send_buffer[12] = AppTempLevel + inc_temp;

					if (send_buffer[12] >= TEMP_MAX)
					{
						send_buffer[12] = TEMP_MAX;
					}
					
				}
				else
				{	
					
					if (!StartSetTime)
					{
						if (inc_temp < 10)
						{
							inc_temp++;					
							inc_temp++;					
						}
					}
					else
					{
						inc_temp = 0;	
						TempLevel = AppTempLevel;						
					}
					
					send_buffer[12] = AppTempLevel + inc_temp;
					
					if (send_buffer[12] >= TEMP_MAX)
					{
						send_buffer[12] = TEMP_MAX;
					}
				}	
			}
			else
			{
				
				{
					
					
					send_buffer[12] = CurrentSenorTemp1 + (TempLevelNumber*20)/10 + 3;

					if (send_buffer[12] >= TEMP_MAX)
					{
						send_buffer[12] = TEMP_MAX;
					}					
				}
			}				
		}
		else
		{
			send_buffer[12] = CurrentSenorTemp1;			
		}
		
		if (my_status == STATUS_POWER_OFF)
		{
			send_buffer[12] = 0;			
		}

		if (send_buffer[12] >= TEMP_MAX)
		{
			send_buffer[12] = TEMP_MAX;
		}
		
		
					
		if (StartSetTime)
		{
			inc_temp = 0;	
			TempLevel = AppTempLevel;				
		}
					
		send_buffer[13] = M_ON;
		send_buffer[14] = LIGHT_EFFECT;
		send_buffer[15] = cw_bat.capacity;
		send_buffer[16] = display_bat_status;
		send_buffer[R1_INDEX] = CMD_SET;
		send_buffer[R2_INDEX] = 0;
		

		if (custom_index == WORK_ON_NORMAL)
		{
			send_buffer[R2_INDEX] &= ~(1<<0);	
		}
		else
		{			
			send_buffer[R2_INDEX] |= (1<<0);			
		}

		
		//设置所得状态
		if (lock == LOCK)
		{
			send_buffer[R2_INDEX] |= (1<<1);
		}
		else
		{
			send_buffer[R2_INDEX] &= ~(1<<1);			
		}


		send_buffer[R3_INDEX] = 0;	
		sum = 0;
		for (i = 2;i < LEN;i++)
		{
			sum += send_buffer[i];
		}			

		err_code = ble_nus_send_string(&m_nus, send_buffer,LEN);
		if (err_code != NRF_ERROR_INVALID_STATE)
		{
			//APP_ERROR_CHECK(err_code);
		}
		
		nrf_delay_ms(50);

 
		
		send_buffer[0] = TableValue[1];

		sum += send_buffer[0];
		
		send_buffer[1] = sum;
		err_code = ble_nus_send_string(&m_nus, send_buffer,LEN_NEXT);
		if (err_code != NRF_ERROR_INVALID_STATE)
		{
			//APP_ERROR_CHECK(err_code);
		}
	
	}
}


void bluetooth_send_custom(void)
{
	uint32_t err_code;

	uint8_t i;
	uint8_t sum = 0;
 	
	if (bluetooth_status == CONNETED)
	{	
 		send_buffer[0] = ONE_HEAD;
		send_buffer[1] = TWO_HEAD_CUSTOM_READ;
		send_buffer[2] = LEN;
		
	 
 
		for(i = 0; i < 8;i++)
		{
			send_buffer[3 + 2 * i] = 0;			 
		}

		for(i = 0; i < 8;i++)
		{
			send_buffer[3 + 2 * i + 1 ] = TableValue[1 + i];
		}		
		
		
		send_buffer[R3_INDEX] = TempLevelNumber;
		
 
		sum = 0;
		for (i = 2;i < LEN;i++)
		{
			sum += send_buffer[i];
		}			

		nrf_delay_ms(50);
		err_code = ble_nus_send_string(&m_nus, send_buffer,LEN);
		if (err_code != NRF_ERROR_INVALID_STATE)
		{
			//APP_ERROR_CHECK(err_code);
		}
		
		nrf_delay_ms(50);

 
		
		send_buffer[0] = TableValue[TempLevelNumber];;

		sum += send_buffer[0];
		
		send_buffer[1] = sum;
		err_code = ble_nus_send_string(&m_nus, send_buffer,LEN_NEXT);
		if (err_code != NRF_ERROR_INVALID_STATE)
		{
			//APP_ERROR_CHECK(err_code);
		}
		
		nrf_delay_ms(50);
		
		//flash_error_led(ERROR_LED_TIMES);//TEST
	
	}
}



uint8_t IS_BT_CONNETED(void)
{
	return (bluetooth_status == CONNETED);
}
 

//蓝牙连接处理
void app_bluetooth_connected_process(void)
{
	//WHITE_RED_LED_ALL_OFF();
 
	IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);	
	
	bluetooth_status = CONNETED;
 
	custom_flag = 250;
	
	Send_Data_To_iPhone(0X99);
	Send_Data_To_iPhone(0X91);

}
//蓝牙掉线处理
void app_bluetooth_disconnected_process(void)
{
	//WHITE_RED_LED_ALL_OFF();

	IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);	
	bluetooth_status = DISCONNETED;	
	
	 

	Send_Data_To_iPhone(0X99);
	Send_Data_To_iPhone(0X92);
	
}



uint8_t getBTTempLevelNumber(uint8_t temp)
{
	if (temp <= (TEMP_LEVEL_ONE + TEMP_LEVEL_TWO)/2)
	{
		return 1;
	}
	else if (temp <= (TEMP_LEVEL_TWO + TEMP_LEVEL_THREE)/2)
	{
		return 2;
	}	
	else if (temp <= (TEMP_LEVEL_THREE + TEMP_LEVEL_FOUR)/2)
	{
		return 3;
	}	
	else if (temp <= (TEMP_LEVEL_FOUR + TEMP_LEVEL_FIVE)/2)
	{
		return 4;
	}	
	else if (temp <= (TEMP_LEVEL_FIVE + TEMP_LEVEL_SIX)/2)
	{
		return 5;
	}
	else if (temp <= (TEMP_LEVEL_SIX + TEMP_LEVEL_SEVEN)/2)
	{
		return 6;
	}	
	else if (temp <= (TEMP_LEVEL_SEVEN + TEMP_LEVEL_EIGHT)/2)
	{
		return 7;
	}	
	else 
	{
		return 8;
	}		
}
uint8_t getHotTimeNumber(uint8_t time)
{

	if (time < (HOT_OK_LEDG_ON_TIME[1] + HOT_OK_LEDG_ON_TIME[2])/2)
	{
		return 1;
	}
	else if (time < (HOT_OK_LEDG_ON_TIME[2] + HOT_OK_LEDG_ON_TIME[3])/2)
	{
		return 2;
	}		
	else if (time < (HOT_OK_LEDG_ON_TIME[3] + HOT_OK_LEDG_ON_TIME[4])/2)
	{
		return 3;
	}	
	else
	{
		return 4;
	}	
	
	
}

void bt_connect_indicate(void)
{

	
	
	if ((!StartAddPower) && (!StartSetTime) && (!StartSetTemp) && IS_BT_CONNETED() && (!IO_KEY_CHARGE_IN) && (display_temp_cnt == 0))
	{		
		bt_connect_cnt++;
		
		if (display_bat_status >= ENABLE_HOT_LEVEL)
		{	
				
			if (bt_connect_cnt == 10)
			{
			 
				IOCTRL_LED(OFF,OFF,ON,LED_MODE_NORMAL);
			}			
			else if(bt_connect_cnt == 80)
			{
				IOCTRL_LED(OFF,OFF,OFF,LED_MODE_NORMAL);
			}		
			else if(bt_connect_cnt >= 100)
			{
				bt_connect_cnt = 0;				
			}
				
		}
	}
	else
	{
		bt_connect_cnt = 0;
		
	}
}

void restore_display_temp(void)
{
	if (display_temp_cnt)
	{
		display_temp_cnt--;
		if (display_temp_cnt == 0)
		{
			Enable_White_Led_Toggle = 0;
			
			//重新计时
			check_hot_over_time_cnt = 0;			
			set_temp_or_time_quit();
			
			if (!StartAddPower)
			{
				WHITE_RED_LED_ALL_OFF();
			}
		}
	}
}

void send_status_to_app(uint8_t system_status,uint16_t delay_time)
{
	nrf_delay_ms(60);
	bluetooth_send_process(system_status);
	nrf_delay_ms(delay_time);
}

bool check_bootloader(void)
{
//	return true;
	
	uint8_t volatile * p_bootloader = (uint8_t volatile *)BOOTLOADER_ADDR;

	uint16_t i;
	
	uint8_t tmp;


	
	for (i = 0;i < 200;i++)
	{
		tmp = *p_bootloader++;
		
		//Send_Data_To_iPhone(tmp);
		
		
		if (tmp != 0xff)
		{
			if (tmp != 0x00)
			{				
				Send_Data_To_iPhone(0xaa);
				Send_Data_To_iPhone(0xaa);				
				return true;	
			}
		}	
		
	}

	Send_Data_To_iPhone(0x55);
	Send_Data_To_iPhone(0x55);
	
	return false;

}

void reset_app_boost(void)
{
	static uint8_t time_cnt = 0;
	
	
	if (IO_KEY_HOUT)
	{
		time_cnt++;
		
		if (time_cnt > RESET_APP_BOOST_OVER_TIME)
		{
			time_cnt = 0;
			
			if ((upadate_temp_time_cnt != 0) || (upadate_temp_delay != 0))
			{
				send_status_to_app(STATUS_BOOST,SEND_DELAY_SHORT_TIME);
			}	
		}		
	}
	else
	{
		time_cnt = 0;
		
	}
}

//检查是否是默认值

uint8_t check_temp_default(uint8_t *p_set_val,uint8_t const *p_default_val)
{
	uint8_t i;
	
	for (i = 1;i <= 8;i++)
	{
		if (p_set_val[i] != p_default_val[i])
		{
			return WORK_ON_CUSTOM;			
		}		
	}	
	
	return WORK_ON_NORMAL;	
	
}



/** 
 * @}
 */

 */ */