#include <string.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"

#include "sdkconfig.h" // generated by "make menuconfig"

#include "SDP31.h"
#include "oled1306.h"
#include "ssd1366.h"
#include "PensaGpio.h"
#include "uart_echo.h"
#include "sf05.h"
#include "time_ctrl.h"
#include "AD_for_temperature_power.h"
#include "sf05.h"
#include "SDP31.h"



float Pensa_Tem=0;
float Pensa_Flow=0;
float Pensa_pressure;

float SFM3000_Total_Flow=0;
float Target_Flow=0;
float SFM3000_Flow_value=0;
float SFM3000_Temperature_value=0;
uint32_t PWM_detection;
uint16_t stop_point=1000;
uint16_t FlowResetTime=2;
float    start_point=0.5;
// stop point

#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */
uint8_t readData[9]={0,0,0,0,0,0,0,0,0};
#define SDA_SDP31_PIN GPIO_NUM_18
#define SCL_SDP31_PIN GPIO_NUM_5
	

/*
#define SDP_MEASUREMENT_COMMAND_0    0x3f
#define SDP_MEASUREMENT_COMMAND_1    0xf9 //stop*/

#define SDP_MEASUREMENT_COMMAND_0    0x36
#define SDP_MEASUREMENT_COMMAND_1    0x2f
#define tag "SDP31"
#define BIU16(data, start) (((uint16_t)(data)[start]) << 8 | ((data)[start + 1]))

#define POLYNOMIAL 0x31

static uint8_t SDP3X_CalcCrc(uint8_t data[], uint8_t nbrOfBytes)
{
        uint8_t bit; // bit mask
        uint8_t crc = 0xFF; // calculated checksum
        uint8_t byteCtr; // byte counter
        // calculates 8-Bit checksum with given polynomial
        for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
        {
                 crc ^= (data[byteCtr]);
                for(bit = 8; bit > 0; --bit)
                {
                        if(crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL; // 保证高位为 1 对齐
                        else crc = (crc << 1);
                }
        }
        return crc;
}

//float Pensa_pressure;
void i2c_SDP31_init()
{
	i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = SDA_SDP31_PIN,
		.scl_io_num = SCL_SDP31_PIN,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 100000
	};
	i2c_param_config(I2C_NUM_0, &i2c_config);
	i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

}

/**
 * @brief test code to write esp-i2c-slave
 *
 * 1. set mode
 *  _______________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 0 byte + ack  | write 1 byte + ack  | stop |
 * --------|---------------------------|---------------------|----------------------------|
 * 2. wait more than 45 ms
 * 3. read data
 * _______________________________________________________________________________________________________________________________
 * | start | slave_addr + rd_bit + ack | read 1 byte + ack  | read 1 byte + nack |read 1 byte + ack  | read 1 byte + nack | stop |
 * --------|---------------------------|--------------------|--------------------|-------------------|--------------------|------|
 */

uint8_t *readSensor() {


	esp_err_t espRc;
    //   uint8_t txData[2] = {SDP_MEASUREMENT_COMMAND_0, SDP_MEASUREMENT_COMMAND_1};
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	espRc =i2c_master_write_byte(cmd, (SDP31_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);//写地址


	i2c_master_write_byte(cmd, SDP_MEASUREMENT_COMMAND_0, true);//写命令
	i2c_master_write_byte(cmd, SDP_MEASUREMENT_COMMAND_1, true);

  	i2c_master_stop(cmd);
   	espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_RATE_MS);
   	i2c_cmd_link_delete(cmd);



    	vTaskDelay(50 / portTICK_RATE_MS);

    	cmd = i2c_cmd_link_create();
     	i2c_master_start(cmd);

     	espRc = i2c_master_write_byte(cmd, (SDP31_I2C_ADDRESS << 1) |I2C_MASTER_READ, true);//写地址

	espRc=i2c_master_read(cmd, readData, (uint8_t)9, ACK_VAL);//读数据

	i2c_master_stop(cmd);
	espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	/*printf("readData[0]: %d\n", readData[0]);
	printf("readData[1]: %d\n", readData[1]);
	printf("readData[2]: %d\n", readData[2]);
	printf("readData[3]: %d\n", readData[3]);
	printf("readData[4]: %d\n", readData[4]);
	printf("readData[5]: %d\n", readData[5]);
	printf("readData[6]: %d\n", readData[6]);
	printf("readData[7]: %d\n", readData[7]);
	printf("readData[8]: %d\n", readData[8]);*/
	return readData; 
}

/**********************************************************
 * getPressureDiff
 *  Gets the current Pressure Differential from the sensor.
 *
 * @return float - The Pressure in Pascal
 **********************************************************/
  #define SCALE_TEMP    200.0   // scale factor temperature
float getPressureDiff(void)
{
  int16_t dp_ticks;float dp_scale;
  uint8_t temp_CRC[2]={0};
  // merge chars to one int
  dp_ticks = BIU16(readSensor(), 0);
  Pensa_Tem=(readData[3]<<8 | readData[4])/SCALE_TEMP;
  temp_CRC[0]=readData[0];  temp_CRC[1]=readData[1];
  if(readData[2]==SDP3X_CalcCrc(temp_CRC,2))   dp_ticks =(readData[0]<<8 | readData[1]);
  else     return 0;  
  temp_CRC[0]=readData[6];  temp_CRC[1]=readData[7];
  if(readData[8]==SDP3X_CalcCrc(temp_CRC,2))  dp_scale=(readData[6]<<8 | readData[7]);
  else     return 0; 
  // adapt scale factor according to datasheet (here for version SDP31)
 // float dp_scale = 60.0;//dp_scale 是比例因子
  return dp_ticks/dp_scale;

}
/**********************************************************
 * getTemparature
 *  Gets the current Temparature from the sensor.
 *
 * @return float - The Temparature
 **********************************************************/
/*
float getTemperature(void)
{
  int16_t  temperature_ticks;
  // merge chars to one int
  temperature_ticks = BIU16(readSensor(), 3);
 // float t_scale = 200.0;//t_scale 是比例因子
  return temperature_ticks/SCALE_TEMP;
}*/

float flowcalculate(float Pensa_Flow)
{
	if(Pensa_Flow==0) return 0;
	else if(Pensa_Flow<=0.15) return 1.1;
	else if(Pensa_Flow<=0.32) return 2.1;
	else if(Pensa_Flow<=0.54) return 3.2;
	else if(Pensa_Flow<=0.79) return 4.3;
	else if(Pensa_Flow<=1.07) return 5.4;
	else if(Pensa_Flow<=1.39) return 6.4;
	else if(Pensa_Flow<=1.75) return 7.5;
	else if(Pensa_Flow<=2.13) return 8.6;
	else if(Pensa_Flow<=2.52) return 9.7;
	else if(Pensa_Flow<=2.93) return 10.7;
	else if(Pensa_Flow<=4.08) return 12.9;
	else if(Pensa_Flow<=5.32) return 15.0;
        else return 17.2;

}
float getpressure(void)
{
  float pressure2;
 // i2c_SDP31_init();
  pressure2=getPressureDiff();//获取气压值
  return  pressure2;
}

bool LED_status=false;
char NTC_AD_buff[20];//用于打印数据
char NTC_temp_buff[20];//用于打印数据
char Temperature_buff[20];//用于打印数据
char Temperature_AD_buff[20];//用于打印数据
char Pressure_buff[20];//用于打印数据
char Total_Flow_buff[20];//用于打印数据
char Target_Flow_buff[20];//用于打印数据
char SFM3000_Flow_buff[20];//用于打印数据
char SFM3000_Temperature_buff[20];//用于打印数据
uint32_t Temperature_ad;
uint32_t delay_cnt=0;//
uint32_t clear_cnt=0;//

uint32_t PWM_cnt=0; 
uint32_t PWM_1=0; 
uint32_t NTC_AD=0; 
uint32_t NTC_temperature=0; 

#define PWM_cycle 50
void pwm_ctrl_task()
{
  
    ADC_init();

    while (1) {
                NTC_AD=Get_NTC();
                NTC_temperature=NTC_Temperature(NTC_AD);
    	         PWM_cnt++;
                PWM_detection=pwm_change_test(); 	 
                PWM_1=(uint32_t)((PWM_cycle*PWM_detection) / 4096);
                if(PWM_cnt==PWM_1){ Heater_Ctrl(Heater_enable);vTaskDelay(2/ portTICK_PERIOD_MS);Temperature_ad=Temperature_test(); }       
                if(PWM_cnt>=PWM_cycle){PWM_cnt=0;Heater_Ctrl(Heater_disable); }
                vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
}
void pwm_ctrl_APP(void)
{
  xTaskCreate(pwm_ctrl_task, "pwm_ctrl_task", 1024*2, NULL, 6, NULL);
}


void task_SDP31_contrast(void *pvParameters) {

     while(1)
	{
	        //Pensa_Tem=getTemperature();//获取温度值

	       Pensa_pressure=getPressureDiff();//获取气压值
	       // Pensa_Flow=flowcalculate(Pensa_pressure);//查表得出气流值

	 

                SFM3000_Flow_value= SF05_GetFlow();
                SFM3000_Temperature_value=SF05_GetTemperature();
	       // if(Pensa_pressure>=0.01){Heater_Ctrl(Heater_enable);}
	        //else if(Pensa_pressure<=(-0.01)){}
	        
	        if(SFM3000_Flow_value>=start_point)//如果大于一个值，IO变高，否则变低
	        {	     
			delay_cnt=0;     
			clear_cnt=0;      
			SFM3000_Total_Flow=SFM3000_Total_Flow+SFM3000_Flow_value;
			gpio_set_level(GPIO_TEST2,1);
			Red_LED(LED_enable);
		        if(Target_Flow*2<stop_point)Target_Flow=Target_Flow+SFM3000_Flow_value;
		        else //大于设定值变低
		        {
		                Target_Flow=stop_point/2;
		                Red_LED(LED_disable);
						gpio_set_level(GPIO_TEST1,1);
						gpio_set_level(GPIO_TEST2,0);		
		        };	        	        
	        }
	        else //否则变低
	        {
				gpio_set_level(GPIO_TEST2,0); 
				Red_LED(LED_disable);
				//vTaskDelay(80 / portTICK_RATE_MS);
				if(++clear_cnt>100){clear_cnt=0;  Target_Flow=0; SFM3000_Total_Flow=0;  	        
				        Red_LED(LED_disable);
	                                gpio_set_level(GPIO_TEST1,0);
	                                gpio_set_level(GPIO_TEST2,0);
	            }
				
	        
	        }
	      //  else if(SFM3000_Flow_value<=(-0.01)){  }	        
	       // else Heater_Ctrl(Heater_disable);
		if(Target_Flow>=(stop_point/2))
		{

			if(++delay_cnt>=(4*FlowResetTime)){delay_cnt=0; Target_Flow=0; SFM3000_Total_Flow=0; }
			vTaskDelay(60 / portTICK_RATE_MS);	
					
		}
	       if(gpio_get_level(GPIO_INPUT_IO_0)){
	       }
	       else  {
	        Red_LED(LED_disable);
	        gpio_set_level(GPIO_TEST1,0);
	        gpio_set_level(GPIO_TEST2,0);	       
	        SFM3000_Total_Flow=0; Target_Flow=0;printf("GPIO_INPUT_IO_0 = 000 \n");
	        oled_display(OLED_2_ADDRESS,6,"Target:         ");  //OLED显示	       
	        oled_display(OLED_2_ADDRESS,7,"Sum:            ");  //OLED显示	
	         vTaskDelay(200 / portTICK_RATE_MS);	       
	       }
	       
	        
	      //if(LED_status==false) {gpio_set_level(GPIO_Green_LED,0);LED_status=true;}
	     // else {gpio_set_level(GPIO_Green_LED,1);LED_status=false;}
	       
/*
	        printf("SFM3000_Flow_value:%05.2f \n", SFM3000_Flow_value);
	        printf("getPressureDiff: %03.2f\n", Pensa_pressure);
	        printf("Tempera: %03.2f\n", Pensa_Tem);*/

	        //uart_my_printf(Total_Flow_buff,strlen(Total_Flow_buff));
	        
                vTaskDelay(1 / portTICK_RATE_MS);
	}
}


void display_task(void)
{
        char PWM_detection_buff[16]={0};
        while(1)
        {
        
        
                sprintf(NTC_AD_buff,"NTC_AD:%d  ",NTC_AD);      //OLED显示   
                sprintf(NTC_temp_buff,"NTC_tem:%d  ",NTC_temperature);      //OLED显示        
                sprintf(PWM_detection_buff,"PWM:%d  ",PWM_detection);      //OLED显示
                sprintf(Temperature_buff,"Tempera:%03.2f  ",Pensa_Tem);      //OLED显示
                sprintf(Temperature_AD_buff,"Tem_AD:%d  ",Temperature_ad);      //OLED显示
                sprintf(SFM3000_Flow_buff,"Flow:%05.2f ",SFM3000_Flow_value);
                sprintf(SFM3000_Temperature_buff,"Tempera:%05.2f",SFM3000_Temperature_value);          
                sprintf(Pressure_buff,"Pressure:%03.2f  ",Pensa_pressure); //OLED显示
                sprintf(Total_Flow_buff,"Total:%05.2f  ",2*SFM3000_Total_Flow); //OLED显示 
                sprintf(Target_Flow_buff,"Target:%05.2f",2*Target_Flow); //OLED显示                                
	        oled_display(OLED_1_ADDRESS,0,"    SmartFlow   ");  //OLED显示	
	        oled_display(OLED_1_ADDRESS,1,"                ");  //OLED显示
	        oled_display(OLED_1_ADDRESS,2,Temperature_buff);  //OLED显示	        
	        oled_display(OLED_1_ADDRESS,3,Pressure_buff);  //OLED显示
	        oled_display(OLED_1_ADDRESS,4,NTC_temp_buff);  //OLED显示	        
	        oled_display(OLED_1_ADDRESS,5,NTC_AD_buff);  //OLED显示
	        oled_display(OLED_1_ADDRESS,6,Temperature_AD_buff);  //OLED显示
	        oled_display(OLED_1_ADDRESS,7,PWM_detection_buff);  //OLED显示
	        oled_display(OLED_2_ADDRESS,0,"    SFM3000     ");  //OLED显示
	        oled_display(OLED_2_ADDRESS,1,"                ");  //OLED显示	

	        oled_display(OLED_2_ADDRESS,2,SFM3000_Temperature_buff);  //OLED显示	
	        oled_display(OLED_2_ADDRESS,3,"                ");  //OLED显示
	        oled_display(OLED_2_ADDRESS,4,SFM3000_Flow_buff);  //OLED显示	
	        oled_display(OLED_2_ADDRESS,5,"                ");  //OLED显示
	        oled_display(OLED_2_ADDRESS,6,Target_Flow_buff);  //OLED显示	
	        oled_display(OLED_2_ADDRESS,7,Total_Flow_buff);  //OLED显示
                vTaskDelay(500 / portTICK_RATE_MS);
        
        }

}
void SDP31_APP(void)
{
 pwm_ctrl_APP();
 xTaskCreate(&task_SDP31_contrast, "ssid1306_contrast", 1024*4, NULL, 6, NULL);

 xTaskCreate(&display_task, "display_task", 2048, NULL, 6, NULL);
}