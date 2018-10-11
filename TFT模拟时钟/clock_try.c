/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
//#include "esp_wifi.h"
//#include "esp_event_loop.h"
#include "esp_log.h"
//#include "esp_attr.h"
//#include "esp_deep_sleep.h"



#include "apps/sntp/sntp.h"

#include "clock_try.h"
#include "math.h"
#include "lcd.h"
#include "GUI.h"


//const int CONNECTED_BIT = BIT0;

//static const char *TAG = "example";

uint32_t 			    datetimecounter;             //�ܷ�����
//rtc��ʱ��id����

//APP_TIMER_DEF(m_rtc_timer_id);

//�·����ݱ�
const uint8_t  table_week[12] = {0,  3,  3,  6,  1,  4,  6,  2,  5,  0,  3,  5}; //���������ݱ�

const uint8_t  mon_table[12] =  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};//ƽ����·����ڱ�



RTC_DATA_ATTR static int boot_count = 0;

static void obtain_time(void);


void Set_time_now(void)//
{
    ++boot_count;
    if(boot_count>20)boot_count =0;
    //ESP_LOGI(TAG, "Boot count: %d", boot_count);
    printf("Boot count: %d", boot_count);
    time_t now= datetimecounter;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        //ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
    	printf("Time is not set yet. \n");
        obtain_time();
        // update 'now' variable with current time
        time(&now);
    }
    setenv("TZ", "CST-8", 1);
    tzset();
	}
void Get_time_now(void)
{


    time_t now;

    struct tm timeinfo;
    time(&now);
    now+=datetimecounter;
    localtime_r(&now, &timeinfo);

    char strftime_buf[64];

//    // Set timezone to Eastern Standard Time and print local time
//    setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
//    tzset();
//    localtime_r(&now, &timeinfo);
//    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
//    //ESP_LOGI(TAG, "The current date/time in New York is: %s", strftime_buf);
//    printf("The current date/time in New York is: %s \n", strftime_buf);
//    // Set timezone to China Standard Time
//    setenv("TZ", "CST-8", 1);
//    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    printf("The current date/time in China Beijing is: %s \n", strftime_buf);
    //ESP_LOGI(TAG, "The current date/time in China Beijing is: %s \n", strftime_buf);
}


static void obtain_time(void)
{
    // wait for time to be set
    time_t now = datetimecounter;
    struct tm timeinfo = { 0};

    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
    	printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}




//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
uint8_t Is_Leap_Year(uint16_t year)
{
    if(year % 4 == 0) //�����ܱ�4����
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)//�����00��β,��Ҫ�ܱ�400����
                return 1;
            else
                return 0;
        }
        else
            return 1;
    }
    else
        return 0;
}
//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.

uint8_t rtc_date_set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint16_t t;
    if(syear < 1970 || syear > 2099)return 1; //||syear>2099
    for(t = 1970; t < syear; t++)	//��������ݵ��������
    {
        if(Is_Leap_Year(t))datetimecounter += 31622400; //�����������31622400
        else datetimecounter += 31536000;			 //ƽ���������
    }
    smon -= 1;
    for(t = 0; t < smon; t++)	 //��ǰ���·ݵ����������
    {
        datetimecounter += (uint32_t)mon_table[t] * 86400; //�·����������
        if(Is_Leap_Year(syear) && t == 1)datetimecounter += 86400; //����2�·�����һ���������
    }
    datetimecounter += (uint32_t)(sday - 1) * 86400; //��ǰ�����ڵ����������
    datetimecounter += (uint32_t)hour * 3600; //Сʱ������
    datetimecounter += (uint32_t)min * 60;	 //����������
    datetimecounter += sec; //�������Ӽ���ȥ

    return 0;
}
uint8_t rtc_utc_set(uint32_t date)
{
    datetimecounter = date;
    return 0;
}
uint8_t rtc_utc_get(uint32_t *date)
{
    *date = datetimecounter;
    return 0;
}
//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
uint8_t rtc_date_get(datetime *timenow) //ʱ��ṹ��
{
     uint32_t temp = 0;
    uint16_t temp1 = 0;
    temp = datetimecounter / 86400; //�õ�����(��������Ӧ��)
     {
         temp1 = 1970;	//��1970�꿪ʼ
        while(temp >= 365)
        {
            if(Is_Leap_Year(temp1))//������
            {
                if(temp >= 366)
                    temp -= 366; //�����������
                else
                {
                    temp1++;       //��һ��
                    break;
                }
            }
            else
                temp -= 365;	 //ƽ��
            temp1++;          //��һ��
        }
        timenow->year = temp1; //�õ����
        temp1 = 0;
        while(temp >= 28) //������һ����
        {
            if(Is_Leap_Year(timenow->year) && temp1 == 1) //�����ǲ�������/2�·�
            {
                if(temp >= 29)temp -= 29; //�����������
                else break;
            }
            else
            {
                if(temp >= mon_table[temp1])temp -= mon_table[temp1]; //ƽ��
                else break;
            }
            temp1++;
        }
        timenow->month = temp1 + 1;	//�õ��·�
        timenow->day = temp + 1;  	//�õ�����
    }
    temp = datetimecounter % 86400;     		//�õ�������
    timenow->h = temp / 3600;     	//Сʱ
    timenow->m = (temp % 3600) / 60; 	//����
    timenow->s = (temp % 3600) % 60; 	//����
    timenow->week = rtc_get_week(timenow->year, timenow->month, timenow->day); //��ȡ����
    return 0;
}
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//�������������������
//����ֵ�����ں�		0~6
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;
    // ���Ϊ21����,�������100
    if (yearH > 19)yearL += 100;
    // ����������ֻ��1900��֮���
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)temp2--;
    return(temp2 % 7);
}

//#define ONE_SECOND_INTERVAL       APP_TIMER_TICKS(1000, 0)

/***************************************************************************
* ����������ʱ���ж�
* ���������
*****************************************************************************/
void calender_timeout_handler(void *p_context)
{
     datetimecounter++;    //��Ϊ��λ
}

void rtc_create_time(void)  //����ʱ�亯��
{
//    uint32_t err_code;
//    err_code = app_timer_create(&m_rtc_timer_id, APP_TIMER_MODE_REPEATED, calender_timeout_handler);//����rtc
//    APP_ERROR_CHECK(err_code);
}
void rtc_time_start(void) //����rtcʱ��
{
//    uint32_t err_code;
//    err_code = app_timer_start(m_rtc_timer_id, ONE_SECOND_INTERVAL, NULL);//��ʼrtc
//    APP_ERROR_CHECK(err_code);
}
void rtc_time_stop(void) //�ر�rtcʱ��
{
//    uint32_t err_code;
//    err_code = app_timer_stop(m_rtc_timer_id);//�ر�alarm
//    APP_ERROR_CHECK(err_code);
}


/**************************************************
**������timerclock_init
**���ܣ����ϵ�ʱ�ӳ�ʼ������

**������
**��ע����ѧ��д

**���ߣ�ִ��ִս
**ʱ�䣺2017/2/12	11:21
*************************************************/
void  timersclock_init()
{

}
/**************************************************
**������showtime
**���ܣ����ϵ�ʱ����ʾ����

**���������ˢ�������Ķ�ʱ����ͻ��ֻ��ˢ������5֡��2֡����ÿ��ʱ����ʵ������������ʱ�䲻׼�������룬�о��Ƕ�ʱ����ͻ
**��ע����ѧ��д

**���ߣ�ִ��ִս
**ʱ�䣺2017/2/12	11:21
*************************************************/

void showtime(u8 x,u8 y)
{
			datetime  timenow;
			char year[4],month[2],day[2],hour[2],min[2],sec[2];
			rtc_date_get(&timenow);
			year[0]=(char)(timenow.year/1000+0x030);		//����0x30ת��ΪASCII��
			year[1]=(char)((timenow.year%1000)/100+0x30);
			year[2]=(char)(((timenow.year%1000)%100)/10+0x030);
			year[3]=(char)(((timenow.year%1000)%100)%10+0x030);

			month[0]=(char)((timenow.month/10)+0x030);
			month[1]=(char)((timenow.month%10)+0x030);

			day[0]=(char)((timenow.day/10)+0x030);
			day[1]=(char)((timenow.day%10)+0x030);

			hour[0]=(char)((timenow.h/10)+0x030);
			hour[1]=(char)((timenow.h%10)+0x030);

			min[0]=(char)((timenow.m/10)+0x030);
			min[1]=(char)((timenow.m%10)+0x030);

			sec[0]=(char)((timenow.s/10)+0x030);
			sec[1]=(char)((timenow.s%10)+0x030);

//			GUI_PutString(x,y,year);
//			GUI_PutString(x+6*4,y,"/");
//			GUI_PutString(x+6*5,y,month);
//			GUI_PutString(x+6*7,y,"/");
//			GUI_PutString(x+6*8,y,day);
//
//
//			GUI_PutString(x+6*11,y,hour);
//			GUI_PutString(x+6*13,y,":");
//			GUI_PutString(x+6*14,y,min);
//			if(timenow.s%2==0){
//			GUI_PutString(x+6*16,y,":");}
//			else
//			{	GUI_PutString(x+6*16,y," ");}
//			GUI_PutString(x+6*17,y,sec);


}


/**************************************************
**������IM_clock
**���ܣ�ģ��ʱ�ӣ�imulation clock��

**������	xo,yo :Ҫ����ģ��ʱ�ӵ�ԭ��	r:Ҫ����ģ��ʱ�ӵİ뾶
**��ע����ѧ��д

**���ߣ�ִ��ִս
**ʱ�䣺2017/2/12	11:21
*************************************************/

void IM_clock(u8 xo,u8 yo,u8 r)
{

	gui_circle(xo,yo,BLACK,r,0);//??
	gui_circle(xo,yo-r+2,BLACK,r/16,1);// ????
	gui_circle(xo+r-2,yo,BLACK,r/16,1);
	gui_circle(xo,yo+r-2,BLACK,r/16,1);
	gui_circle(xo-r+2,yo,BLACK,r/16,1);

	//IMclock_move(xo,yo,r);
}

/**************************************************
**������IM_clock
**���ܣ�ģ��ʱ�ӣ�imulation clock��

**������xo,yo :Ҫ����ģ��ʱ�ӵ�ԭ��	r:Ҫ����ģ��ʱ�ӵİ뾶
					����r�Ĵ�С�Ϳ��Ըı�ģ��ʱ�ӵĴ�С��λ��Ҳ���Ա䶯

**��ע����ѧ��д,��д��֮ǰ����ͨ���Ŀ���
				�ܺã������ã�֮ǰ����д�ķ����������ǻ����룬������cpu�ٶȲ�����ԭ��

**���ߣ�ִ��ִս
**ʱ�䣺2017/2/12	11:21       14:00�޸�
*************************************************/
void IMclock_move(u8 xo,u8 yo,u8 r)
{
//		datetime  timenow;
//		rtc_date_get(&timenow);	//��ȡʱ��
    time_t now;

    struct tm timeinfo;
    u16 bckcolo;
    u32 color=0xE0F0F8;
    bckcolo=((((color) >> 19) & 0x1f) << 11) \
            |((((color) >> 10) & 0x3f) << 5) \
            |(((color) >> 3) & 0x1f);
    time(&now);
    now+=datetimecounter;
    localtime_r(&now, &timeinfo);

	if(timeinfo.tm_sec==0)
		{DrawHand(xo,yo,r-r/6,59,6,bckcolo);} 		//������ȥ����֮ǰ�ĵ�һ�����룬��59��������㣬��������������ʱ��ģ�ԭ����ͬ
	else
		{DrawHand(xo,yo,r-r/6,timeinfo.tm_sec-1,6,bckcolo);}		//����ǰһ������


	if(timeinfo.tm_min==0)
		{DrawHand(xo,yo,r-r/4,59,6,bckcolo);}
	else
		{DrawHand(xo,yo,r-r/4,timeinfo.tm_min-1,6,bckcolo);}


	if(timeinfo.tm_hour==0)
		{DrawHand(xo,yo,r/2,11,30,bckcolo);}
	else
		{DrawHand(xo,yo,r/2,(timeinfo.tm_hour)%12-1,30,bckcolo);}



	DrawHand(xo,yo,r-r/6,timeinfo.tm_sec,6,BLACK);	 //�����
	DrawHand(xo,yo,r-r/4,timeinfo.tm_min,6,BLUE);
	DrawHand(xo,yo,r/2,(timeinfo.tm_hour%12),30,RED);






}
/**************************************************
**������IM_clock
**���ܣ�����ָ��λ�ò���ָ��

**������
**��ע����ѧ��д

**���ߣ�ִ��ִս
**ʱ�䣺2017/2/12	11:21
*************************************************/
void DrawHand(uchar x0,uchar y0,uchar  R,uchar  Time,uchar step,u16 color)
{	   double pi=3.14;
        uchar x,y;
        x=R*cos(Time*step*pi/180-pi/2)+x0;
        y=R*sin(Time*step*pi/180-pi/2)+y0;

        //LCD_DrawLine1((u16)x0,(u16)y0,(u16)x,(u16)y,(u16)color);
        GUI_LineWith(x0,y0,x,y,3,color);
}

