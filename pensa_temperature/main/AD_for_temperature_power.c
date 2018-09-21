/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/



#include "AD_for_temperature_power.h"


#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "freertos/queue.h"

#include "esp_system.h"




#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   2         //Multisampling



const unsigned int  K_temperature_table[TEMPERATURE_TAB_NUM]={
1283,1288,1293,1299,1304,1309,1314,1319,1324,1329,//0
1334,1339,1345,1350,1355,1360,1365,1370,1376,1381,//10
1386,1391,1396,1401,1407,1412,1417,1422,1427,1433,//20
1438,1443,1448,1454,1459,1464,1469,1475,1480,1485,//30
1490,1496,1501,1506,1511,1517,1522,1527,1533,1538,//40
1543,1548,1554,1559,1564,1570,1575,1580,1585,1591,//50
1596,1601,1607,1612,1617,1623,1628,1633,1638,1644,//60
1649,1654,1660,1665,1670,1676,1681,1686,1692,1697,//70
1702,1708,1713,1718,1724,1729,1734,1739,1745,1750,//80
1755,1761,1766,1771,1777,1782,1787,1792,1798,1803,//90
1808,1814,1819,1824,1829,1835,1840,1845,1850,1856,//100
1861,1866,1871,1877,1882,1887,1892,1898,1903,1908,//110
1913,1919,1924,1929,1934,1939,1945,1950,1955,1960,//120
1965,1970,1976,1981,1986,1991,1996,2002,2007,2012,//130
2017,2022,2027,2032,2038,2043,2048,2053,2058,2063,//140
2068,2074,2079,2084,2089,2094,2099,2104,2109,2114,//150
2119,2124,2130,2135,2140,2145,2150,2155,2160,2165,//160
2170,2175,2180,2185,2191,2196,2201,2206,2211,2216,//170
2221,2226,2231,2236,2241,2246,2251,2256,2261,2266,//180
2272,2277,2282,2287,2292,2297,2302,2307,2312,2317,//190
2322,2327,2332,2337,2342,2347,2352,2358,2363,2368,//200
2373,2378,2383,2388,2393,2398,2403,2408,2413,2419,//210
2424,2429,2434,2439,2444,2449,2454,2459,2464,2469,//220
2474,2480,2485,2490,2495,2500,2505,2510,2515,2520,//230
2526,2531,2536,2541,2546,2551,2556,2561,2567,2572,//240
2577,2582,2587,2592,2597,2602,2608,2613,2618,2623,//250
2628,2633,2639,2644,2649,2654,2659,2664,2670,2675,//260
2680,2685,2690,2695,2700,2706,2711,2716,2721,2726,//270
2732,2737,2742,2747,2752,2757,2763,2768,2773,2778,//280
2784,2789,2794,2799,2804,2809,2815,2820,2825,2830,//290
2836,2841,2846,2851,2856,2862,2867,2872,2877,2882,//300

};
const unsigned int  NTC_3950[351]={
4082,4081,4080,4079,4079,4078,4077,4076,4075,4075,
4074,4073,4072,4070,4069,4068,4067,4066,4064,4063,
4062,4060,4058,4057,4055,4053,4052,4050,4048,4046,
4044,4042,4039,4037,4035,4032,4030,4027,4024,4021,
4018,4015,4012,4009,4006,4002,3999,3995,3991,3987,
3983,3979,3974,3970,3965,3961,3956,3951,3946,3940,
3935,3929,3923,3917,3911,3905,3899,3892,3885,3878,
3871,3863,3856,3848,3840,3832,3824,3815,3806,3797,
3788,3779,3769,3759,3749,3739,3728,3717,3707,3695,
3684,3672,3660,3648,3636,3623,3610,3597,3584,3570,
3556,3542,3528,3513,3498,3483,3468,3452,3436,3420,
3404,3387,3370,3353,3336,3319,3301,3283,3265,3246,
3228,3209,3190,3171,3151,3132,3112,3092,3072,3051,
3031,3010,2989,2968,2947,2925,2904,2882,2860,2838,
2816,2794,2771,2749,2727,2704,2681,2658,2636,2613,
2590,2567,2544,2520,2497,2474,2451,2428,2404,2381,
2358,2335,2311,2288,2265,2242,2219,2196,2173,2150,
2127,2104,2082,2059,2036,2014,1991,1969,1947,1925,
1903,1881,1859,1838,1816,1795,1774,1753,1732,1711,
1691,1670,1650,1630,1610,1590,1570,1551,1531,1512,
1493,1474,1456,1437,1419,1401,1383,1365,1347,1330,
1313,1296,1279,1262,1245,1229,1213,1197,1181,1166,
1150,1135,1120,1105,1090,1076,1061,1047,1033,1019,
1006,992,979,966,953,940,927,915,903,890,
878,867,855,843,832,821,810,799,788,777,
767,757,746,736,726,717,707,698,688,679,
670,661,652,644,635,626,618,610,602,594,
586,578,570,563,556,548,541,534,527,520,
513,507,500,493,487,481,474,468,462,456,
450,445,439,433,428,422,417,412,406,401,
396,391,386,381,377,372,367,363,358,354,
349,345,341,337,333,328,324,320,317,313,
309,305,302,298,294,291,287,284,281,277,
274,271,267,264,261,258,255,252,250,247,
244,241,238,235,233,230,227,225,222,220,
217
};
static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t pwm_channel =  ADC_CHANNEL_0 ;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_channel_t temperature_channel = ADC_CHANNEL_7 ;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_channel_t NTC_channel = ADC_CHANNEL_3 ;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;
extern uint32_t PWM_detection;


static void check_efuse()
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}
uint32_t NTC_Temperature(uint32_t advalue)
{
        uint32_t i = 0;
        if (advalue >= NTC_3950[0]) return 0x3ff;//断路，未接传感器
            if(advalue<=NTC_3950[350]) return 0x4ff;//短路，传感器短路
	        for (i = 0; i< 350;i++)
	        {
		        if (advalue>NTC_3950[i])
		        {
		         	return i;
		        }	
	        }	   
            return 380;	 
}
uint32_t Temperature_test(void)
{
    //Check if Two Point or Vref are burned into eFuse
   

    //Continuously sample ADC1
  //  while (1) 
	{
        uint32_t adc_reading = 0;
 
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
      
                adc_reading += adc1_get_raw((adc1_channel_t)temperature_channel);
        }
        adc_reading /= NO_OF_SAMPLES;


				
        return adc_reading;
		
       // vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void ADC_init(void)
{
 check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(temperature_channel|pwm_channel | NTC_channel, atten);
    } 
    
    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

}
uint32_t pwm_change_test(void)
{
        uint32_t adc_read;
        adc_read=adc1_get_raw((adc1_channel_t)pwm_channel);

        return adc_read;
		
}
uint32_t Get_NTC(void)
{
        uint32_t adc_read;
        adc_read=adc1_get_raw((adc1_channel_t)NTC_channel);

        return adc_read;
		
}
uint32_t get_temperture(uint32_t temperature_ad)
{
        uint32_t i;        
        for(i=0;i<TEMPERATURE_TAB_NUM;i++)
        {
                if(temperature_ad<K_temperature_table[0]) return 0;        
                if(temperature_ad<=K_temperature_table[i]) return i;
                if(temperature_ad>=K_temperature_table[TEMPERATURE_TAB_NUM-1]) return 310;
        
        } 
        return 1;
}



void ad2_app(void)
{

	xTaskCreate(&Temperature_test, "Temperature_test", 2048, NULL, 6, NULL);

}


