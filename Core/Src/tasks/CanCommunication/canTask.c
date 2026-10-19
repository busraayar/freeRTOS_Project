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
static TaskAppQueues_s* TaskQueuesPtr = UNDEFINED_PTR;
extern CAN_HandleTypeDef hcan1;

static void canTaskFunc( void * pvParameters )
{
    ( void ) pvParameters;
    /* The parameter value is expected to be 1 as 1 is passed in the
       pvParameters value in the call to xTaskCreate() below. */

//    configASSERT( pvParameters != NULL );
    CAN_RxHeaderTypeDef pxRxHeader;
    uint8_t pReceiveData[8] = {0};

    HAL_StatusTypeDef status;

    for( ;; )
    {
    	if( !HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) ) ;
        /* Task code goes here. */
    	if( HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &pxRxHeader, pReceiveData) == HAL_OK )
    	{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    	}

        vTaskDelay(pdMS_TO_TICKS(10));

//    	CAN_SEND
//    	HAL_CAN_AddTxMessage()

    }
}


void CAN_Send(void)
{
	CAN_TxHeaderTypeDef pxTxHeader;

    uint8_t pTxData[8] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};

    pxTxHeader.DLC						= 8u;
    pxTxHeader.StdId					= 0xCCA;
    pxTxHeader.IDE						= CAN_ID_STD;
    pxTxHeader.RTR						= CAN_RTR_DATA;
    pxTxHeader.ExtId					= 0x000;
    pxTxHeader.TransmitGlobalTime		= DISABLE;

    uint32_t TxMailbox;

    HAL_StatusTypeDef status;
    status = HAL_CAN_AddTxMessage(&hcan1, &pxTxHeader, pTxData, &TxMailbox);

	if(status != HAL_OK) {
//	ERROR
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	}

	while( HAL_CAN_IsTxMessagePending(&hcan1, TxMailbox) );

	return;
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
