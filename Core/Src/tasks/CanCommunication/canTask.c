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

    AppMessage_t queueRxMsg;
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    for( ;; )
    {
    	if(xQueueReceive(TaskQueuesPtr->canTaskQueue, &queueRxMsg, CAN_CONNECTION_TIMEOUT_TICK)){
    		switch(queueRxMsg.FrameId){
    		case MQTT_SUBS_MSG_RCV:
    		{
    			MqttMessage_s* mqttReceivedMsg = (MqttMessage_s*)queueRxMsg.Data;

    			switch (mqttReceivedMsg->topic) {
					case MQTT_TOPIC_CAN_CMD_SEND:
//						do something
						CAN_Send();

						break;
					default:
						break;
				}
    		}
    		case CAN_MSG_RECEIVED:
    			break;
    		case MQTT_PUBL_MSG_SEND:
    			break;
    		default:
    			break;
    		}
    	}

		if( HAL_CAN_GetRxFifoFillLevel( &hcan1, CAN_TX_MAILBOX0 ) )
		{
			/* means that there is no ongoing transfer, call Receive */
			HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxHeader, rxData);
		}
    }
}

void HAL_CAN_RxFIFO0MsgPendingCallback(){

	CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];
    AppMessage_t canMsg = {0};

    if (HAL_CAN_GetRxMessage(
            &hcan1,
            CAN_RX_FIFO0,
            &rxHeader,
            rxData) == HAL_OK)
    {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

		canMsg.FrameId = CAN_MSG_RECEIVED;
		memcpy(&canMsg.Data, &rxData, sizeof(rxData));

		xQueueSend(TaskQueuesPtr->canTaskQueue, &canMsg, CAN_CONNECTION_TIMEOUT_TICK);

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
