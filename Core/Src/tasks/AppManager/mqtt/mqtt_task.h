/*
 * mqtt_agent.h
 *
 *  Created on: 3 Ağu 2026
 *      Author: busra.ayar
 */

#ifndef SRC_TASKS_MQTT_MQTT_AGENT_H_
#define SRC_TASKS_MQTT_MQTT_AGENT_H_

#define RECEIVED_CAN_ID_FROM_MQTT			"canId"

typedef enum
{
	MQTT_TOPIC_UNKNOWN,
	MQTT_TOPIC_CAN_CMD_SEND,
	MQTT_TOPIC_GET_STATUS,
	MQTT_TOPIC_SETTINGS
} mqtt_topic_t;

typedef struct{

}mqttJsonFormat;

void vMQTTTask( void * pvParameters );
void vTCPInitializeTask( TaskAppQueues_s* pTaskQueues );

#endif /* SRC_TASKS_MQTT_MQTT_AGENT_H_ */
