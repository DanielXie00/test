
#include "driver/gpio.h"
 #include <freertos/FreeRTOS.h>
 #include <freertos/task.h>
 #include <freertos/portmacro.h>
#include "esp_system.h"

//#include "c_timeutils.c"
//#include " c_timeutils.h"
//#include "esp_system.h"
//#include "esp_event.h"
//#include "esp_event_loop.h"
//
//#include "soc/rtc_cntl_reg.h"
//
//#include "driver/spi_master.h"
//#include "esp_log.h"


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



