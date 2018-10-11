


#include "lcd.h"
#include "stdlib.h"

#include "LCD_GUI_SF.h"


//#include "TFTforgpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F103RBT6,��Ƶ72M  ��Ƭ��������ѹ3.3V
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtech.net
//�Ա���վ��http://qdtech.taobao.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:QDtech2008@gmail.com 
//Skype:QDtech2008
//��������QQȺ:324828016
//��������:2013/5/13
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2009-2019
//All rights reserved
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


/************************************************************************************************/
//CS		//Ƭѡ�ź�
//CLK	    //SPIʱ���ź�
//SDI(DIN)	//SPI���������ź�
//=======================================Һ���������߽���==========================================//
//LED	    ��5V	//��������źţ��ߵ�ƽ����
//RS(D/C)  //�Ĵ���/����ѡ���ź�(RS=0�������߷��͵���ָ�RS=1�������߷��͵�����������)
//RST	    //Һ������λ�źţ��͵�ƽ��λ

//*************************************************************************************************/



#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


#include <string.h>
#include <stdlib.h>

#include "freertos/queue.h"
#include "driver/gpio.h"
//#include "oled_driver.h"
#include "mydelay.h"

#include "lcd.h"


#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"








	   
//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

//������ɫ,������ɫ
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 
void ili_spi_pre_transfer_callback(spi_transaction_t *t) ;
esp_err_t ret;
   spi_device_handle_t TFTSPI;
   spi_bus_config_t buscfg={
       .miso_io_num=PIN_NUM_MISO,
       .mosi_io_num=PIN_NUM_MOSI,
       .sclk_io_num=PIN_NUM_CLK,
       .quadwp_io_num=-1,
       .quadhd_io_num=-1
   };
   spi_device_interface_config_t devcfg={
       .clock_speed_hz=22000000,               //Clock out at 10 MHz
       .mode=0,                                //SPI mode 0
       .spics_io_num=PIN_NUM_CS,               //CS pin
       .queue_size=16,                          //We want to be able to queue 7 transactions at a time
       .pre_cb=ili_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
   };



void delay_spi(uint32_t time)
{
	uint32_t n=time;
	while(n!=0)
		n--;
}


//Send a command to the ILI9341. Uses spi_device_transmit, which waits until the transfer is complete.
void ili_cmd(spi_device_handle_t spi, const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

//Send data to the ILI9341. Uses spi_device_transmit, which waits until the transfer is complete.
void ili_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_transmit(spi, &t);  //Transmit!
   assert(ret==ESP_OK);            //Should have had no issues.
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void ili_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}



void ili_init(spi_device_handle_t spi)
{
    int cmd=0;
    //Initialize non-SPI GPIOs
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_BCKL, 1);

   // gpio_set_level(PIN_NUM_BCKL, 0);
}

/****************************************************************************
* ��    �ƣ�SPI2_Init(void)
* ��    �ܣ�STM32_SPI2Ӳ�����ó�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����STM32_SPI2Ӳ�����ó�ʼ��      MD_BUK|MD_EHS|MD_EZI     SCU_MODE_REPEATER|SCU_PINIO_FAST  MD_PLN_FAST|MD_PUP
(MD_PUP | MD_EZI | MD_ZI)   MD_PUP|MD_ZI   MD_PUP|MD_ZI   SCU_PINIO_FAST
****************************************************************************/
void SPI2_Init(void)	
{
	

//	    //Initialize the SPI bus
//	    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
//	    assert(ret==ESP_OK);
//	    //Attach the LCD to the SPI bus
//	    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &TFTSPI);
//	    assert(ret==ESP_OK);
//	    //Initialize the LCD
//	    ili_init(TFTSPI);



	 //��ʼ��GPIO����
	  gpio_config_t io_conf;
	    //disable interrupt
	    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
	    //set as output mode
	    io_conf.mode = GPIO_MODE_OUTPUT;
	    //bit mask of the pins that you want to set,e.g.GPIO18/19
	    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_ALL;
	    //disable pull-down mode
	    io_conf.pull_down_en = 0;
	    //disable pull-up mode
	    io_conf.pull_up_en = 0;
	    //configure GPIO with the given settings
	    gpio_config(&io_conf);

//	    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
//	    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
//	    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

	    //Reset the display
	    gpio_set_level(PIN_NUM_RST, 0);
	    vTaskDelay(100 / portTICK_RATE_MS);
	    gpio_set_level(PIN_NUM_RST, 1);
	    vTaskDelay(100 / portTICK_RATE_MS);
	    gpio_set_level(PIN_NUM_BCKL, 1);

	}

/****************************************************************************
* ��    �ƣ�void  SPIv_WriteData(u8 Data)
* ��    �ܣ�STM32_ģ��SPIдһ���ֽ����ݵײ㺯��
* ��ڲ�����Data
* ���ڲ�������
* ˵    ����STM32_ģ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
void  SPIv_WriteData( u8 Data)
{


	//ili_data(TFTSPI,&Data,1);

	unsigned char i=0;
		for(i=8;i>0;i--)
		{
			if(Data&0x80)
			{ LCD_SDA_SET; //�������
			}
	      else
		  {LCD_SDA_CLR;}

	      LCD_SCL_CLR;

	      LCD_SCL_SET;

	      Data<<=1;
		}

}

//******************************************************************
//��������  LCD_WR_REG
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��Һ��������д��д16λָ��
//���������Reg:��д���ָ��ֵ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_WR_REG(u16 data)
{ 
  
	//ili_cmd(TFTSPI,data);
	   LCD_CS_CLR;

	   LCD_RS_CLR;

	   SPIv_WriteData(data);

	   LCD_CS_SET;

}

//******************************************************************
//��������  LCD_WR_DATA
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��Һ��������д��д8λ����
//���������Data:��д�������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_WR_DATA( u8 data)
{




   //SPIv_WriteData(data);

	   LCD_CS_CLR;

	   LCD_RS_SET;


	   SPIv_WriteData(data);

	   LCD_CS_SET;

}
//******************************************************************
//��������  LCD_DrawPoint_16Bit
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    8λ���������д��һ��16λ����
//���������(x,y):�������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_WR_DATA_16Bit(u16 data)
{	

//
//   SPIv_WriteData(data>>8);
////
//   SPIv_WriteData(data);


//       esp_err_t ret;
//       spi_transaction_t t;
////       u8 a,b;
////       a=data>>8;
////       b=(data<<8)|a;
//
//       memset(&t, 0, sizeof(t));       //Zero out the transaction
//       t.length=2*8;                 //Len is in bytes, transaction length is in bits.
//       t.tx_buffer=&data;               //Data
//       t.user=(void*)1;                //D/C needs to be set to 1
//       ret=spi_device_transmit(TFTSPI, &t);  //Transmit!
//    //  assert(ret==ESP_OK);            //Should have had no issues.

//	       u8 a,b;
//	       a=data>>8;
//	       b=(data<<8)|a;	//Ӳ��SPI��������ݶ˿��ܺ�ģ����෴


	  LCD_CS_CLR;

	   LCD_RS_SET;

	   SPIv_WriteData(data>>8);

	   SPIv_WriteData(data);

	   LCD_CS_SET;


}

//******************************************************************
//��������  LCD_WriteReg
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    д�Ĵ�������
//���������LCD_Reg:�Ĵ�����ַ
//			LCD_RegValue:Ҫд�������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);
//
	LCD_WR_DATA(LCD_RegValue);

}	   
	 
//******************************************************************
//��������  LCD_WriteRAM_Prepare
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ʼдGRAM
//			�ڸ�Һ��������RGB����ǰ��Ӧ�÷���дGRAMָ��
//�����������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

//******************************************************************
//��������  LCD_DrawPoint
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    ��ָ��λ��д��һ�����ص�����
//���������(x,y):�������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//���ù��λ�� 
	LCD_WR_DATA_16Bit(POINT_COLOR);
}

//******************************************************************
//��������  LCD_GPIOInit
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    Һ����IO��ʼ����Һ����ʼ��ǰҪ���ô˺���
//�����������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_GPIOInit(void)
{
    SPI2_Init ();
}

//******************************************************************
//��������  LCD_Reset
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    LCD��λ������Һ����ʼ��ǰҪ���ô˺���
//�����������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_RESET(void)
{
	//GPIO_ClearValue(RST_GPIO_PORT,(1<<RST_GPIO_BIT));
	LCD_RST_CLR;
	delay_ms(500);	
	//GPIO_SetValue(RST_GPIO_PORT,(1<<RST_GPIO_BIT));	 
	LCD_RST_SET;
	delay_ms(50);
}
 	 
//******************************************************************
//��������  LCD_Init
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    LCD��ʼ��
//�����������
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_Init(void)
{  

	LCD_GPIOInit();//ʹ��ģ��SPI
 										 

 	LCD_RESET(); //Һ������λ

	//************* Start Initial Sequence **********//		
	//��ʼ��ʼ��Һ����
	LCD_WR_REG(0x11);//Sleep exit 
	delay_ms (1000);		
	//ST7735R Frame Rate
	LCD_WR_REG(0xB1); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 

	LCD_WR_REG(0xB2); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 

	LCD_WR_REG(0xB3); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	
	LCD_WR_REG(0xB4); //Column inversion 
	LCD_WR_DATA(0x07); 
	
	//ST7735R Power Sequence
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA(0xA2); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x84); 
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA(0xC5); 

	LCD_WR_REG(0xC2); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x00); 

	LCD_WR_REG(0xC3); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_REG(0xC4); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0xEE); 
	
	LCD_WR_REG(0xC5); //VCOM 
	LCD_WR_DATA(0x0E); 
	
	LCD_WR_REG(0x36); //MX, MY, RGB mode 				 
	LCD_WR_DATA(0xC8); 
	
	//ST7735R Gamma Sequence
	LCD_WR_REG(0xe0); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1a); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x18); 
	LCD_WR_DATA(0x2f); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x20); 
	LCD_WR_DATA(0x22); 
	LCD_WR_DATA(0x1f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x23); 
	LCD_WR_DATA(0x37); 
	LCD_WR_DATA(0x00); 	
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x10); 

	LCD_WR_REG(0xe1); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x33); 
	LCD_WR_DATA(0x2c); 
	LCD_WR_DATA(0x29); 
	LCD_WR_DATA(0x2e); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x3f); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0x10);  
	
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x7f);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x9f);
	
	LCD_WR_REG(0xF0); //Enable test command  
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xF6); //Disable ram power save mode 
	LCD_WR_DATA(0x00); 
	
	LCD_WR_REG(0x3A); //65k mode 
	LCD_WR_DATA(0x05); 	
	LCD_WR_REG(0x29);//Display on	

	LCD_SetParam();//����LCD����	


//	LED2_LIGHT;



	//LCD_LED_SET;//��������	 
	//LCD_Clear(WHITE);
}
//******************************************************************
//��������  LCD_Clear
//���ߣ�    xiao��@ȫ������
//���ڣ�    2013-02-22
//���ܣ�    LCDȫ�������������
//���������Color:Ҫ���������ɫ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void LCD_Clear(u16 Color)
{
	u16 i,j;      
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
	for(i=0;i<lcddev.width;i++)
	{
		for(j=0;j<lcddev.height;j++)
		LCD_WR_DATA_16Bit(Color);	//д������
	}
}   		  
/*************************************************
��������LCD_SetWindows
���ܣ�����lcd��ʾ���ڣ��ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{
#if USE_HORIZONTAL==1	//ʹ�ú���
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar+3);
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd+3);

	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar+2);
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd+2);

#else
	
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA((0x00FF&xStar)+2);
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA((0x00FF&xEnd)+2);

	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA((0x00FF&yStar)+3);
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA((0x00FF&yEnd)+3);
#endif

	LCD_WriteRAM_Prepare();	//��ʼд��GRAM				
}   

/*************************************************
��������LCD_SetCursor
���ܣ����ù��λ��
��ڲ�����xy����
����ֵ����
*************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);
} 

//����LCD����
//������к�����ģʽ�л�
void LCD_SetParam(void)
{ 	
	lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//ʹ�ú���	  
	lcddev.dir=1;//����
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;			
	LCD_WriteReg(0x36,0xA8);

#else//����
	lcddev.dir=0;//����				 	 		
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;	
	LCD_WriteReg(0x36,0xC8);
#endif
}	

//To send a line we have to send a command, 2 data bytes, another command, 2 more data bytes and another command
//before sending the line data itself; a total of 6 transactions. (We can't put all of this in just one transaction
//because the D/C line needs to be toggled in the middle.)
//This routine queues these commands up so they get sent as quickly as possible.
static void send_line(spi_device_handle_t spi, int ypos, uint16_t *line)
{
    esp_err_t ret;
    int x;
    //Transaction descriptors. Declared static so they're not allocated on the stack; we need this memory even when this
    //function is finished because the SPI driver needs access to it even while we're already calculating the next line.
    static spi_transaction_t trans[6];

    //In theory, it's better to initialize trans and data only once and hang on to the initialized
    //variables. We allocate them on the stack, so we need to re-init them each call.
    for (x=0; x<6; x++) {
        memset(&trans[x], 0, sizeof(spi_transaction_t));
        if ((x&1)==0) {
            //Even transfers are commands
            trans[x].length=8;
            trans[x].user=(void*)0;
        } else {
            //Odd transfers are data
            trans[x].length=8*4;
            trans[x].user=(void*)1;
        }
        trans[x].flags=SPI_TRANS_USE_TXDATA;
    }
    trans[0].tx_data[0]=0x2A;           //Column Address Set
    trans[1].tx_data[0]=0;              //Start Col High
    trans[1].tx_data[1]=0;              //Start Col Low
    trans[1].tx_data[2]=(128)>>8;       //End Col High
    trans[1].tx_data[3]=(128)&0xff;     //End Col Low
    trans[2].tx_data[0]=0x2B;           //Page address set
    trans[3].tx_data[0]=ypos>>8;        //Start page high
    trans[3].tx_data[1]=ypos&0xff;      //start page low
    trans[3].tx_data[2]=(ypos+1)>>8;    //end page high
    trans[3].tx_data[3]=(ypos+1)&0xff;  //end page low
    trans[4].tx_data[0]=0x2C;           //memory write
    trans[5].tx_buffer=line;            //finally send the line data
    trans[5].length=128*2*8;            //Data length, in bits
    trans[5].flags=0; //undo SPI_TRANS_USE_TXDATA flag

    //Queue all transactions.
    for (x=0; x<6; x++) {
        ret=spi_device_queue_trans(spi, &trans[x], portMAX_DELAY);
        assert(ret==ESP_OK);
    }

    //When we are here, the SPI driver is busy (in the background) getting the transactions sent. That happens
    //mostly using DMA, so the CPU doesn't have much to do here. We're not going to wait for the transaction to
    //finish because we may as well spend the time calculating the next line. When that is done, we can call
    //send_line_finish, which will wait for the transfers to be done and check their status.
}


static void send_line_finish(spi_device_handle_t spi)
{
    spi_transaction_t *rtrans;
    esp_err_t ret;
    //Wait for all 6 transactions to be done and get back the results.
    for (int x=0; x<6; x++) {
        ret=spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
        assert(ret==ESP_OK);
        //We could inspect rtrans now if we received any info back. The LCD is treated as write-only, though.
    }
}

//Simple routine to generate some patterns and send them to the LCD. Don't expect anything too
//impressive. Because the SPI driver handles transactions in the background, we can calculate the next line
//while the previous one is being sent.
static void display_pretty_colors(spi_device_handle_t spi)
{
    uint16_t line[2][128];
    int x, y, frame=0;
    //Indexes of the line currently being sent to the LCD and the line we're calculating.
    int sending_line=-1;
    int calc_line=0;

    while(1) {
        frame++;
        for (y=0; y<128; y++) {
            //Calculate a line.
            for (x=0; x<128; x++) {
                line[calc_line][x]=((x<<3)^(y<<3)^(frame+x*y));
                LCD_WR_DATA_16Bit( line[calc_line][x]);
            }
            //Finish up the sending process of the previous line, if any
            if (sending_line!=-1) send_line_finish(spi);
            //Swap sending_line and calc_line
       // sending_line=calc_line;
        // calc_line=(calc_line==1)?0:1;
            //Send the line we currently calculated.
       // send_line(spi, y, line[sending_line]);
            //The line is queued up for sending now; the actual sending happens in the
            //background. We can go on to calculate the next line as long as we do not
            //touch line[sending_line]; the SPI sending process is still reading from that.
        }
    }
}
void show_exa(void ){
	display_pretty_colors(TFTSPI);
}

void MYShowScreen( uint16_t screen)
{

    esp_err_t ret;
    spi_transaction_t t;


		    memset(&t, 0, sizeof(t));       //Zero out the transaction
		    t.length=128*2*8;                 //Len is in bytes, transaction length is in bits.
		    t.tx_buffer=screen;               //Data
		    t.user=(void*)1;                //D/C needs to be set to 1
		    ret=spi_device_transmit(TFTSPI, &t);  //Transmit!


}
