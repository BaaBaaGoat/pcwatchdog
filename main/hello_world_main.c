/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "bsp.h"
#include "nvs_flash.h"
#include "bluetooth.h"
#include "mqtt.h"
#include "network.h"
#include "config.h"

#include "esp_log.h"
volatile TickType_t lastFeedDog = 0;

void RestartPower(){
	gpio_set_level(RELAY,1);
	gpio_set_level(LED,1);
	vTaskDelay(10*CONFIG_FREERTOS_HZ);
	gpio_set_level(RELAY,0);
	gpio_set_level(LED,0);

}
void parsejson(cJSON * json){
	cJSON* cmd = cJSON_GetObjectItem(json, "Cmd");
	if(strcmp(cJSON_GetStringValue(cmd) ,"config")==0){
		ParseJsonConfig(json);
		vTaskDelay(200);
		esp_restart();
	}
	if(strcmp(cJSON_GetStringValue(cmd) ,"reboot")==0){
		RestartPower();
		lastFeedDog = xTaskGetTickCount();
	}
	if(strcmp(cJSON_GetStringValue(cmd) ,"feed")==0){
		lastFeedDog = xTaskGetTickCount();
	}
}
void OnMQTTArrive(char* topic,int topiclen,char* data,int datalen){
	if(datalen<=2)return;
	cJSON* json=cJSON_ParseWithLength(data,datalen);
	parsejson(json);
	cJSON_Delete(json);

}
void Bluetooth_Uplink_callback(unsigned char* data,int* len/*<600Bytes*/){

	cJSON* json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "SSID",config.SSID);
	cJSON_AddStringToObject(json, "WifiPass",config.WifiPass);
	cJSON_AddStringToObject(json, "MQTTIP",config.MQTTIP);
	cJSON_AddNumberToObject(json, "MQTTPort",config.MQTTPort);
	cJSON_AddStringToObject(json, "MQTTAccount",config.MQTTAccount);
	cJSON_AddStringToObject(json, "MQTTPass",config.MQTTPass);
	cJSON_AddNumberToObject(json, "WatchDogInterval_sec",config.WatchDogInterval_sec);

	cJSON_PrintPreallocated(json,(char*)data,600,false);
	*len=strlen((char*)data);
	cJSON_Delete(json);
	printf("%s\n",data);
}
void Bluetooth_Downlink_callback(unsigned char* data,int len){
	if(len<=2)return;
	cJSON* json=cJSON_ParseWithLength((void*)data,len);
	parsejson(json);
	cJSON_Delete(json);
}
void app_main(void)
{
	gpio_reset_pin(RELAY);
	gpio_set_direction(RELAY,GPIO_MODE_OUTPUT);
	gpio_set_level(RELAY,0);
	gpio_reset_pin(LED);
	gpio_set_direction(LED,GPIO_MODE_OUTPUT);
	gpio_set_level(LED,0);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    LoadConfig();
    Init_Bluetooth();
    InitNetwork();
    ConnectToNetwork();
    Init_MQTT();
    lastFeedDog = xTaskGetTickCount();
    while(1){
    	// 检查看门狗是否到期
    	// 到期则重启继电器
    	ESP_LOGI("WatchDog","WatchDog %d/%d",(xTaskGetTickCount()-lastFeedDog)/CONFIG_FREERTOS_HZ, config.WatchDogInterval_sec);
    	if(config.WatchDogInterval_sec != 0 && (xTaskGetTickCount()-lastFeedDog) >  CONFIG_FREERTOS_HZ * config.WatchDogInterval_sec){
    		RestartPower();
    		lastFeedDog = xTaskGetTickCount();
    	}
    	vTaskDelay(CONFIG_FREERTOS_HZ);

    }
}
/* 通信协议
 * 蓝牙和mqtt均可向板子发送数据
 * 数据为json格式 mqtt topic为/pcwatchdog
 * {"Cmd":"config","MQTTIP":"192.168.1.1","MQTTPort":1883,"MQTTAccount":"","MQTTPass":"","SSID":"eulalawrence","WifiPass":"testonly","WatchDogInterval_sec":900} 配置参数
 * {"Cmd":"reboot"} 立马重启
 * {"Cmd":"feed"} 喂狗
 */
