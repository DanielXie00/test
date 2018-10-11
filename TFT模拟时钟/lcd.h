#ifndef __LCD_H
#define __LCD_H		
//#include "sys.h"	 
#include "stdlib.h"
#include "stdint.h" 

//#include "TFTforgpio.h"
//#include "chip.h"

//////////////////////////////////////////////////////////////////////////////////	
/****************************************************************************************************
//=======================================Һ���������߽���==========================================//
//CS		��PB11	//Ƭѡ�ź�
//CLK	    ��PB13	//SPIʱ���ź�
//SDI(DIN)	��PB15	//SPI���������ź�
//=======================================Һ���������߽���==========================================//
//LED	    ��PB9	//��������źţ��ߵ�ƽ����
//RS(D/C)   ��PB10	//�Ĵ���/����ѡ���ź�(RS=0�������߷��͵���ָ�RS=1�������߷��͵�����������)
//RST	    ��PB12	//Һ������λ�źţ��͵�ƽ��λ
/////////////////////////////////////////////////////////////////////////////////////////////////
//==================================��ξ���ֻ��Ҫ3��IO=======================================//
//1.CS�źſ��Ծ��򣬲���SPI����Ƭѡ�ɽ�CS�ӵس��ͣ���ʡ1��IO
//2.LED��������źſ��ԽӸߵ�ƽ3.3V���ⳣ������ʡ1��IO
//3.RST��λ�źſ��Խӵ���Ƭ���ĸ�λ�ˣ�����ϵͳ�ϵ縴λ����ʡ1��IO
//==================================����л���������ʾ=======================================//
//��lcd.hͷ�ļ����޸ĺ�#define USE_HORIZONTAL ֵΪ0ʹ������ģʽ.1,ʹ�ú���ģʽ
//===========================����л�ģ��SPI����������Ӳ��SPI��������=========================//
//��lcd.hͷ�ļ����޸ĺ�#define USE_HARDWARE_SPI  ֵΪ0ʹ��ģ��SPI����.1,ʹ��Ӳ��SPI����
**************************************************************************************************/	

//LCD��Ҫ������


//SCL P1_3  GPIO 0[10] 	FUNC0
//SDA PD_4	GPIO 6[18]	FUNC4
//��������������е�LCP4357����ԭ��ͼ
//FUNC��ȷ��Ҫ��Datesheet�ĵ���������ϸ��ѯ
/************************************************************************************************/
//CS		��SGPIO3	PF_9/GPIO7[23]/U0_DIR/CTOUT_1/SGPIO3/ADC1_2//Ƭѡ�ź�
//CLK	    ��SGPIO4	P6_3/GPIO3[2]/T2_CAP2/EMC_CS1/USB0_PPWR/SGPIO4//SPIʱ���ź�
//SDI(DIN)	��SGPIO0	P9_0/GPIO4[12]/SGPIO0/SSP0_SSEL/MCABORT/ENET_CRS//SPI���������ź�
//=======================================Һ���������߽���==========================================//
//LED	    ��5V	//��������źţ��ߵ�ƽ����
//RS(D/C)   ��SGPIO6	PF_7/GPIO7[21]/U3_BAUD/SSP1_MOSI/TRACEDATA[2]/SGPIO6/I2S1_TX_WS/ADC1_7//�Ĵ���/����ѡ���ź�(RS=0�������߷��͵���ָ�RS=1�������߷��͵�����������)
//RST	    ��SGPIO7	PF_8/GPIO7[22]/U0_UCLK/CTIN_2/TRACEDATA[3]/SGPIO7/ADC0_2//Һ������λ�źţ��͵�ƽ��λ



//*************************************************************************************************/





#include "driver/gpio.h"
#include "wpa/common.h"
typedef  uint16_t u16;
typedef  uint32_t u32;


typedef struct  
{										    
	u16 width;			//LCD ���
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������	
	u16	 wramcmd;		//��ʼдgramָ��
	u16  setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ��	 
}_lcd_dev; 	

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
/////////////////////////////////////�û�������///////////////////////////////////	 
//֧�ֺ��������ٶ����л���֧��8/16λģʽ�л�
#define USE_HORIZONTAL  	0	//�����Ƿ�ʹ�ú��� 		0,��ʹ��.1,ʹ��.
//ʹ��Ӳ��SPI ����ģ��SPI��Ϊ����
#define USE_HARDWARE_SPI    0  //1:Enable Hardware SPI;0:USE Soft SPI
//////////////////////////////////////////////////////////////////////////////////	  
//����LCD�ĳߴ�
#if USE_HORIZONTAL==1	//ʹ�ú���
#define LCD_W 128
#define LCD_H 128
#else
#define LCD_W 128
#define LCD_H 128
#endif

//TFTLCD������Ҫ���õĺ���		   
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

////////////////////////////////////////////////////////////////////
//-----------------LCD�˿ڶ���---------------- 
//QDtechȫϵ��ģ������������ܿ��Ʊ��������û�Ҳ���Խ�PWM���ڱ�������
//#define LCD_LED        	GPIO_Pin_9  //PB9 ������TFT -LED
////�ӿڶ�����Lcd_Driver.h�ڶ��壬����ݽ����޸Ĳ��޸���ӦIO��ʼ��LCD_GPIO_Init()

							    
////////////////////////////////////////////////////////////////////////




/************************************************************************/
//��LCD�ӿڿ��õ���Ļ��ʾ

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL  5

#define GPIO_OUTPUT_PIN_ALL   ((1<<PIN_NUM_MOSI)|(1<<PIN_NUM_CLK)|(1<<PIN_NUM_CS)|(1<<PIN_NUM_DC)|(1<<PIN_NUM_RST)|(1<<PIN_NUM_BCKL) )



#define	LCD_CS_SET  		 gpio_set_level(PIN_NUM_CS,1);
#define	LCD_RS_SET  		 gpio_set_level(PIN_NUM_DC,1);
#define	LCD_SDA_SET  		 gpio_set_level(PIN_NUM_MOSI,1);
#define	LCD_SCL_SET  		 gpio_set_level(PIN_NUM_CLK,1);
#define	LCD_RST_SET  		 gpio_set_level(PIN_NUM_RST,1);
#define	LCD_LED_SET  		 gpio_set_level(PIN_NUM_BCKL,1);

//#define LED1_LIGHT		 gpio_set_level(PIN_NUM_CS,1);
//#define LED2_LIGHT		 gpio_set_level(PIN_NUM_CS,1);
//Һ�����ƿ���0�������궨��
#define	LCD_CS_CLR  	gpio_set_level(PIN_NUM_CS,0);
#define	LCD_RS_CLR  	gpio_set_level(PIN_NUM_DC,0);
#define	LCD_SDA_CLR  	gpio_set_level(PIN_NUM_MOSI,0);
#define	LCD_SCL_CLR  	gpio_set_level(PIN_NUM_CLK,0);
#define	LCD_RST_CLR  	gpio_set_level(PIN_NUM_RST,0);
#define	LCD_LED_CLR 	gpio_set_level(PIN_NUM_BCKL,0);













//#define	LCD_LED_CLR  	GPIO_ClearValue(SCL_GPIO_PORT,(1<<SCL_GPIO_BIT));
//#define LED1_OFF		GPIO_ClearValue(LED1_GPIO_PORT,(1<<LED1_GPIO_BIT));
//#define LED2_OFF		GPIO_ClearValue(LED2_GPIO_PORT,(1<<LED2_GPIO_BIT));

//������ɫ
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 //��ɫ
#define BRRED 			0XFC07 //�غ�ɫ
#define GRAY  			0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	0X841F //ǳ��ɫ
//#define LIGHTGRAY     0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 		0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE      	0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE          0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
	    															  
extern u16 BACK_COLOR, POINT_COLOR ;  

void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(u16 Color);	 
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);//����
u16  LCD_ReadPoint(u16 x,u16 y); //����
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
void LCD_DrawPoint_16Bit(u16 color);
u16 LCD_RD_DATA(void);//��ȡLCD����									    
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_DATA(u8 data);
void LCD_WR_DATA_16Bit(u16 data);
u16 LCD_ReadReg(u8 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
u16 LCD_ReadRAM(void);		   
u16 LCD_BGR2RGB(u16 c);
void LCD_SetParam(void);

void show_exa(void );
void MYShowScreen( uint16_t screen);

#endif  
	 
	 



