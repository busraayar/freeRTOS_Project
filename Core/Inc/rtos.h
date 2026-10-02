/*
 * rtos.h
 *
 *  Created on: May 4, 2026
 *      Author: busra.ayar
 */

#ifndef INC_RTOS_H_
#define INC_RTOS_H_

#include <string.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define BufferLen				1536u
#define UNDEFINED_PTR			((void*)(0xFFFFFFFF))

/* Task Priorities */
#define CAN_TASK_PRIORITY					( 1 )
#define APP_TASK_PRIORITY					( 2 )
#define MQTT_TASK_PRIORITY					( 3 )

#if configSUPPORT_DYNAMIC_ALLOCATION
#define CAN_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE//configMINIMAL_STACK_SIZE
#define APP_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE
#define MQTT_TASK_STACK_SIZE				1024
#define ETH_TASK_STACK_SIZE					300

#else
#define CAN_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE//configMINIMAL_STACK_SIZE
#define APP_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE
#define ETH_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE
#define MQTT_TASK_STACK_SIZE				configMINIMAL_STACK_SIZE
#endif

#define TCP_CONNECTION_TIMEOUT_TICK				( 5000/portTICK_PERIOD_MS )
#define MQTT_TASK_QUEUE_RECEIVE_TIMEOUT_MS		( 5/portTICK_PERIOD_MS )

#define TASK_QUEUE_LENGTH_MQTT						24
#define TASK_QUEUE_LENGTH_CAN						24
#define MAX_QUEUE_MSG_LEN_IN_BYTES					24

typedef struct {
	QueueHandle_t mqttTaskQueue;
	QueueHandle_t canTaskQueue;
}TaskAppQueues_s;

#endif /* INC_RTOS_H_ */
