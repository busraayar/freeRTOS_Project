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
const CAN_FilterTypeDef sFilterConfig;

TaskHandle_t CanTaskHandle = NULL;
static TaskAppQueues_s* TaskQueuesPtr = UNDEFINED_PTR;
static void configureFilter(CAN_FilterTypeDef *sFilterConfig);

static void canTaskFunc( void * pvParameters )
{
    ( void ) pvParameters;
    HAL_StatusTypeDef status;
    /* The parameter value is expected to be 1 as 1 is passed in the
       pvParameters value in the call to xTaskCreate() below. */

    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    CAN_RxHeaderTypeDef pxHeader = {.0};

    pxHeader.StdId 			= 0x000;
    pxHeader.IDE 			= CAN_ID_STD;

    configureFilter(&sFilterConfig);

	HAL_CAN_Start(&hcan1);

    for( ;; )
    {
        /* Task code goes here. */
    	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0,
                CAN_RxHeaderTypeDef *pHeader, uint8_t aData[])){

    	}

//    	CAN_SEND
//    	HAL_CAN_AddTxMessage()

    }
}

void configureFilter(CAN_FilterTypeDef *sFilterConfig){

	HAL_StatusTypeDef status;

    sFilterConfig->FilterBank 				= 0;
    sFilterConfig->FilterMode 				= CAN_FILTERMODE_IDMASK;
    sFilterConfig->FilterScale 				= CAN_FILTERSCALE_16BIT;
    sFilterConfig->FilterActivation 		= CAN_FILTER_ENABLE;
    sFilterConfig->FilterFIFOAssignment 	= CAN_FILTER_FIFO0;
    sFilterConfig->SlaveStartFilterBank 	= 28u;

    // Standart ID'ler 16-bit formatta sola 5 bit kaydırılmalıdır (Bit [15:5] aralığına oturması için)
	sFilterConfig->FilterIdHigh 			= (0x0100 << 5);
	sFilterConfig->FilterIdLow 				= (0x0201 << 5);
	sFilterConfig->FilterMaskIdHigh 		= (0x0FF0 << 5); // [0x0100 - 0x0109] aralığındaki mesajları kabul eder
	sFilterConfig->FilterMaskIdLow 			= (0x0FFF << 5); // 0x0201 mesajını kabul eder

	sFilterConfig->FilterIdHigh 			= 0x0102;
	sFilterConfig->FilterIdLow 				= 0x0145;
	sFilterConfig->FilterMaskIdHigh 		= 0x0FFF;
	sFilterConfig->FilterMaskIdLow 			= 0x0FFF;

	status = HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
	if(status != HAL_OK){
//		ERROR
	}

    sFilterConfig->FilterBank 				= 1;
    sFilterConfig->FilterMode 				= CAN_FILTERMODE_IDLIST;
    sFilterConfig->FilterScale 				= CAN_FILTERSCALE_16BIT;
    sFilterConfig->FilterActivation 		= CAN_FILTER_ENABLE;
    sFilterConfig->FilterFIFOAssignment 	= CAN_FILTER_FIFO0;
    sFilterConfig->SlaveStartFilterBank 	= 28u;

	sFilterConfig->FilterIdHigh 			= (0x0758 << 5);
	sFilterConfig->FilterIdLow 				= (0x0889 << 5);
	sFilterConfig->FilterMaskIdHigh 		= (0x0001 << 5); // IDLIST old için mask lar da filter görevi görür
	sFilterConfig->FilterMaskIdLow 			= (0x004C << 5);
	status = HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
	if(status != HAL_OK){
//		ERROR
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
