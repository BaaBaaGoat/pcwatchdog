
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt.h"
#include "config.h"
extern uint8_t mac[6];
static esp_mqtt_client_handle_t mqttclient;
uint8_t Flag_MQTTConnLost=0;
char* MAC2STR2(const uint8_t* x){
	static char buff[20];
	sprintf(buff,"%02X:%02X:%02X:%02X:%02X:%02X",x[0],x[1],x[2],x[3],x[4],x[5]);
	return buff;
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:{
        	esp_mqtt_client_subscribe(client,"/pcwatchdog",0);
            Flag_MQTTConnLost=0;

        }break;
        case MQTT_EVENT_DISCONNECTED:
        	ESP_LOGE("MQTT", "MQTT_EVENT_DISCONNECTED");
        	Flag_MQTTConnLost++;
        	assert(Flag_MQTTConnLost<3);
        	break;
        case MQTT_EVENT_DATA:{
        	OnMQTTArrive(event->topic,event->topic_len,event->data,event->data_len);
        }break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE("MQTT", "MQTT_EVENT_ERROR");break;
        default:
            ESP_LOGI("MQTT", "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    mqtt_event_handler_cb(event_data);
}
void Init_MQTT(){
    esp_mqtt_client_config_t mqtt_cfg = {
       .host = config.MQTTIP,
       .port = config.MQTTPort,
	    .client_id=MAC2STR2(mac),
	    .username=config.MQTTAccount,
	    .password=config.MQTTPass,
		.disable_auto_reconnect=0
   };
   mqttclient = esp_mqtt_client_init(&mqtt_cfg);
   ESP_ERROR_CHECK(esp_mqtt_client_start(mqttclient));

   esp_mqtt_client_register_event(mqttclient, ESP_EVENT_ANY_ID, mqtt_event_handler, mqttclient);
}
void PushDataToMQTT(char* topic,char* data,int len){
	if(mqttclient != NULL)esp_mqtt_client_publish(mqttclient, topic, data, len,0,0);
}
