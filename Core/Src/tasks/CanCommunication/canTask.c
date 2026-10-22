/*
 * canTask.c
 *
 *  Created on: 2 Tem 2026
 *      Author: busra.ayar
 */
#include "rtos.h"
#include "../../Core/Src/app_config.h"
/*
 * CAN TASK CREATE
 *
 */
TaskHandle_t CanTaskHandle = NULL;
static TaskAppQueues_s* TaskQueuesPtr = UNDEFINED_PTR;
extern CAN_HandleTypeDef hcan1;

void CAN_Send(void);

static void canTaskFunc( void * pvParameters )
{
    ( void ) pvParameters;
    /* The parameter value is expected to be 1 as 1 is passed in the
       pvParameters value in the call to xTaskCreate() below. */

    MqttMessage_s queueRxMsg;
    MqttMessage_s queueTxMsg = { .topic = MQTT_TOPIC_LED_BLUE };
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];
    CAN_RxHeaderTypeDef pxRxHeader;
    uint8_t pReceiveData[8] = {0};

    for( ;; )
    {
    	if(xQueueReceive(TaskQueuesPtr->canTaskQueue, &queueRxMsg, CAN_CONNECTION_TIMEOUT_TICK)){
    		switch(queueRxMsg.topic){
    		case MQTT_TOPIC_CAN_CMD_SEND:
    			CAN_Send();
    		case MQTT_TOPIC_CAN_CMD_SEND:
    		{
    			switch(rxData.){
    			case:
    			{

    			}
    			}
    			break;
    		}
    		default:
    			break;
    		}
    	}

		if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
		{
			/* means that there is no ongoing transfer, call Receive */
			HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxHeader, rxData);
		}
    }
}

void HAL_CAN_RxFIFO0MsgPendingCallback(){
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    if (HAL_CAN_GetRxMessage(
            &hcan1,
            CAN_RX_FIFO0,
            &rxHeader,
            rxData) == HAL_OK)
    {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
        /* message received */
    }
}

void CAN_Send(void)
{
	CAN_TxHeaderTypeDef pxTxHeader;

    uint8_t pTxData[8] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};

    pxTxHeader.DLC						= 8u;
    pxTxHeader.StdId					= 0x3CA;
    pxTxHeader.IDE						= CAN_ID_STD;
    pxTxHeader.RTR						= CAN_RTR_DATA;
    pxTxHeader.ExtId					= 0x000;
    pxTxHeader.TransmitGlobalTime		= DISABLE;

    uint32_t TxMailbox;

    HAL_StatusTypeDef status;
    status = HAL_CAN_AddTxMessage(&hcan1, &pxTxHeader, pTxData, &TxMailbox);

	if(status != HAL_OK) {
//		ERROR OCCURED
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	}

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
