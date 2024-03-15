/*
 * network.h
 *
 *  Created on: 2021Äê11ÔÂ16ÈÕ
 *      Author: exote
 */

#ifndef MAIN_NETWORK_H_
#define MAIN_NETWORK_H_
#include "lwip/sockets.h"
#include "freertos/event_groups.h"
void InitNetwork();
extern EventGroupHandle_t networkStatus;
#define WIFI_IP_READY 1
void WaitForNetwork();
void ConnectToNetwork();
#endif /* MAIN_NETWORK_H_ */
