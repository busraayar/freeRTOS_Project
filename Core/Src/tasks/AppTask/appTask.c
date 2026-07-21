/*
 * appTask.c
 *
 *  Created on: 2 Tem 2026
 *      Author: busra.ayar
 */

#include "rtos.h"

TaskHandle_t AppTaskHandle = NULL;

static void AppTaskFunction( void * pvParameters ){

    /* The parameter value is expected to be 1 as 1 is passed in the
       pvParameters value in the call to xTaskCreate() below. */

    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );


	for(;;){
		/* Task code goes here. */
	}
}

void AppTaskCreate( void ){

	BaseType_t xReturned;

	xReturned = xTaskCreate(
			AppTaskFunction,
			"APP TASK",
			APP_TASK_STACK_SIZE,
			( void * )1,
			APP_TASK_PRIORITY,
			&AppTaskHandle);

	if( xReturned == pdPASS ){
		//task created success
	}

}
