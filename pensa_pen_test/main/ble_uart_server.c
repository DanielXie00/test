/* Copyright (c) 2017 pcbreflux. All Rights Reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "bt.h"
#include "bta_api.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_main.h"

#include <driver/rmt.h>
#include <driver/gpio.h>

#include "sdkconfig.h"

#include "ble_uart_server.h"

#include "oled1306.h"
#include "ssd1366.h"

#define SMART_FLOW   0x01
#define DEVICE_ID   0x02
#define GET_TEMPERATURE   0x03
#define SET_TEMPERATURE   0x04
#define GET_TIME   0x05
#define SET_TIME   0x06
#define BATTERY_LEVEL   0x07
#define CARTRIDGE_INFO   0x08
#define GET_FLOW_LOGS   0x09
#define KEYVALUE   0x0a
#define STATUS   0x0b
#define HW_VERSION   0x0c
#define USER_DATA   0x0d
#define ERROR_LOGS   0x0e
#define PENSA_TEM   0x0F
#define PENSA_PRESSURE   0x10
#define SFM3000_FLOW   0x11
#define SFM3000_TEM   0x12
#define PENSA   0x13
#define SFM3000   0x14
#define ALL   0x15
#define TOTAL_FLOW   0x16
#define TARGET_FLOW   0x17
#define FLOW_RESET_TIME   0x18

#define GATTS_TAG "GATTS"
#define GATTS_TABLE_TAG "GATTS_TABLE_DEMO"
#define LED_PIN GPIO_NUM_22  //led显示引脚
#define HIGH 1
#define LOW 0

#define PROFILE_NUM                 2
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x55
#define SAMPLE_DEVICE_NAME          "SmartFlow"
#define SVC_INST_ID                 0

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 100
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)
static uint8_t led_stat=0;
static uint8_t adv_config_done       = 0;
uint16_t heart_rate_handle_table[HRS_IDX_NB];

uint16_t Time_delay=200;
extern float Pensa_Tem;
extern float Pensa_pressure;


extern float SFM3000_Flow_value;
extern float SFM3000_Temperature_value;
extern uint16_t stop_point;
extern float    start_point;

extern float SFM3000_Total_Flow;
extern float Target_Flow;
extern uint16_t FlowResetTime;

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

static prepare_type_env_t prepare_write_env;


uint8_t RX_char_str[GATTS_CHAR_VAL_LEN_MAX] = {0x11,0x22,0x33};
uint8_t TX_char_str[GATTS_CHAR_VAL_LEN_MAX] = {0x44,0x55,0x66};
uint8_t RX_descr_str[GATTS_CHAR_VAL_LEN_MAX] = "RX_descr_str";
uint8_t TX_descr_str[GATTS_CHAR_VAL_LEN_MAX] = "TX_descr_str";
uint8_t Data_type=0;
static bool is_connected = false;// connection flag.
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

//#define GATTS_SERVICE_UUID_TEST_A   0x00FF
//#define GATTS_CHAR_UUID_TEST_A      0xFF01
#define GATTS_DESCR_UUID_TEST_A     0x3333
#define GATTS_NUM_HANDLE_TEST_A     4
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40
uint8_t TX_test_buff[20] = "TX_descr_str";
uint8_t notify_data[16]="notify_data";

uint8_t f_Send_Data = F_Standby;//The send data flag 
uint8_t char1_str[] = {0x11,0x22,0x33};
esp_attr_value_t gatts_demo_char1_val =
{
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
    .attr_len     = sizeof(char1_str),
    .attr_value   = char1_str,
};

esp_attr_value_t RX_gatts_demo_char_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len		= sizeof(RX_char_str),
	.attr_value     = RX_char_str,
};

esp_attr_value_t TX_gatts_demo_char_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len		= sizeof(TX_char_str),
	.attr_value     = TX_char_str,
};

esp_attr_value_t RX_gatts_demo_descr_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len		= 13,
	.attr_value     = RX_descr_str,
};

esp_attr_value_t TX_gatts_demo_descr_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len		= 13,
	.attr_value     = TX_descr_str,
};

#define BLE_SERVICE_UUID_SIZE 32
//#define BLE_SERVICE_UUID_SIZE ESP_UUID_LEN_128

// Add more UUIDs for more then one Service 为多个服务添加更多UUID
static uint8_t ble_service_uuid128[BLE_SERVICE_UUID_SIZE] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
	 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E,
         0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xAB, 0xCD, 0x00, 0x00,
    //0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t ble_manufacturer[BLE_MANUFACTURER_DATA_LEN] =  {0x12, 0x23, 0x45, 0x56};

static uint32_t ble_add_char_pos;

static esp_ble_adv_data_t ble_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .manufacturer_len = BLE_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  (uint8_t *)ble_manufacturer,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = BLE_SERVICE_UUID_SIZE,
    .p_service_uuid = ble_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp        = true,
    .include_name        = true,
    .include_txpower     = true,
    .min_interval        = 0x20,
    .max_interval        = 0x40,
    .appearance          = 0x00,
    .manufacturer_len    = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //&test_manufacturer[0],
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = BLE_SERVICE_UUID_SIZE,
    .p_service_uuid      = ble_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

struct gatts_char_inst {
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t char_perm;
	esp_gatt_char_prop_t char_property;
	esp_attr_value_t *char_val;
    esp_attr_control_t *char_control;
    uint16_t char_handle;
    esp_gatts_cb_t char_read_callback;
	esp_gatts_cb_t char_write_callback;
    esp_bt_uuid_t descr_uuid;
    esp_gatt_perm_t descr_perm;
	esp_attr_value_t *descr_val;
    esp_attr_control_t *descr_control;
    uint16_t descr_handle;
    esp_gatts_cb_t descr_read_callback;
	esp_gatts_cb_t descr_write_callback;
};

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
/*
static struct gatts_profile_inst gl_profile_tab[1] = {
         .gatts_if = ESP_GATT_IF_NONE,      
};*/

static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_a_event_handler,
         .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    [BLE_PROFILE_APP_ID] = {
        .gatts_cb = gatts_profile_event_handler,                   /* This demo does not implement, similar as profile A */
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

/* Service */
static const uint16_t GATTS_SERVICE_UUID_TEST      = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A       = 0xFF01;
static const uint16_t GATTS_CHAR_UUID_TEST_B       = 0xFF02;
static const uint16_t GATTS_CHAR_UUID_TEST_C       = 0xFF03;

static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read                =  ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write               = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_read_write_notify   = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t heart_measurement_ccc[2]      = {0x00, 0x00};
static const uint8_t char_value[4]                 = {0x11, 0x22, 0x33, 0x44};


/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] =
{
    // Service Declaration
    [IDX_SVC]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_TEST), (uint8_t *)&GATTS_SERVICE_UUID_TEST}},

    /* Characteristic Declaration */
    [IDX_CHAR_A]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_A] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_A, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_A]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},

    /* Characteristic Declaration */
    [IDX_CHAR_B]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_B]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_B, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

    /* Characteristic Declaration */
    [IDX_CHAR_C]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_C]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_C, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

};
/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_char_inst gl_char[GATTS_CHAR_NUM] = {
		{
				.char_uuid.len = ESP_UUID_LEN_128, // RX
				.char_uuid.uuid.uuid128 =  { 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E },
				.char_perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
				.char_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
				.char_val = &RX_gatts_demo_char_val,
				.char_control = NULL,
				.char_handle = 0,
				.char_read_callback=RX_char_read_handler,
				.char_write_callback=RX_char_write_handler,
				.descr_uuid.len = ESP_UUID_LEN_16,
				.descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,
				.descr_perm=ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
				.descr_val = &RX_gatts_demo_descr_val,
				.descr_control=NULL,
				.descr_handle=0,
				.descr_read_callback=RX_descr_read_handler,
				.descr_write_callback=RX_descr_write_handler
		},
		{
				.char_uuid.len = ESP_UUID_LEN_128,  // TX
				.char_uuid.uuid.uuid128 =  { 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E },
				.char_perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
				.char_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
				.char_val = &TX_gatts_demo_char_val,
				.char_control=NULL,
				.char_handle=0,
				.char_read_callback=TX_char_read_handler,
				.char_write_callback=TX_char_write_handler,
				.descr_uuid.len = ESP_UUID_LEN_16,
				.descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG, // ESP_GATT_UUID_CHAR_DESCRIPTION,
				.descr_perm=ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
				.descr_val = &TX_gatts_demo_descr_val,
				.descr_control=NULL,
				.descr_handle=0,
				.descr_read_callback=TX_descr_read_handler,
				.descr_write_callback=TX_descr_write_handler
		}
};

void RX_char_read_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	ESP_LOGI(GATTS_TAG, "RX_char_read_handler %d\n", param->read.handle);

	esp_gatt_rsp_t rsp;
	memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
	rsp.attr_value.handle = param->read.handle;
	if (gl_char[0].char_val!=NULL) {
		ESP_LOGI(GATTS_TAG, "RX_char_read_handler char_val %d\n",gl_char[0].char_val->attr_len);
		rsp.attr_value.len = gl_char[0].char_val->attr_len;
		for (uint32_t pos=0;pos<gl_char[0].char_val->attr_len&&pos<gl_char[0].char_val->attr_max_len;pos++) {
			rsp.attr_value.value[pos] = gl_char[0].char_val->attr_value[pos];
		}
	}
	ESP_LOGI(GATTS_TAG, "RX_char_read_handler esp_gatt_rsp_t\n");
	esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
								ESP_GATT_OK, &rsp);
}

void TX_char_read_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	ESP_LOGI(GATTS_TAG, "TX_char_read_handler %d\n", param->read.handle);

	esp_gatt_rsp_t rsp;
	memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
	rsp.attr_value.handle = param->read.handle;
	if (gl_char[1].char_val!=NULL) {
		ESP_LOGI(GATTS_TAG, "TX_char_read_handler char_val\n %d\n",gl_char[1].char_val->attr_len);
		rsp.attr_value.len = gl_char[1].char_val->attr_len;
		for (uint32_t pos=0;pos<gl_char[1].char_val->attr_len&&pos<gl_char[1].char_val->attr_max_len;pos++) {
			rsp.attr_value.value[pos] = gl_char[1].char_val->attr_value[pos];
		}
	}
	ESP_LOGI(GATTS_TAG, "TX_char_read_handler esp_gatt_rsp_t\n");
	esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
								ESP_GATT_OK, &rsp);
}

void RX_descr_read_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	ESP_LOGI(GATTS_TAG, "RX_descr_read_handler %d\n", param->read.handle);

	esp_gatt_rsp_t rsp;
	memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
	rsp.attr_value.handle = param->read.handle;
	if (gl_char[0].descr_val!=NULL) {
		ESP_LOGI(GATTS_TAG, "RX_descr_read_handler descr_val %d\n",gl_char[0].descr_val->attr_len);
		rsp.attr_value.len = gl_char[0].descr_val->attr_len;
		for (uint32_t pos=0;pos<gl_char[0].descr_val->attr_len&&pos<gl_char[0].descr_val->attr_max_len;pos++) {
			rsp.attr_value.value[pos] = gl_char[0].descr_val->attr_value[pos];
		}
	}
	ESP_LOGI(GATTS_TAG, "RX_descr_read_handler esp_gatt_rsp_t \n");
	esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
								ESP_GATT_OK, &rsp);
}

void TX_descr_read_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	ESP_LOGI(GATTS_TAG, "TX_descr_read_handler %d\n", param->read.handle);

	esp_gatt_rsp_t rsp;
	memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
	rsp.attr_value.handle = param->read.handle;
	if (gl_char[1].descr_val!=NULL) {
		ESP_LOGI(GATTS_TAG, "TX_descr_read_handler descr_val %d\n",gl_char[1].descr_val->attr_len);
		rsp.attr_value.len = gl_char[1].descr_val->attr_len;
		for (uint32_t pos=0;pos<gl_char[1].descr_val->attr_len&&pos<gl_char[1].descr_val->attr_max_len;pos++) {
			rsp.attr_value.value[pos] = gl_char[1].descr_val->attr_value[pos];
		}
	}
	ESP_LOGI(GATTS_TAG, "TX_descr_read_handler esp_gatt_rsp_t\n");
	esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
								ESP_GATT_OK, &rsp);
}

static  uint16_t notify_conn_id = 0;
static  esp_gatt_if_t notify_gatts_if = NULL;
static uint8_t notify_pos=0;
static uint8_t is_notify=0; 
//广播数据，发送数捿
void char2_notify_handle(esp_gatt_if_t gatts_if, uint16_t conn_id) {
	if (is_notify==1) {
		notify_pos='0';
		//for (uint32_t i=0;i<10;i++)
		 {
			ESP_LOGI(GATTS_TAG, "char2_notify_handle esp_ble_gatts_send_indicate\n");
			// vTaskDelay(1000 / portTICK_RATE_MS); // delay 1s
			esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,1,&notify_pos,false);
			
			notify_pos++;
		}
	}
}
char PensaTembuff[20];//用于打印数据
char SFM3000Tembuff[20];//用于打印数据
char SFM3000Flowbuff[20];//用于打印数据  
char PensaPressurebuff[20];//用于打印数据  
char TotalFlowBuff[20];//用于打印数据  
char TargetFlowbuff[20];//用于打印数据  
        
//广播数据，发送数捿
void char2_notify_buff_handle(esp_gatt_if_t gatts_if, uint16_t conn_id) 
{
        switch(Data_type)
        {
                case PENSA_TEM:
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaTembuff,false);
                        break; 
                case PENSA_PRESSURE:
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaTembuff,false);
                        break;                 
                case SFM3000_TEM:
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaTembuff,false);
                        break;                 
                case SFM3000_FLOW:
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaTembuff,false);
                        break; 
                case TOTAL_FLOW:
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaTembuff,false);
                        break; 
                case TARGET_FLOW: 
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaTembuff,false);
                        break;                        
                case PENSA:
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaTembuff,false);
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaPressurebuff,false);  
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                        break;
                case SFM3000:
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaTembuff,false);
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaPressurebuff,false);   
                        vTaskDelay(1 / portTICK_PERIOD_MS);    
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&TotalFlowBuff,false);
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&TargetFlowbuff,false); 
                        vTaskDelay(1 / portTICK_PERIOD_MS);                         
                        
                        
                                             
                        break;                 
                case ALL:
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaTembuff,false);
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&PensaPressurebuff,false);
                        vTaskDelay(1 / portTICK_PERIOD_MS);                   
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&SFM3000Tembuff,false);
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&SFM3000Flowbuff,false); 
                        vTaskDelay(1 / portTICK_PERIOD_MS);    
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&TotalFlowBuff,false);
                        vTaskDelay(1 / portTICK_PERIOD_MS);
                        esp_ble_gatts_send_indicate(gatts_if, conn_id, gl_char[1].char_handle,20,&TargetFlowbuff,false); 
                        vTaskDelay(1 / portTICK_PERIOD_MS); 
                                           
                        break; 
        }                 
              
}


void data_receive(void)
{
        if (strncmp((const char *)gl_char[0].char_val->attr_value,"1",2)==0) Data_type=SMART_FLOW;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"2",2)==0) Data_type=DEVICE_ID;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"3",2)==0) Data_type=GET_TEMPERATURE;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"4",2)==0) Data_type=SET_TEMPERATURE;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"5",2)==0) Data_type=GET_TIME;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"6",2)==0) Data_type=SET_TIME;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"7",2)==0) Data_type=BATTERY_LEVEL;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"8",2)==0) Data_type=CARTRIDGE_INFO;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"9",2)==0) Data_type=GET_FLOW_LOGS;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"10",2)==0) Data_type=KEYVALUE;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"11",2)==0) Data_type=STATUS;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"12",2)==0) Data_type=HW_VERSION;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"13",2)==0) Data_type=USER_DATA;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"14",2)==0) Data_type=ERROR_LOGS;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"PENSA_TEM",9)==0) Data_type=PENSA_TEM;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"PENSA_PRESSURE",14)==0) Data_type=PENSA_PRESSURE;
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"SFM3000_FLOW",12)==0) Data_type=SFM3000_FLOW; 
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"SFM3000_TEM",11)==0) Data_type=SFM3000_TEM;   
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"TOTAL_FLOW",10)==0) Data_type=TOTAL_FLOW;  
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"TARGET_FLOW",11)==0) Data_type=TARGET_FLOW;                 
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"L",1)==0) Data_type=PENSA;          
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"R",1)==0) Data_type=SFM3000;          
        else if (strncmp((const char *)gl_char[0].char_val->attr_value,"ALL",3)==0) Data_type=ALL;  
        else if(strncmp((const char *)gl_char[0].char_val->attr_value,"FLOWRESET",9)==0)//
        {
                FlowResetTime=(100*(gl_char[0].char_val->attr_value[9]-0x30))+(10*(gl_char[0].char_val->attr_value[10]-0x30))+(1*(gl_char[0].char_val->attr_value[11]-0x30));
                if(FlowResetTime>=1000)FlowResetTime=1000;
                if(FlowResetTime<=0)FlowResetTime=1;
        } 
         if(strncmp((const char *)gl_char[0].char_val->attr_value,"Time",4)==0)//
        {
                Time_delay=(100*(gl_char[0].char_val->attr_value[4]-0x30))+(10*(gl_char[0].char_val->attr_value[5]-0x30))+(1*(gl_char[0].char_val->attr_value[6]-0x30));
                if(Time_delay>=2000)Time_delay=2000;
                if(Time_delay<=10)Time_delay=10;
        }            
         if(strncmp((const char *)gl_char[0].char_val->attr_value,"Stop",4)==0)//
        {
                stop_point=(1000*(gl_char[0].char_val->attr_value[4]-0x30))+(100*(gl_char[0].char_val->attr_value[5]-0x30))+(10*(gl_char[0].char_val->attr_value[6]-0x30))+(1*(gl_char[0].char_val->attr_value[7]-0x30));
                if(stop_point>=10000)stop_point=10000;
                if(stop_point<=10)stop_point=10;
        }         
         if(strncmp((const char *)gl_char[0].char_val->attr_value,"Start",5)==0)//
        {
                start_point=(10*(gl_char[0].char_val->attr_value[5]-0x30))+(1*(gl_char[0].char_val->attr_value[6]-0x30))+(0.1*(gl_char[0].char_val->attr_value[7]-0x30))+(0.01*(gl_char[0].char_val->attr_value[8]-0x30));
                if(start_point>=100)start_point=100;
                if(start_point<=0)start_point=0.01;
        }             
}

void char_notify_data_type(void)
{

  

        memset(TX_test_buff, 0, sizeof(TX_test_buff));//
        memset(PensaTembuff, 0, sizeof(PensaTembuff));  
        memset(PensaPressurebuff, 0, sizeof(PensaPressurebuff));//
        memset(TotalFlowBuff, 0, sizeof(TotalFlowBuff));  
        memset(TargetFlowbuff, 0, sizeof(TargetFlowbuff));//
        memset(SFM3000Tembuff, 0, sizeof(SFM3000Tembuff));                        
        memset(SFM3000Flowbuff, 0, sizeof(SFM3000Flowbuff));                        
        switch(Data_type)
        {
                case SMART_FLOW:strcpy((const char *)TX_test_buff,"SMART_FLOW");                                        break;
                case DEVICE_ID:strcpy((const char *)TX_test_buff,"DEVICE_ID");                                          break;
                case GET_TEMPERATURE:strcpy((const char *)TX_test_buff,"GET_TEMPERATURE");                              break;
                case SET_TEMPERATURE:strcpy((const char *)TX_test_buff,"SET_TEMPERATURE");                              break;
                case GET_TIME: strcpy((const char *)TX_test_buff,"GET_TIME");                                           break;
                case SET_TIME: strcpy((const char *)TX_test_buff,"SET_TIME");                                           break;
                case BATTERY_LEVEL:strcpy((const char *)TX_test_buff,"BATTERY_LEVEL");                                  break;
                case CARTRIDGE_INFO:strcpy((const char *)TX_test_buff,"CARTRIDGE_INFO");                                break;
                case GET_FLOW_LOGS:strcpy((const char *)TX_test_buff,"GET_FLOW_LOGS");                                  break;
                case KEYVALUE:strcpy((const char *)TX_test_buff,"KEYVALUE");                                            break;
                case STATUS: strcpy((const char *)TX_test_buff,"STATUS");                                               break;
                case HW_VERSION: strcpy((const char *)TX_test_buff,"HW_VERSION");                                       break;
                case USER_DATA:strcpy((const char *)TX_test_buff,"USER_DATA");                                          break;
                case ERROR_LOGS:strcpy((const char *)TX_test_buff,"ERROR_LOGS");                                        break;

                case PENSA_TEM:
                        sprintf(PensaTembuff,"Pensa_Tem:%05.2f ",Pensa_Tem);
                        break; 
                case PENSA_PRESSURE:
                        sprintf(PensaTembuff,"Pensa_pressure:%05.2f ",Pensa_pressure);
                        break;                 
                case SFM3000_TEM:
                        sprintf(PensaTembuff,"SFM3000_Tem:%05.2f ",SFM3000_Temperature_value);
                        break;                 
                case SFM3000_FLOW:
                        sprintf(PensaTembuff,"SFM3000_Flow:%05.2f ",SFM3000_Flow_value); 
                        break; 
                case TOTAL_FLOW:
                        sprintf(PensaTembuff,"Total_Flow:%05.2f ",2*SFM3000_Total_Flow); 
                        break; 
                case TARGET_FLOW:
                        sprintf(PensaTembuff,"Target_Flow:%05.2f ",2*Target_Flow); 
                        break;                                                
                case PENSA:
                        sprintf(PensaTembuff,"Pensa_Tem:%05.2f ",Pensa_Tem);
                        sprintf(PensaPressurebuff,"pressure:%05.2f ",Pensa_pressure);                       
                        break;
                case SFM3000:
                        sprintf(PensaTembuff,"SFM3000_Tem:%05.2f ",SFM3000_Temperature_value);
                        sprintf(PensaPressurebuff,"Flow:%05.2f ",SFM3000_Flow_value); 
                        sprintf(TotalFlowBuff,"Total_Flow:%05.2f ",2*SFM3000_Total_Flow); 
                        sprintf(TargetFlowbuff,"Target_Flow:%05.2f ",2*Target_Flow);                                                                        
                        break;                 
                case ALL:
                        sprintf(PensaTembuff,"Pensa_Tem:%05.2f ",Pensa_Tem);
                        sprintf(PensaPressurebuff,"pressure:%05.2f ",Pensa_pressure);                        
                        sprintf(SFM3000Tembuff,"SFM3000_Tem:%05.2f ",SFM3000_Temperature_value);
                        sprintf(SFM3000Flowbuff,"Flow:%05.2f ",SFM3000_Flow_value);     
                        break;                  
        }
        char2_notify_buff_handle(notify_gatts_if, notify_conn_id);
}
void RX_char_write_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	ESP_LOGI(GATTS_TAG, "RX_char_write_handler %d\n", param->write.handle);

        char ble_dis_buf[16]={0};
       // int k=0;
	if (gl_char[0].char_val!=NULL) {
		ESP_LOGI(GATTS_TAG, "RX_char_write_handler char_val %d\n",param->write.len);
		gl_char[0].char_val->attr_len = param->write.len;
		for (uint32_t pos=0;pos<param->write.len;pos++) {
			gl_char[0].char_val->attr_value[pos]=param->write.value[pos];
			ble_dis_buf[pos]=param->write.value[pos];
		}
		ESP_LOGI(TAG, "RX_char_write_handler %.*s", gl_char[0].char_val->attr_len, (char*)gl_char[0].char_val->attr_value);
	}
	ESP_LOGI(GATTS_TAG, "RX_char_write_handler esp_gatt_rsp_t\n");

	//oled_display(OLED_2_ADDRESS,0,"Received data:  ");    //OLED显示
	//oled_display(OLED_2_ADDRESS,4,"                ");    //OLED显示	
        //oled_display(OLED_2_ADDRESS,4,ble_dis_buf);           //OLED显示	
        //oled_display(OLED_2_ADDRESS,7,"                ");    //OLED显示
	notify_gatts_if = gatts_if;
	notify_conn_id = param->write.conn_id;
    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
    if (strncmp((const char *)gl_char[0].char_val->attr_value,"getd",6)==0) {//比较字符串，若相等则控制LED灿
    //	gpio_set_level(LED_PIN,HIGH);
    //	led_stat=1;
        memset(TX_test_buff, 0, sizeof(TX_test_buff));
    	strcpy((const char *)TX_test_buff,"LED ON");
    	char2_notify_buff_handle(gatts_if, param->write.conn_id);//发鿁数捿    	
    	    	
    } else if (strncmp((const char *)gl_char[0].char_val->attr_value,"LED OFF",7)==0) {//比较字符串，若相等则控制LED灿
    //	gpio_set_level(LED_PIN,LOW);
    	//led_stat=0;
        memset(TX_test_buff, 0, sizeof(TX_test_buff));
    	strcpy((const char *)TX_test_buff,"LED OFF");
    	char2_notify_buff_handle(gatts_if, param->write.conn_id);//发鿁数捿    	
    } else if (strncmp((const char *)gl_char[0].char_val->attr_value,"LED SWITCH",10)==0) {//比较字符串，若相等则控制LED灿
    //	led_stat=1-led_stat;
    //	gpio_set_level(LED_PIN,led_stat);
    
    } else {
    	//char2_notify_handle(gatts_if, param->write.conn_id);//发鿁数捿
    }
    
    data_receive();//
        	//char2_notify_buff_handle(gatts_if, param->write.conn_id);//发鿁数捿    	

}

void TX_send_Data(void)
{

      if(is_connected)
      {
                if(NULL==notify_gatts_if)	{    	ESP_LOGI(GATTS_TAG, "notify_gatts_if =%d \n",notify_gatts_if);}
            	else 
            	{
            	        char_notify_data_type();
            	        //char2_notify_buff_handle(notify_gatts_if, notify_conn_id);
                	ESP_LOGI(GATTS_TAG, "notify_gatts_if =%d \n",notify_gatts_if);
            	
            	}//发鿁数捿    	
      }
}
void TX_char_write_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	ESP_LOGI(GATTS_TAG, "TX_char_write_handler %d\n", param->write.handle);

	if (gl_char[1].char_val!=NULL) {
		ESP_LOGI(GATTS_TAG, "TX_char_write_handler char_val %d\n",param->write.len);
		gl_char[1].char_val->attr_len = param->write.len;
		for (uint32_t pos=0;pos<param->write.len;pos++) {
			gl_char[1].char_val->attr_value[pos]=param->write.value[pos];
		}
	}
	ESP_LOGI(GATTS_TAG, "TX_char_write_handler esp_gatt_rsp_t\n");
    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
}

void RX_descr_write_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	ESP_LOGI(GATTS_TAG, "RX_descr_write_handler %d\n", param->write.handle);

	if (gl_char[0].descr_val!=NULL) {
		ESP_LOGI(GATTS_TAG, "RX_descr_write_handler descr_val %d\n",param->write.len);
		gl_char[0].descr_val->attr_len = param->write.len;
		for (uint32_t pos=0;pos<param->write.len;pos++) {
			gl_char[0].descr_val->attr_value[pos]=param->write.value[pos];
		}
	}
	ESP_LOGI(GATTS_TAG, "RX_descr_write_handler esp_gatt_rsp_t\n");
    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
}

void TX_descr_write_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	ESP_LOGI(GATTS_TAG, "TX_descr_write_handler %d\n", param->write.handle);

	if (gl_char[1].descr_val!=NULL) {
		ESP_LOGI(GATTS_TAG, "TX_descr_write_handler descr_val %d\n",param->write.len);
		gl_char[1].descr_val->attr_len = param->write.len;
		for (uint32_t pos=0;pos<param->write.len;pos++) {
			gl_char[1].descr_val->attr_value[pos]=param->write.value[pos];
		}
		is_notify = gl_char[1].descr_val->attr_value[0];
		ESP_LOGI(GATTS_TAG, "RX_descr_write_handler is_notify %d\n",is_notify);
	}
	ESP_LOGI(GATTS_TAG, "TX_descr_write_handler esp_gatt_rsp_t\n");
    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
}

void gatts_add_char() {

	ESP_LOGI(GATTS_TAG, "gatts_add_char %d\n", GATTS_CHAR_NUM);
	for (uint32_t pos=0;pos<GATTS_CHAR_NUM;pos++) {
		if (gl_char[pos].char_handle==0) {
			ESP_LOGI(GATTS_TAG, "ADD pos %d handle %d service %d\n", pos,gl_char[pos].char_handle,gl_profile_tab[BLE_PROFILE_APP_ID].service_handle);
			ble_add_char_pos=pos;
			esp_ble_gatts_add_char(gl_profile_tab[BLE_PROFILE_APP_ID].service_handle, &gl_char[pos].char_uuid,
								   gl_char[pos].char_perm,gl_char[pos].char_property,gl_char[pos].char_val, gl_char[pos].char_control);
			break;
		}
	}
}

void gatts_check_add_char(esp_bt_uuid_t char_uuid, uint16_t attr_handle) {

	ESP_LOGI(GATTS_TAG, "gatts_check_add_char %d\n", attr_handle);
	if (attr_handle != 0) {
		if (char_uuid.len == ESP_UUID_LEN_16) {
			ESP_LOGI(GATTS_TAG, "Char UUID16: %x", char_uuid.uuid.uuid16);
		} else if (char_uuid.len == ESP_UUID_LEN_32) {
			ESP_LOGI(GATTS_TAG, "Char UUID32: %x", char_uuid.uuid.uuid32);
		} else if (char_uuid.len == ESP_UUID_LEN_128) {
			ESP_LOGI(GATTS_TAG, "Char UUID128: %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x", char_uuid.uuid.uuid128[0],
					 char_uuid.uuid.uuid128[1], char_uuid.uuid.uuid128[2], char_uuid.uuid.uuid128[3],
					 char_uuid.uuid.uuid128[4], char_uuid.uuid.uuid128[5], char_uuid.uuid.uuid128[6],
					 char_uuid.uuid.uuid128[7], char_uuid.uuid.uuid128[8], char_uuid.uuid.uuid128[9],
					 char_uuid.uuid.uuid128[10], char_uuid.uuid.uuid128[11], char_uuid.uuid.uuid128[12],
					 char_uuid.uuid.uuid128[13], char_uuid.uuid.uuid128[14], char_uuid.uuid.uuid128[15]);
		} else {
			ESP_LOGE(GATTS_TAG, "Char UNKNOWN LEN %d\n", char_uuid.len);
		}

		ESP_LOGI(GATTS_TAG, "FOUND Char pos %d handle %d\n", ble_add_char_pos,attr_handle);
		gl_char[ble_add_char_pos].char_handle=attr_handle;
		// is there a descriptor to add ?
		if (gl_char[ble_add_char_pos].descr_uuid.len!=0 && gl_char[ble_add_char_pos].descr_handle==0) {
			ESP_LOGI(GATTS_TAG, "ADD Descr pos %d handle %d service %d\n", ble_add_char_pos,gl_char[ble_add_char_pos].descr_handle,gl_profile_tab[BLE_PROFILE_APP_ID].service_handle);
			esp_ble_gatts_add_char_descr(gl_profile_tab[BLE_PROFILE_APP_ID].service_handle, &gl_char[ble_add_char_pos].descr_uuid,
					gl_char[ble_add_char_pos].descr_perm, gl_char[ble_add_char_pos].descr_val, gl_char[ble_add_char_pos].descr_control);
		} else {
			gatts_add_char();
		}
	}
}



void gatts_check_add_descr(esp_bt_uuid_t descr_uuid, uint16_t attr_handle) {

	ESP_LOGI(GATTS_TAG, "gatts_check_add_descr %d\n", attr_handle);
	if (attr_handle != 0) {
		if (descr_uuid.len == ESP_UUID_LEN_16) {
			ESP_LOGI(GATTS_TAG, "Char UUID16: %x", descr_uuid.uuid.uuid16);
		} else if (descr_uuid.len == ESP_UUID_LEN_32) {
			ESP_LOGI(GATTS_TAG, "Char UUID32: %x", descr_uuid.uuid.uuid32);
		} else if (descr_uuid.len == ESP_UUID_LEN_128) {
			ESP_LOGI(GATTS_TAG, "Char UUID128: %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x", descr_uuid.uuid.uuid128[0],
					 descr_uuid.uuid.uuid128[1], descr_uuid.uuid.uuid128[2], descr_uuid.uuid.uuid128[3],
					 descr_uuid.uuid.uuid128[4], descr_uuid.uuid.uuid128[5], descr_uuid.uuid.uuid128[6],
					 descr_uuid.uuid.uuid128[7], descr_uuid.uuid.uuid128[8], descr_uuid.uuid.uuid128[9],
					 descr_uuid.uuid.uuid128[10], descr_uuid.uuid.uuid128[11], descr_uuid.uuid.uuid128[12],
					 descr_uuid.uuid.uuid128[13], descr_uuid.uuid.uuid128[14], descr_uuid.uuid.uuid128[15]);
		} else {
			ESP_LOGE(GATTS_TAG, "Descriptor UNKNOWN LEN %d\n", descr_uuid.len);
		}
		ESP_LOGI(GATTS_TAG, "FOUND Descriptor pos %d handle %d\n", ble_add_char_pos,attr_handle);
		gl_char[ble_add_char_pos].descr_handle=attr_handle;
	}
	gatts_add_char();
}

void gatts_check_callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	uint16_t handle=0;
	uint8_t read=1;

    switch (event) {
		case ESP_GATTS_READ_EVT: {
			read=1;
			handle=param->read.handle;
			break;
		}
		case ESP_GATTS_WRITE_EVT: {
			read=0;
			handle=param->write.handle;
		}
		default:
			break;
    }

	ESP_LOGI(GATTS_TAG, "gatts_check_callback read %d num %d handle %d\n", read, GATTS_CHAR_NUM, handle);
	for (uint32_t pos=0;pos<GATTS_CHAR_NUM;pos++) {
		if (gl_char[pos].char_handle==handle) {
			if (read==1) {
				if (gl_char[pos].char_read_callback!=NULL) {
					gl_char[pos].char_read_callback(event, gatts_if, param);
				}
			} else {
				if (gl_char[pos].char_write_callback!=NULL) {
					gl_char[pos].char_write_callback(event, gatts_if, param);
				}
			}
			break;
		}
		if (gl_char[pos].descr_handle==handle) {
			if (read==1) {
				if (gl_char[pos].descr_read_callback!=NULL) {
					gl_char[pos].descr_read_callback(event, gatts_if, param);
				}
			} else {
				if (gl_char[pos].descr_write_callback!=NULL) {
					gl_char[pos].descr_write_callback(event, gatts_if, param);
				}
			}
			break;
		}
	}
}


void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {

    switch (event) {
    #ifdef CONFIG_SET_RAW_ADV_DATA
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&ble_adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
            adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&ble_adv_params);
            }
            break;
    #else
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&ble_adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&ble_adv_params);
            }
            break;
    #endif
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            /* advertising start complete event to indicate advertising start successfully or failed */
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(GATTS_TABLE_TAG, "advertising start failed");
            }else{
                ESP_LOGI(GATTS_TABLE_TAG, "advertising start successfully");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(GATTS_TABLE_TAG, "advertising stop failed");
            }
            else {
                ESP_LOGI(GATTS_TABLE_TAG, "stop adv successfully\n");
            }
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "update connetion params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
            break;
        default:
            break;
    }
}

void example_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(GATTS_TABLE_TAG, "prepare write, handle = %d, value len = %d", param->write.handle, param->write.len);
    esp_gatt_status_t status = ESP_GATT_OK;
    if (prepare_write_env->prepare_buf == NULL) {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
        if (prepare_write_env->prepare_buf == NULL) {
            ESP_LOGE(GATTS_TABLE_TAG, "%s, gatt_server prep no mem", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
    } else {
        if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_OFFSET;
        } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_ATTR_LEN;
        }
    }
    /*send response when param->write.need_rsp is true */
    if (param->write.need_rsp){
        esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
        if (gatt_rsp != NULL){
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
               ESP_LOGE(GATTS_TABLE_TAG, "send response error");
            }
            free(gatt_rsp);
        }else{
            ESP_LOGE(GATTS_TABLE_TAG, "%s, malloc failed", __func__);
        }
    }
    if (status != ESP_GATT_OK){
        return;
    }
    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);
    prepare_write_env->prepare_len += param->write.len;

}
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC && prepare_write_env->prepare_buf){
        esp_log_buffer_hex(GATTS_TABLE_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }else{
        ESP_LOGI(GATTS_TABLE_TAG,"ESP_GATT_PREP_WRITE_CANCEL");
    }
    if (prepare_write_env->prepare_buf) {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
   switch (event) {
        case ESP_GATTS_REG_EVT:{
            esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(SAMPLE_DEVICE_NAME);
            if (set_dev_name_ret){
                ESP_LOGE(GATTS_TABLE_TAG, "set device name failed, error code = %x", set_dev_name_ret);
            }
    #ifdef CONFIG_SET_RAW_ADV_DATA
            esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
            if (raw_adv_ret){
                ESP_LOGE(GATTS_TABLE_TAG, "config raw adv data failed, error code = %x ", raw_adv_ret);
            }
            adv_config_done |= ADV_CONFIG_FLAG;
            esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));
            if (raw_scan_ret){
                ESP_LOGE(GATTS_TABLE_TAG, "config raw scan rsp data failed, error code = %x", raw_scan_ret);
            }
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
    #else
            //config adv data
            esp_err_t ret = esp_ble_gap_config_adv_data(&ble_adv_data);
            if (ret){
                ESP_LOGE(GATTS_TABLE_TAG, "config adv data failed, error code = %x", ret);
            }
            adv_config_done |= ADV_CONFIG_FLAG;
            //config scan response data
            ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
            if (ret){
                ESP_LOGE(GATTS_TABLE_TAG, "config scan response data failed, error code = %x", ret);
            }
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
    #endif
            esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
            if (create_attr_ret){
                ESP_LOGE(GATTS_TABLE_TAG, "create attr table failed, error code = %x", create_attr_ret);
            }
        }
       	    break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_READ_EVT");
       	    break;
        case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep){
                ESP_LOGI(GATTS_TABLE_TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :", param->write.handle, param->write.len);
                esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);
                if (heart_rate_handle_table[IDX_CHAR_CFG_A] == param->write.handle && param->write.len == 2){
                    uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
                    if (descr_value == 0x0001){
                        ESP_LOGI(GATTS_TABLE_TAG, "notify enable");
                        uint8_t notify_data[15];
                        for (int i = 0; i < sizeof(notify_data); ++i)
                        {
                            notify_data[i] = i % 0xff;
                        }
                        //the size of notify_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, heart_rate_handle_table[IDX_CHAR_VAL_A],
                                                sizeof(notify_data), notify_data, false);
                    }else if (descr_value == 0x0002){
                        ESP_LOGI(GATTS_TABLE_TAG, "indicate enable");
                        uint8_t indicate_data[15];
                        for (int i = 0; i < sizeof(indicate_data); ++i)
                        {
                            indicate_data[i] = i % 0xff;
                        }
                        //the size of indicate_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, heart_rate_handle_table[IDX_CHAR_VAL_A],
                                            sizeof(indicate_data), indicate_data, true);
                    }
                    else if (descr_value == 0x0000){
                        ESP_LOGI(GATTS_TABLE_TAG, "notify/indicate disable ");
                    }else{
                        ESP_LOGE(GATTS_TABLE_TAG, "unknown descr value");
                        esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);
                    }
                }
                /* send response when param->write.need_rsp is true*/
                if (param->write.need_rsp){
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
                }
            }else{
                /* handle prepare write */
                example_prepare_write_event_env(gatts_if, &prepare_write_env, param);
            }
      	    break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
            example_exec_write_event_env(&prepare_write_env, param);
            break;
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
            break;
        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONF_EVT, status = %d", param->conf.status);
            break;
        case ESP_GATTS_START_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
            break;
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
            esp_log_buffer_hex(GATTS_TABLE_TAG, param->connect.remote_bda, 6);
            esp_ble_conn_update_params_t conn_params = {0};
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
            conn_params.latency = 0;
            conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
            conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
            conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
            //start sent the update connection parameters to the peer device.
            esp_ble_gap_update_conn_params(&conn_params);
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = %d", param->disconnect.reason);
            esp_ble_gap_start_advertising(&ble_adv_params);
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:{
            if (param->add_attr_tab.status != ESP_GATT_OK){
                ESP_LOGE(GATTS_TABLE_TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
            }
            else if (param->add_attr_tab.num_handle != HRS_IDX_NB){
                ESP_LOGE(GATTS_TABLE_TAG, "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, HRS_IDX_NB);
            }
            else {
                ESP_LOGI(GATTS_TABLE_TAG, "create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
                memcpy(heart_rate_handle_table, param->add_attr_tab.handles, sizeof(heart_rate_handle_table));
                esp_ble_gatts_start_service(heart_rate_handle_table[IDX_SVC]);
            }
            break;
        }
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_DELETE_EVT:
        default:
            break;
    }
}
void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	esp_err_t ret;
	switch (event) {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_TAG, "REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
        gl_profile_tab[BLE_PROFILE_APP_ID].service_id.is_primary = true;
        gl_profile_tab[BLE_PROFILE_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[BLE_PROFILE_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_128;
        for (uint8_t pos=0;pos<ESP_UUID_LEN_128;pos++) {
        	gl_profile_tab[BLE_PROFILE_APP_ID].service_id.id.uuid.uuid.uuid128[pos]=ble_service_uuid128[pos];
        }

        esp_ble_gap_set_device_name(BLE_DEVICE_NAME);
        ret=esp_ble_gap_config_adv_data(&ble_adv_data);
        ESP_LOGI(GATTS_TAG, "esp_ble_gap_config_adv_data %d", ret);

        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[BLE_PROFILE_APP_ID].service_id, GATTS_NUM_HANDLE);
        break;
    case ESP_GATTS_READ_EVT: {
        ESP_LOGI(GATTS_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);
//        esp_gatt_rsp_t rsp;
//        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
//        rsp.attr_value.handle = param->read.handle;
//        rsp.attr_value.len = 4;
//        rsp.attr_value.value[0] = 0x0A;
//        rsp.attr_value.value[1] = 0x0B;
//        rsp.attr_value.value[2] = 0x0C;
//        rsp.attr_value.value[3] = 0x0D;
//        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
//                                    ESP_GATT_OK, &rsp);
        gatts_check_callback(event, gatts_if, param);
        break;
    }
    case ESP_GATTS_WRITE_EVT: {
        ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d\n", param->write.conn_id, param->write.trans_id, param->write.handle);
        ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, value len %d, value %08x\n", param->write.len, *(uint32_t *)param->write.value);
//       esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        gatts_check_callback(event, gatts_if, param);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
    case ESP_GATTS_MTU_EVT:
    case ESP_GATTS_CONF_EVT:
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(GATTS_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d\n", param->create.status, param->create.service_handle);
        gl_profile_tab[BLE_PROFILE_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[BLE_PROFILE_APP_ID].char_uuid.len = gl_char[0].char_uuid.len;
        gl_profile_tab[BLE_PROFILE_APP_ID].char_uuid.uuid.uuid16 = gl_char[0].char_uuid.uuid.uuid16;

        esp_ble_gatts_start_service(gl_profile_tab[BLE_PROFILE_APP_ID].service_handle);
        gatts_add_char();
//        esp_ble_gatts_add_char(gl_profile.service_handle, &gl_profile.char_uuid,
//                               ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
//                               ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
//                               &RX_gatts_demo_char_val, NULL);
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_EVT: {
//	    uint16_t length = 0;
//        const uint8_t *prf_char;

        ESP_LOGI(GATTS_TAG, "ADD_CHAR_EVT, status 0x%X,  attr_handle %d, service_handle %d\n",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        gl_profile_tab[BLE_PROFILE_APP_ID].char_handle = param->add_char.attr_handle;
//        gl_profile.descr_uuid.len = ESP_UUID_LEN_16;
//        gl_profile.descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
//        esp_ble_gatts_get_attr_value(param->add_char.attr_handle,  &length, &prf_char);


//        ESP_LOGI(GATTS_TAG, "the gatts demo char length = %x\n", length);
//        for(int i = 0; i < length; i++){
//            ESP_LOGI(GATTS_TAG, "prf_char[%x] =%x\n",i,prf_char[i]);
//        }
//        esp_ble_gatts_add_char_descr(gl_profile.service_handle, &gl_profile.descr_uuid,
//                                     ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
        if (param->add_char.status==ESP_GATT_OK) {
        	gatts_check_add_char(param->add_char.char_uuid,param->add_char.attr_handle);
        }
        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        ESP_LOGI(GATTS_TAG, "ADD_DESCR_EVT char, status %d, attr_handle %d, service_handle %d\n",
                 param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        ESP_LOGI(GATTS_TAG, "ADD_DESCR_EVT desc, status %d, attr_handle %d, service_handle %d\n",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        if (param->add_char_descr.status==ESP_GATT_OK) {
        	gatts_check_add_descr(param->add_char.char_uuid,param->add_char.attr_handle);
        }
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_TAG, "SERVICE_START_EVT, status %d, service_handle %d\n",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        is_connected = true;
        ESP_LOGI(GATTS_TAG, "SERVICE_START_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x: \n",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gl_profile_tab[BLE_PROFILE_APP_ID].conn_id = param->connect.conn_id;
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        is_connected = false;
        esp_ble_gap_start_advertising(&ble_adv_params);
        break;
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}

void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;

        } else {
            ESP_LOGI(GATTS_TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id, 
                    param->reg.status);
            return;
        }
    }

    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == gl_profile_tab[idx].gatts_if) {
                if (gl_profile_tab[idx].gatts_cb) {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}




