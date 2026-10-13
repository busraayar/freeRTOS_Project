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
CAN_TxHeaderTypeDef pxTxHeader;
CAN_RxHeaderTypeDef pxRxHeader;

TaskHandle_t CanTaskHandle = NULL;
static TaskAppQueues_s* TaskQueuesPtr = UNDEFINED_PTR;
extern CAN_HandleTypeDef hcan1;

static void canTaskFunc( void * pvParameters )
{
    ( void ) pvParameters;
    /* The parameter value is expected to be 1 as 1 is passed in the
       pvParameters value in the call to xTaskCreate() below. */

    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    uint8_t pRxData[8] = {0};

    pxTxHeader.StdId					= 0x123;
    pxTxHeader.ExtId					= 0x000;
    pxTxHeader.IDE						= CAN_ID_STD;
    pxTxHeader.RTR						= CAN_RTR_DATA;
    pxTxHeader.DLC						= 8u;
    pxTxHeader.TransmitGlobalTime		= DISABLE;

    for( ;; )
    {
        /* Task code goes here. */
    	if( HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &pxRxHeader, pRxData) == HAL_OK )
    	{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    	}

//    	CAN_SEND
//    	HAL_CAN_AddTxMessage()

    }
}

void CAN_RX_IRQHandler(void)
{
    uint8_t pTxData[8] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};
    uint32_t TxMailbox = CAN_TX_MAILBOX0;

    pxTxHeader.StdId					= 0xCCA;
    pxTxHeader.ExtId					= 0x000;
    pxTxHeader.IDE						= CAN_ID_STD;
    pxTxHeader.RTR						= CAN_RTR_DATA;
    pxTxHeader.DLC						= 8u;
    pxTxHeader.TransmitGlobalTime		= DISABLE;

	if(HAL_CAN_AddTxMessage(&hcan1, &pxTxHeader, pTxData, &TxMailbox) != HAL_OK) {
//	ERROR
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	}

	vTaskDelay(pdMS_TO_TICKS(10));
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
