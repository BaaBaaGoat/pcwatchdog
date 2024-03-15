#include <stdint.h>
extern void OnMQTTArrive(char* topic,int topiclen,char* data,int datalen);
void Init_MQTT();
char* ATE_MAC2STR(const uint8_t* x);
void PushDataToMQTT(char* topic,char* data,int len);
