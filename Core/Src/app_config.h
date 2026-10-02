/*
 * app_config.h
 *
 *  Created on: 1 Eki 2026
 *      Author: busra.ayar
 */

#ifndef SRC_APP_CONFIG_H_
#define SRC_APP_CONFIG_H_

#include "rtos.h"
#include "mqtt_task.h"

#define MAX_MQTT_MSG_LENGTH							64

typedef struct
{
	mqtt_topic_t topic;
	char payload[MAX_MQTT_MSG_LENGTH];
	uint16_t payloadLength;
}MqttMessage_s;

typedef struct
{
	uint32_t FrameId;
	uint8_t Length;
	uint8_t Data[8];
}AppMessage_t;


#endif /* SRC_APP_CONFIG_H_ */
