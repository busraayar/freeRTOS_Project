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

TaskHandle_t CanTaskHandle = NULL;

static void canTaskFunc( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
       pvParameters value in the call to xTaskCreate() below. */

    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    for( ;; )
    {
        /* Task code goes here. */
    }
}
void CanTaskCreate( void ){
	BaseType_t xReturned;

	xReturned = xTaskCreate(
			canTaskFunc,
			"CAN TASK",
			CAN_TASK_STACK_SIZE,
			( void * ) 1,
			CAN_TASK_PRIORITY,
			&CanTaskHandle );

    if( xReturned == pdPASS )
    {
        /* The task was created. Use the task's handle to delete the task. */
//        vTaskDelete( CanTaskHandle );
    }
}
