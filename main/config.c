#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "RTC.h"
#include "nvs_flash.h"
#include "cJSON.h"
#include "math.h"
#include "string.h"
#include "config.h"
#include "esp_log.h"
#include <esp_https_ota.h>
config_t config;
void SaveConfig();
void LoadDefaultConfig(){

	strcpy(config.SSID,"eulalawrence");
	strcpy(config.WifiPass,"defaultpass");
	strcpy(config.MQTTIP,"192.168.1.100");
	config.MQTTPort = 1883;
	strcpy(config.MQTTAccount,"");
	strcpy(config.MQTTPass,"");
	config.WatchDogInterval_sec = 0;
	SaveConfig();
}
void LoadConfig(){
	nvs_handle_t my_handle;
	nvs_open("storage", NVS_READWRITE, &my_handle);
	uint8_t flag_success = 1;
	int size;
	size=sizeof(config.MQTTIP);flag_success =flag_success&& (ESP_OK == nvs_get_str (my_handle, "MQTTIP", config.MQTTIP,(void*)&size));
	flag_success =flag_success&& (ESP_OK == nvs_get_u16 (my_handle, "MQTTPort",&config.MQTTPort));
	size=sizeof(config.MQTTAccount);flag_success =flag_success&& (ESP_OK == nvs_get_str (my_handle, "MQTTAccount",config.MQTTAccount,(void*)&size));
	size=sizeof(config.MQTTPass);flag_success =flag_success&& (ESP_OK == nvs_get_str (my_handle, "MQTTPass", config.MQTTPass,(void*)&size));

	size=sizeof(config.SSID);flag_success =flag_success&& (ESP_OK == nvs_get_str (my_handle, "SSID",config.SSID,(void*)&size));
	size=sizeof(config.WifiPass);flag_success =flag_success&& (ESP_OK == nvs_get_str (my_handle, "WifiPass", config.WifiPass,(void*)&size));
	flag_success =flag_success&& (ESP_OK == nvs_get_u32 (my_handle, "WDTIME",&config.WatchDogInterval_sec));

	if(!flag_success)LoadDefaultConfig(my_handle);
	ESP_LOGI("config","SSID %s",config.SSID);
	ESP_LOGI("config","WifiPass %s",config.WifiPass);
	ESP_LOGI("config","MQTTIP %s",config.MQTTIP);
	ESP_LOGI("config","MQTTPort %d",config.MQTTPort);
	ESP_LOGI("config","MQTTAccount %s",config.MQTTAccount);
	ESP_LOGI("config","MQTTPass %s",config.MQTTPass);
	ESP_LOGI("config","WatchDogInterval_sec %d",config.WatchDogInterval_sec);
	nvs_close(my_handle);
	return;

}
void SaveConfig(){
	nvs_handle_t my_handle;
	nvs_open("storage", NVS_READWRITE, &my_handle);

	ESP_ERROR_CHECK(nvs_set_str (my_handle, "SSID", config.SSID));
	ESP_ERROR_CHECK(nvs_set_str (my_handle, "WifiPass",config.WifiPass));

	ESP_ERROR_CHECK(nvs_set_str (my_handle, "MQTTIP", config.MQTTIP));
	ESP_ERROR_CHECK(nvs_set_u16 (my_handle, "MQTTPort", config.MQTTPort));
	ESP_ERROR_CHECK(nvs_set_str (my_handle, "MQTTAccount", config.MQTTAccount));
	ESP_ERROR_CHECK(nvs_set_str (my_handle, "MQTTPass",config.MQTTPass));

	ESP_ERROR_CHECK(nvs_set_u32 (my_handle, "WDTIME", config.WatchDogInterval_sec));
	nvs_close(my_handle);
}
void ParseJsonConfig(cJSON* json){
	//cJSON* json=cJSON_ParseWithLength(jsonstr,len);
	if(json){
		float temp;
		char* temp2;
		temp2=cJSON_GetStringValue(cJSON_GetObjectItem(json,"MQTTIP"));if(temp2){
			strcpy(config.MQTTIP,temp2);
		}
		temp=cJSON_GetNumberValue(cJSON_GetObjectItem(json,"MQTTPort"));if(isfinite(temp)){
			config.MQTTPort=temp;
		}
		temp2=cJSON_GetStringValue(cJSON_GetObjectItem(json,"MQTTAccount"));if(temp2){
			strcpy(config.MQTTAccount,temp2);
		}
		temp2=cJSON_GetStringValue(cJSON_GetObjectItem(json,"MQTTPass"));if(temp2){
			strcpy(config.MQTTPass,temp2);
		}
		temp2=cJSON_GetStringValue(cJSON_GetObjectItem(json,"SSID"));if(temp2){
			strcpy(config.SSID,temp2);
		}
		temp2=cJSON_GetStringValue(cJSON_GetObjectItem(json,"WifiPass"));if(temp2){
			strcpy(config.WifiPass,temp2);
		}
		temp=cJSON_GetNumberValue(cJSON_GetObjectItem(json,"WatchDogInterval_sec"));if(isfinite(temp)){
			config.WatchDogInterval_sec=temp;
		}

		nvs_handle_t my_handle;
		nvs_open("storage", NVS_READWRITE, &my_handle);
		SaveConfig(my_handle);
		nvs_close(my_handle);
		//cJSON_Delete(json);
	}
}
