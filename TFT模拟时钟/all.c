#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "timer_clock.h"
#include "clock_try.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "wpa/common.h"
#include "lcd.h"
#include "GUI.h"
#include "LCD_GUI_SF.h"
//#include "FONT.h"
#include "mydelay.h"
#include "clock_try.h"


#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "timer_clock.h"
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

#define GPIO_OUTPUT_IO_LED   2
#define GPIO_OUTPUT_PIN_SEL  ((1<<GPIO_OUTPUT_IO_LED) )

#define TIMER_INTR_SEL TIMER_INTR_LEVEL  /*!< Timer level interrupt */
#define TIMER_GROUP    TIMER_GROUP_0     /*!< Test on timer group 0 */
#define TIMER_DIVIDER   16               /*!< Hardware timer clock divider */
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)  /*!< used to calculate counter value */
#define TIMER_FINE_ADJ   (1.4*(TIMER_BASE_CLK / TIMER_DIVIDER)/1000000) /*!< used to compensate alarm value */
#define TIMER_INTERVAL0_SEC   (1)   /*!< test interval for timer 0 */
#define TIMER_INTERVAL1_SEC   (1)   /*!< test interval for timer 1 */
#define TEST_WITHOUT_RELOAD   0   /*!< example of auto-reload mode */
#define TEST_WITH_RELOAD      1      /*!< example without auto-reload mode */



#define uchar u8

#define APP_TIMER_PRESCALER       0                         //分频系数
#define APP_TIMER_OP_QUEUE_SIZE   4                         //队列数量



typedef struct   
{
    uint8_t week;       //3600s *24*7=604800
    uint8_t s;          //秒60s
    uint8_t m;          //分钟60m   60s翻转
    uint8_t h;          //小时24h   3600s翻转

    uint8_t day;       //天  3600s *24=86400
    uint8_t month;
    uint16_t year;      //2010起  256年
} datetime;             //7bit



void Set_time_now(void);
void Get_time_now();



uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);

uint8_t rtc_date_set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t rtc_date_get(datetime *timenow) ;//时间结构体
void rtc_create_time(void);
void rtc_time_start(void); //开启rtc时钟
void rtc_time_stop(void); //关闭rtc时钟



void  timersclock_init(void);
void showtime(u8 x,u8 y);
void DrawHand(uchar x0,uchar y0,uchar  R,uchar  Time,uchar step,u16 color);
void IMclock_move(u8 xo,u8 yo,u8 r)	;
void IM_clock(u8 xo,u8 yo,u8 r);
typedef struct {
    int type;                  /*!< event type */
    int group;                 /*!< timer group */
    int idx;                   /*!< timer number */
    uint64_t counter_val;      /*!< timer counter value */
    double time_sec;           /*!< calculated time from counter value */
} timer_event_t;

xQueueHandle timer_queue;

static void example_tg0_timer1_init();
void moniclock_set();
void delay_us(int time_us);
void delay_ms(unsigned char  time_ms);
/*
 * @brief timer group0 ISR handler
 */
void IRAM_ATTR timer_group0_isr(void *para)
{
    int timer_idx = (int) para;
    uint32_t intr_status = TIMERG0.int_st_timers.val;
    timer_event_t evt;
 if((intr_status & BIT(timer_idx)) && timer_idx == TIMER_1) {
        /*Timer1 is an example that will reload counter value*/
        TIMERG0.hw_timer[timer_idx].update = 1;
        /*We don't call a API here because they are not declared with IRAM_ATTR*/
        TIMERG0.int_clr_timers.t1 = 1;
        uint64_t timer_val = ((uint64_t) TIMERG0.hw_timer[timer_idx].cnt_high) << 32
            | TIMERG0.hw_timer[timer_idx].cnt_low;
        double time = (double) timer_val / (TIMER_BASE_CLK / TIMERG0.hw_timer[timer_idx].config.divider);
        /*Post an event to out example task*/
        evt.type = TEST_WITH_RELOAD;
        evt.group = 0;
        evt.idx = timer_idx;
        evt.counter_val = timer_val;
        evt.time_sec = time;
        //xQueueSendFromISR(timer_queue, &evt, NULL);

        //中断要处理的任务，此处是时钟刷新
    	IMclock_move(64,64,32);


        /*For a auto-reload timer, we still need to set alarm_en bit if we want to enable alarm again.*/
        TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;
    }
}



/*
 * @brief timer group0 hardware timer1 init
 */
static void example_tg0_timer1_init()
{
    int timer_group = TIMER_GROUP_0;
    int timer_idx = TIMER_1;
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_SEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, TIMER_INTERVAL1_SEC * TIMER_SCALE);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
    /*Set ISR handler*/
    timer_isr_register(timer_group, timer_idx, timer_group0_isr, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    /*Start timer counter*/
    timer_start(timer_group, timer_idx);
}

/**
 * @brief In this test, we will test hardware timer0 and timer1 of timer group0.
 */
void moniclock_set()
{
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    example_tg0_timer1_init();
   // xTaskCreate(timer_example_evt_task, "timer_evt_task", 2048, NULL, 5, NULL);
}


int i;

//
//
//
//static void timer_example_evt_task(void *arg)
//{
//    while(1) {
//        timer_event_t evt;
//        xQueueReceive(timer_queue, &evt, portMAX_DELAY);
//        if(evt.type == TEST_WITHOUT_RELOAD) {
//            printf("\n\n   example of count-up-timer \n");
//        } else if(evt.type == TEST_WITH_RELOAD) {
//            printf("\n\n   example of reload-timer \n");
//
//        }
//        /*Show timer event from interrupt*/
//        printf("-------INTR TIME EVT--------\n");
//        printf("TG[%d] timer[%d] alarm evt\n", evt.group, evt.idx);
//        printf("reg: ");
//        print_u64(evt.counter_val);
//        printf("time: %.8f S\n", evt.time_sec);
//        /*Read timer value from task*/
//        printf("======TASK TIME======\n");
//        uint64_t timer_val;
//        timer_get_counter_value(evt.group, evt.idx, &timer_val);
//        double time;
//        timer_get_counter_time_sec(evt.group, evt.idx, &time);
//        printf("TG[%d] timer[%d] alarm evt\n", evt.group, evt.idx);
//        printf("reg: ");
//        print_u64(timer_val);
//        printf("time: %.8f S\n", time);
//    }
//}







void app_main()
{	   // vTaskDelay(100/1);

	 //初始化GPIO引脚
	  gpio_config_t io_conf;
	    //disable interrupt
	    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
	    //set as output mode
	    io_conf.mode = GPIO_MODE_OUTPUT;
	    //bit mask of the pins that you want to set,e.g.GPIO18/19
	    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	    //disable pull-down mode
	    io_conf.pull_down_en = 0;
	    //disable pull-up mode
	    io_conf.pull_up_en = 0;
	    //configure GPIO with the given settings
	    gpio_config(&io_conf);
	    printf("LED init OK!\n");


	    LCD_Init();
	    LCD_Clear(RED);

//	    while(1)
//	    	{
//	    	if((i%2)==0){
//	    	LCD_Clear(GREEN);
//	    	printf("GREEN \n");}
//	    	else
//	    	{
//	    		LCD_Clear(RED);
//	    		printf("RED \n");
//	    	}
//	    	i++;
//	    	if(i==20)i=0;
//	    }
//

	    u8 x,y,z;
	    unsigned int num=0;
	    	char number[3];


	    	//delay_init(72);	     //延时初始化
	    	//LCD_Init();	   //液晶屏初始化
	    	LCD_Clear(BLACK); //清屏
	    	gui_circle(128/2,128/2,WHITE,128/2-2, 0); //画圆
	    	gui_circle(128/2,128/2,WHITE,128/2-3, 0);//画圆

	    	Gui_StrCenter(0,10,BLUE,YELLOW,&"SysParam",16,1);//居中显示
	    	Gui_StrCenter(0,28,RED,BLUE,&"Temp:32℃",16,1);//居中显示
	    	Gui_StrCenter(0,46,YELLOW,BLUE,&"Hum:20%",16,1);//居中显示
	    	Gui_StrCenter(0,64,WHITE,BLUE,&"VOL:3.2V",16,1);//居中显示
	    	Gui_StrCenter(0,82,GREEN,BLUE,&"I:1.25mA",16,1);//居中显示
	    	Gui_StrCenter(0,100,RED,BLUE,&"12:20AM",16,1);//居中显示
	    	//Set_time_now();
	    	rtc_date_set(2017,8,18,13,30,30);
	    	//LCD_Clear(WHITE);
//	    	GUI_LineWith(0,5,50,50,3,RED);
//	    	GUI_LineWith(64,5,30,70,15,GREEN);
	    	//IM_clock(64,64,64);
	    	showiclockimage();
	    	moniclock_set();
	    	while(1){

//	    		LCD_show_big_image(num,20,gImage_qq23,2);
//	    		load_image();
//	    		LCD_show_big_image(num,20,gImage_qq23,1);
//
//	    		LCD_Show();
//	    		LCD_clearn_fill(BLACK);

	    		//Get_time_now();
	    		//IMclock_move(64,64,64);
	    		delay_ms(50);
	    	num++;
	    		if (num>40)
	    			num=0;
	    	};



}



void delay_us(uint32_t time_us){
	//int i=0,m=0;
	  uint32_t diff=0;
    static uint32_t lasthandshaketime;
    uint32_t currtime=xthal_get_ccount();
    lasthandshaketime=currtime;
    while(diff<250*2.5*time_us){
    	currtime=xthal_get_ccount();
    	diff=currtime-lasthandshaketime;}
  // diff=currtime-lasthandshaketime;
  //  if (diff<240) ( i++);
   // printf("now delay diff: %d ",diff);
    //ignore everything <1ms after an earlier irq
//for(i=0;i<time_us;i++)
//	for (m=0;m<30;i++);
}
void delay_ms(unsigned char  time_ms)
{	uint32_t i;
for(i=0;i<time_ms;i++)
	delay_us(1000);
	  //vTaskDelay(time_ms/portTICK_RATE_MS );
	}


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


void Set_time_now(void)
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


void Set_time_now(void)
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


static void obtain_time(void)//获取时间
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

// 计算2000～2099年任一天星期几 
// year        : 00-99 
// month       : 01-12 
// day         : 01-31 
// 返回 1 -> 7 : 星期一 -> 星期天
unsigned char GetDayFromDate(unsigned char year, unsigned char month, unsigned char date)
{ 
    if (month == 1 || month == 2)  
    { 
        month += 12;
        if (year > 0) 
            year--; 
        else 
            year = 4;
    } 
    return (1 + ((date + 2 * month + 3 * (month + 1) / 5 + year + year / 4) % 7)); 
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




