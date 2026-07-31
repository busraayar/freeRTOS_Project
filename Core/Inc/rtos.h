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

/* Task Priorities */
/* #define tskIDLE_PRIORITY 				0 */
//#define ipconfigIP_TASK_PRIORITY			( 3 )
#define CAN_TASK_PRIORITY					( 1 )
#define APP_TASK_PRIORITY					( 4 )
#define MQTT_TASK_PRIORITY					( 3 )

#if configSUPPORT_DYNAMIC_ALLOCATION
#define CAN_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE//configMINIMAL_STACK_SIZE
#define APP_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE
#define ETH_TASK_STACK_SIZE					300
#define MQTT_TASK_STACK_SIZE				1024

#else
#define CAN_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE//configMINIMAL_STACK_SIZE
#define APP_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE
#define ETH_TASK_STACK_SIZE					configMINIMAL_STACK_SIZE
#define MQTT_TASK_STACK_SIZE				configMINIMAL_STACK_SIZE
#endif

#define TCP_CONNECTION_TIMEOUT_TICK			5000/portTICK_PERIOD_MS

#endif /* INC_RTOS_H_ */
