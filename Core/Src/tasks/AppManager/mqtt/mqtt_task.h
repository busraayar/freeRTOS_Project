/*
 * mqtt_agent.h
 *
 *  Created on: 3 Ağu 2026
 *      Author: busra.ayar
 */

#ifndef SRC_TASKS_MQTT_MQTT_AGENT_H_
#define SRC_TASKS_MQTT_MQTT_AGENT_H_

typedef enum
{
	MQTT_TOPIC_UNKNOWN,
	MQTT_TOPIC_LED_GREEN,
	MQTT_TOPIC_LED_BLUE,
	MQTT_TOPIC_LED_RED
} mqtt_topic_t;

void vMQTTTask( void * pvParameters );
void vTCPInitializeTask( TaskAppQueues_s* pTaskQueues );

#endif /* SRC_TASKS_MQTT_MQTT_AGENT_H_ */
