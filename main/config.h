#include "stdint.h"
#include "cJSON.h"
#ifndef __config__
#define __config__
typedef  struct {
	//≈‰÷√
	char SSID[64];
	char WifiPass[64];

	char MQTTIP[64];
	uint16_t MQTTPort;
	char MQTTAccount[64];
	char MQTTPass[64];

	uint32_t WatchDogInterval_sec;

}config_t;
extern config_t config;
void LoadDefaultConfig();
void LoadConfig();
void SaveConfig();
void ParseJsonConfig(cJSON* json);
#endif
