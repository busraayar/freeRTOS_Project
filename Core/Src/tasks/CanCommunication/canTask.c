/*
 * canTask.c
 *
 *  Created on: 2 Tem 2026
 *      Author: busra.ayar
 */
#include "rtos.h"
/*
 * CAN TASK CREATE
 *
 */
CAN_RxHeaderTypeDef pxHeader = {0};

TaskHandle_t CanTaskHandle = NULL;
static TaskAppQueues_s* TaskQueuesPtr = UNDEFINED_PTR;
extern CAN_HandleTypeDef hcan1;

static void canTaskFunc( void * pvParameters )
{
    ( void ) pvParameters;
    /* The parameter value is expected to be 1 as 1 is passed in the
       pvParameters value in the call to xTaskCreate() below. */

    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    uint8_t pData[8];

    for( ;; )
    {
        /* Task code goes here. */
    	if( HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &pxHeader, pData) == HAL_OK ){

    	}

//    	CAN_SEND
//    	HAL_CAN_AddTxMessage()

    }
}

void CanTaskCreate(  TaskAppQueues_s* pTaskQueues ){
	BaseType_t xReturned;

	TaskQueuesPtr = pTaskQueues;

	xReturned = xTaskCreate(
			canTaskFunc,
			"CAN TASK",
			CAN_TASK_STACK_SIZE,
			( void * ) pTaskQueues,
			CAN_TASK_PRIORITY,
			&CanTaskHandle );

    if( xReturned == pdPASS )
    {
        /* The task was created. Use the task's handle to delete the task. */
//        vTaskDelete( CanTaskHandle );
    }
}
