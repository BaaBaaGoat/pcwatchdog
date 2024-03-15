
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "stdbool.h"
#include "esp_event.h"
#include "lwip/sockets.h"
#include "config.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "network.h"
#include "esp_netif.h"
#include "lwip/netif.h"
#include "bsp.h"
#include "esp_event.h"
#define TEST_WIFI_WAITTIME 3000
TaskHandle_t tasknetwork_handle;
EventGroupHandle_t networkStatus=NULL;
esp_netif_t* esp_netif_WIFI=NULL;
char macstr[64];
uint8_t mac[6];
static void on_wifi_disconnected(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
	esp_wifi_connect();
}
static void on_wifi_lost_ip(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
	ESP_LOGI("NETWORK", "Lost IPv4 from  WIFI");
	xEventGroupClearBits(networkStatus,WIFI_IP_READY);
}
static void on_got_ip(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const char* netif_desc=esp_netif_get_desc(event->esp_netif);
    ESP_LOGI("NETWORK", "Got IPv4 from  interface \"%s\":%d.%d.%d.%d", netif_desc, IP2STR(&event->ip_info.ip));
	if(0==strcmp(netif_desc,"WIFI")){
		xEventGroupSetBits(networkStatus,WIFI_IP_READY);
	}
}

void WaitForNetwork(){
	xEventGroupWaitBits(networkStatus,WIFI_IP_READY,false,false,portMAX_DELAY );

}
void InitNetwork(){

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(esp_netif_init());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_netif_inherent_config_t esp_wifi_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
    esp_wifi_netif_config.if_desc = "WIFI";
    esp_wifi_netif_config.route_prio = 2;

    esp_netif_WIFI = esp_netif_create_wifi(WIFI_IF_STA, &esp_wifi_netif_config);
    esp_wifi_set_default_wifi_sta_handlers();
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    esp_wifi_get_mac(WIFI_IF_STA, mac);
	sprintf((char*)macstr,"%02X%02X%02X%02X%02X%02X",(uint32_t)mac[0],(uint32_t)mac[1],(uint32_t)mac[2],(uint32_t)mac[3],(uint32_t)mac[4],(uint32_t)mac[5]);

	networkStatus=xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_LOST_IP, &on_wifi_lost_ip, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnected, NULL));
    ESP_LOGI("NETWORK","Init Finish");

}


void ConnectToNetwork(){
	// 启动WIFI 等待60s尝试连接出厂测试WIFI 连上则退出
	wifi_config_t wifi_config;
		// 启动WIFI和以太网 连接客户WIFI
		esp_wifi_disconnect();
		esp_wifi_stop();
		memset((void*)&wifi_config,0,sizeof(wifi_config));
		strcpy((void*)wifi_config.sta.ssid,config.SSID);
		strcpy((void*)wifi_config.sta.password,config.WifiPass);
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
		esp_wifi_start();
		esp_wifi_connect();
		ESP_LOGI("NETWORK","NETWORK use DHCP");
		if(xEventGroupWaitBits(networkStatus,WIFI_IP_READY,false,false,TEST_WIFI_WAITTIME )) return;
}
