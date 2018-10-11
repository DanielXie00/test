#ifndef __KEY_H
#define __KEY_H
#include "stm8l15x.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_rtc.h"
#include "clock.h"


#define  key_pin1 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)
#define  key_pin2 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)
#define  key_pin3 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)
#define  key_pin4 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)
//#define  key_pin5 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)

#define  extin_pin 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)	//外置电源插入
#define  chang_pin 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)	//充电ok检测
#define  power_pin	GPIO_SetBits(GPIOB, GPIO_Pin_1) 	//升压控制
#define  select_pin	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)

#define KEY_ON  0x01
#define KEY_SET 0x02
#define KEY_UP  0x03
#define KEY_DOW  0x04
#define VALUE_ON  0x05
#define VALUE_SET  0x06
#define VALUE_UP  0x07
#define VALUE_DOW  0x08
#define VALUE_SOUND  0x09
#define KEY_C_HCHO_VB  0x10

extern u8 hco2_level;
extern u8 dust_level;
extern u8 f_on_off;
extern u8 f_outsid_v;
//#define hco2_level 100
//#define dust_level 100

void buzzer_program(void);	//2ms
void key_scannal(void);        //2ms
void key_process(void);	//2ms
void key_scannal2(void);	//2ms



#endif