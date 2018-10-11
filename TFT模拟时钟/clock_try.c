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

uint32_t 			    datetimecounter;             //总分钟数
//rtc计时器id号码

//APP_TIMER_DEF(m_rtc_timer_id);

//月份数据表
const uint8_t  table_week[12] = {0,  3,  3,  6,  1,  4,  6,  2,  5,  0,  3,  5}; //月修正数据表

const uint8_t  mon_table[12] =  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};//平年的月份日期表



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




//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
uint8_t Is_Leap_Year(uint16_t year)
{
    if(year % 4 == 0) //必须能被4整除
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)//如果以00结尾,还要能被400整除
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
//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.

uint8_t rtc_date_set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint16_t t;
    if(syear < 1970 || syear > 2099)return 1; //||syear>2099
    for(t = 1970; t < syear; t++)	//把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))datetimecounter += 31622400; //闰年的秒钟数31622400
        else datetimecounter += 31536000;			 //平年的秒钟数
    }
    smon -= 1;
    for(t = 0; t < smon; t++)	 //把前面月份的秒钟数相加
    {
        datetimecounter += (uint32_t)mon_table[t] * 86400; //月份秒钟数相加
        if(Is_Leap_Year(syear) && t == 1)datetimecounter += 86400; //闰年2月份增加一天的秒钟数
    }
    datetimecounter += (uint32_t)(sday - 1) * 86400; //把前面日期的秒钟数相加
    datetimecounter += (uint32_t)hour * 3600; //小时秒钟数
    datetimecounter += (uint32_t)min * 60;	 //分钟秒钟数
    datetimecounter += sec; //最后的秒钟加上去

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
//得到当前的时间
//返回值:0,成功;其他:错误代码.
uint8_t rtc_date_get(datetime *timenow) //时间结构体
{
     uint32_t temp = 0;
    uint16_t temp1 = 0;
    temp = datetimecounter / 86400; //得到天数(秒钟数对应的)
     {
         temp1 = 1970;	//从1970年开始
        while(temp >= 365)
        {
            if(Is_Leap_Year(temp1))//是闰年
            {
                if(temp >= 366)
                    temp -= 366; //闰年的秒钟数
                else
                {
                    temp1++;       //加一年
                    break;
                }
            }
            else
                temp -= 365;	 //平年
            temp1++;          //加一年
        }
        timenow->year = temp1; //得到年份
        temp1 = 0;
        while(temp >= 28) //超过了一个月
        {
            if(Is_Leap_Year(timenow->year) && temp1 == 1) //当年是不是闰年/2月份
            {
                if(temp >= 29)temp -= 29; //闰年的秒钟数
                else break;
            }
            else
            {
                if(temp >= mon_table[temp1])temp -= mon_table[temp1]; //平年
                else break;
            }
            temp1++;
        }
        timenow->month = temp1 + 1;	//得到月份
        timenow->day = temp + 1;  	//得到日期
    }
    temp = datetimecounter % 86400;     		//得到秒钟数
    timenow->h = temp / 3600;     	//小时
    timenow->m = (temp % 3600) / 60; 	//分钟
    timenow->s = (temp % 3600) % 60; 	//秒钟
    timenow->week = rtc_get_week(timenow->year, timenow->month, timenow->day); //获取星期
    return 0;
}
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日
//返回值：星期号		0~6
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;
    // 如果为21世纪,年份数加100
    if (yearH > 19)yearL += 100;
    // 所过闰年数只算1900年之后的
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)temp2--;
    return(temp2 % 7);
}

//#define ONE_SECOND_INTERVAL       APP_TIMER_TICKS(1000, 0)

/***************************************************************************
* 功能描述：时间中断
* 输入参数：
*****************************************************************************/
void calender_timeout_handler(void *p_context)
{
     datetimecounter++;    //秒为单位
}

void rtc_create_time(void)  //创新时间函数
{
//    uint32_t err_code;
//    err_code = app_timer_create(&m_rtc_timer_id, APP_TIMER_MODE_REPEATED, calender_timeout_handler);//创建rtc
//    APP_ERROR_CHECK(err_code);
}
void rtc_time_start(void) //开启rtc时钟
{
//    uint32_t err_code;
//    err_code = app_timer_start(m_rtc_timer_id, ONE_SECOND_INTERVAL, NULL);//开始rtc
//    APP_ERROR_CHECK(err_code);
}
void rtc_time_stop(void) //关闭rtc时钟
{
//    uint32_t err_code;
//    err_code = app_timer_stop(m_rtc_timer_id);//关闭alarm
//    APP_ERROR_CHECK(err_code);
}


/**************************************************
**函数：timerclock_init
**功能：整合的时钟初始化函数

**描述：
**备注：边学边写

**作者：执念执战
**时间：2017/2/12	11:21
*************************************************/
void  timersclock_init()
{

}
/**************************************************
**函数：showtime
**功能：整合的时钟显示函数

**描述：会和刷屏函数的定时器冲突，只有刷屏低于5帧（2帧？）每秒时，现实才正常，但是时间不准，会跳秒，感觉是定时器冲突
**备注：边学边写

**作者：执念执战
**时间：2017/2/12	11:21
*************************************************/

void showtime(u8 x,u8 y)
{
			datetime  timenow;
			char year[4],month[2],day[2],hour[2],min[2],sec[2];
			rtc_date_get(&timenow);
			year[0]=(char)(timenow.year/1000+0x030);		//加上0x30转换为ASCII码
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
**函数：IM_clock
**功能：模拟时钟（imulation clock）

**描述：	xo,yo :要画的模拟时钟的原点	r:要画的模拟时钟的半径
**备注：边学边写

**作者：执念执战
**时间：2017/2/12	11:21
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
**函数：IM_clock
**功能：模拟时钟（imulation clock）

**描述：xo,yo :要画的模拟时钟的原点	r:要画的模拟时钟的半径
					设置r的大小就可以改变模拟时钟的大小，位置也可以变动

**备注：边学边写,仿写于之前给梁通做的课设
				很好，可以用，之前给他写的仿真器上老是画乱码，看样是cpu速度不够的原因

**作者：执念执战
**时间：2017/2/12	11:21       14:00修改
*************************************************/
void IMclock_move(u8 xo,u8 yo,u8 r)
{
//		datetime  timenow;
//		rtc_date_get(&timenow);	//获取时间
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
		{DrawHand(xo,yo,r-r/6,59,6,bckcolo);} 		//用于消去整点之前的的一个秒针，即59过后的整点，下面是消除分针时针的，原理相同
	else
		{DrawHand(xo,yo,r-r/6,timeinfo.tm_sec-1,6,bckcolo);}		//消除前一个秒针


	if(timeinfo.tm_min==0)
		{DrawHand(xo,yo,r-r/4,59,6,bckcolo);}
	else
		{DrawHand(xo,yo,r-r/4,timeinfo.tm_min-1,6,bckcolo);}


	if(timeinfo.tm_hour==0)
		{DrawHand(xo,yo,r/2,11,30,bckcolo);}
	else
		{DrawHand(xo,yo,r/2,(timeinfo.tm_hour)%12-1,30,bckcolo);}



	DrawHand(xo,yo,r-r/6,timeinfo.tm_sec,6,BLACK);	 //画针的
	DrawHand(xo,yo,r-r/4,timeinfo.tm_min,6,BLUE);
	DrawHand(xo,yo,r/2,(timeinfo.tm_hour%12),30,RED);






}
/**************************************************
**函数：IM_clock
**功能：计算指针位置并画指针

**描述：
**备注：边学边写

**作者：执念执战
**时间：2017/2/12	11:21
*************************************************/
void DrawHand(uchar x0,uchar y0,uchar  R,uchar  Time,uchar step,u16 color)
{	   double pi=3.14;
        uchar x,y;
        x=R*cos(Time*step*pi/180-pi/2)+x0;
        y=R*sin(Time*step*pi/180-pi/2)+y0;

        //LCD_DrawLine1((u16)x0,(u16)y0,(u16)x,(u16)y,(u16)color);
        GUI_LineWith(x0,y0,x,y,3,color);
}

