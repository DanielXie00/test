#ifndef __clock_try_H__
#define __clock_try_H__

#include "GUI.h"






#define uchar u8

#define APP_TIMER_PRESCALER       0                         //��Ƶϵ��
#define APP_TIMER_OP_QUEUE_SIZE   4                         //��������



typedef struct
{
    uint8_t week;      //    3600s *24*7=604800
    uint8_t s;          //��60s
    uint8_t m;          //����60m   60s��ת
    uint8_t h;          //Сʱ24h   3600s��ת

    uint8_t day;       //��   3600s *24=86400
    uint8_t month;
    uint16_t year;      //2010��  256��
} datetime;             //7bit



void Set_time_now(void);
void Get_time_now();



uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);

uint8_t rtc_date_set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t rtc_date_get(datetime *timenow) ;//ʱ��ṹ��
void rtc_create_time(void);
void rtc_time_start(void); //����rtcʱ��
void rtc_time_stop(void); //�ر�rtcʱ��



void  timersclock_init(void);
void showtime(u8 x,u8 y);
void DrawHand(uchar x0,uchar y0,uchar  R,uchar  Time,uchar step,u16 color);
void IMclock_move(u8 xo,u8 yo,u8 r)	;
void IM_clock(u8 xo,u8 yo,u8 r);




#endif
