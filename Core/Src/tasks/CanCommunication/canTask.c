/*
 * canTask.c
 *
 *  Created on: 2 Tem 2026
 *      Author: busra.ayar
 */
#include "rtos.h"
#include <stdlib.h>
#include "../../Core/Src/app_config.h"
/*
 * CAN TASK CREATE
 *
 */
TaskHandle_t CanTaskHandle = NULL;
static TaskAppQueues_s* TaskQueuesPtr = UNDEFINED_PTR;
CAN_TxHeaderTypeDef   TxHeader;
uint8_t               TxData[8];
uint32_t              TxMailbox;
extern CAN_HandleTypeDef hcan1;
uint8_t rxData[8];

void CAN_Send(CanMessage_s* data);
uint32_t parseCmdFromJson(char* data, uint8_t datalen, char* cmndKey);

static void canTaskFunc( void * pvParameters )
{
    ( void ) pvParameters;
    /* The parameter value is expected to be 1 as 1 is passed in the
       pvParameters value in the call to xTaskCreate() below. */

    AppMessage_t queueRxMsg;

    for( ;; )
    {
    	if(xQueueReceive(TaskQueuesPtr->canTaskQueue, &queueRxMsg, CAN_CONNECTION_TIMEOUT_TICK)){
    		switch(queueRxMsg.TypeId){
    		case MQTT_SUBS_MSG_RCV:
    		{
    			MqttMessage_s* mqttReceivedMsg = &queueRxMsg.mqttMsg;

    			switch (mqttReceivedMsg->topic) {
					case MQTT_TOPIC_CAN_CMD_SEND:
//						do something
						CanMessage_s canMsg = {0};

						canMsg.canId = parseCmdFromJson(mqttReceivedMsg->payload , mqttReceivedMsg->payloadLength, (char*)canMsg.data);

						canMsg.dataLen = mqttReceivedMsg->payloadLength;

						if(canMsg.dataLen > 8)
						{
						    canMsg.dataLen = 8;
						}

//						memcpy(canMsg.data,
//						       mqttReceivedMsg->payload,
//						       canMsg.dataLen);

						CAN_Send(&canMsg);

						break;
					default:
						break;
				}
    			break;
    		}
    		case CAN_MSG_RECEIVED:
    		{
    			CanMessage_s* canReceivedMsg = &queueRxMsg.canMsg;

				switch (canReceivedMsg->canId) {
					case 0x100:
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
						break;
					case 0x111:
						break;
					case 0x002:
						break;
					default:
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
						break;
				}
				break;

    		}
    		case MQTT_PUBL_MSG_SEND:
    			break;
    		default:
    			break;
    		}
    	}
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){

	CAN_RxHeaderTypeDef rxHeader;
    AppMessage_t canMsg = {0};
    uint8_t rxData[8];
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (HAL_CAN_GetRxMessage(
            &hcan1,
            CAN_RX_FIFO0,
            &rxHeader,
            rxData) == HAL_OK)
    {

		canMsg.TypeId = CAN_MSG_RECEIVED;

		memcpy(&canMsg.canMsg.data, &rxData, sizeof(rxData));
		canMsg.canMsg.canId = rxHeader.StdId;
		canMsg.canMsg.dataLen = rxHeader.DLC;

//		xQueueSend(TaskQueuesPtr->canTaskQueue, &canMsg, CAN_CONNECTION_TIMEOUT_TICK);

        xQueueSendFromISR( TaskQueuesPtr->canTaskQueue, &canMsg, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        /* message received */
    }
}

void CAN_Send(CanMessage_s* data)
{

    TxHeader.StdId 				= data->canId;
    TxHeader.RTR 				= CAN_RTR_DATA;
    TxHeader.IDE 				= CAN_ID_STD;
    TxHeader.DLC 				= data->dataLen;
    TxHeader.TransmitGlobalTime = DISABLE;
    memcpy(TxData, data->data, data->dataLen);

    if(HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
	  /* Transmission request Error */
	  // İsteğe bağlı hata yönetimi
	}

	return;
}

uint32_t parseCmdFromJson(char* data, uint8_t datalen, char* cmndKey)
{
    if (data == NULL || datalen == 0) return 0;

    uint32_t parsedCanId = 0;
    char canIdStr[16] = {0};

    char* canIdKey = strstr(data, RECEIVED_CAN_ID_FROM_MQTT);
    cmndKey = strstr(data, "cmnd");

    if (canIdKey != NULL) {
    	canIdKey = strchr(canIdKey, '"');
    	canIdKey = strchr(canIdKey + 1, '"');
    	if (canIdKey != NULL) {
    		canIdKey++;
        	parsedCanId = strtol(canIdKey, NULL, 16);

    	}

    }

    return parsedCanId;

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
