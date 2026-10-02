#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP.h"
#include "core_mqtt.h"
#include "core_mqtt_config.h"
#include "../Src/app_config.h"


F_TCP_UDP_Handler_t xHandler;

#define mqttBROKER_IP     "192.168.2.10"
#define mqttBROKER_PORT   ( 1883U )
#define mqttCLIENT_ID     "stm32-gateway-01"
#define mqttTOPIC         "busrayar/test"

#define NUMBER_OF_SUBSCRIPTIONS		3
#define mqttGREENLedTOPIC		"busrayar/green"
#define mqttBLUELedTOPIC		"busrayar/blue"
#define mqttREDLedTOPIC			"busrayar/red"

static uint8_t ucNetworkBuffer[ 1024 ];
static TaskAppQueues_s* TaskQueuesPtr = UNDEFINED_PTR;
static TaskHandle_t TcpTaskHandle = UNDEFINED_PTR;
static TaskHandle_t canTaskHandle = UNDEFINED_PTR;

mqtt_topic_t GetTopicId(const char *topic);
static MQTTStatus_t mqttSubscribeTopic( MQTTContext_t * pMqttContext );
static void mqttEventCallback( MQTTContext_t * pMqttContext,
                              MQTTPacketInfo_t * pPacketInfo,
                              MQTTDeserializedInfo_t * pDeserializedInfo );
uint32_t prvGetTimeMs( void )
{
    return ( uint32_t ) ( xTaskGetTickCount() * portTICK_PERIOD_MS );
}

static void prvEventCallback( MQTTContext_t * pContext,
                              MQTTPacketInfo_t * pPacketInfo,
                              MQTTDeserializedInfo_t * pDeserializedInfo )
{
    ( void ) pContext;
    ( void ) pDeserializedInfo;

    if( ( pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        printf( "Gelen MQTT mesaji var!\r\n" );
    }
}

static int32_t prvTransportSend( NetworkContext_t * pCtx,
                                 const void * pBuffer,
								 size_t xBytes )
{
    return FreeRTOS_send( pCtx->xTCPSocket, pBuffer, xBytes, 0 );
}

int32_t prvTransportRecv( NetworkContext_t * pCtx,
                        void * pBuffer,
                        size_t xBytes )
{
    int32_t lResult = FreeRTOS_recv( pCtx->xTCPSocket, pBuffer, xBytes, 0 );

    if( lResult < 0 )
    {

        return -1;
    }

    return lResult;
}

/*------------------ TCP connection ------------------*/
static Socket_t prvTcpConnect( void )
{
	Socket_t xSocket;
	struct freertos_sockaddr serverAddress = { 0 };
	struct freertos_sockaddr localAddress = { 0 };
	TickType_t receiveTimeout;
	BaseType_t result;

	xSocket = FreeRTOS_socket( FREERTOS_AF_INET,
								FREERTOS_SOCK_STREAM,
								FREERTOS_IPPROTO_TCP );

	if( xSocket == FREERTOS_INVALID_SOCKET )
	{
		return FREERTOS_INVALID_SOCKET;
	}

	receiveTimeout = pdMS_TO_TICKS( 500 );

	result = FreeRTOS_setsockopt( xSocket,
									0,
									FREERTOS_SO_RCVTIMEO,
									&receiveTimeout,
									sizeof( receiveTimeout ) );

	if( result != 0 )
	{
		printf( "setsockopt failed: %ld\r\n", ( long ) result );
		FreeRTOS_closesocket( xSocket );
		return FREERTOS_INVALID_SOCKET;
	}

	localAddress.sin_family = FREERTOS_AF_INET;
	localAddress.sin_port = 0;
	localAddress.sin_address.ulIP_IPv4 = 0;

	result = FreeRTOS_bind( xSocket,
							&localAddress,
							sizeof( localAddress ) );

	if( result != 0 )
	{
		printf( "bind failed: %ld\r\n", ( long ) result );
		FreeRTOS_closesocket( xSocket );
		return FREERTOS_INVALID_SOCKET;
	}

	serverAddress.sin_family = FREERTOS_AF_INET;
	serverAddress.sin_port =
	FreeRTOS_htons( mqttBROKER_PORT );
	serverAddress.sin_address.ulIP_IPv4 =
	FreeRTOS_inet_addr( mqttBROKER_IP );

	result = FreeRTOS_connect( xSocket,
								&serverAddress,
								sizeof( serverAddress ) );

	if( result != 0 )
	{
		printf( "connect failed: %ld\r\n", ( long ) result );
		FreeRTOS_closesocket( xSocket );
		return FREERTOS_INVALID_SOCKET;
	}

	return xSocket;
}

/*------------------ MQTT TASK ------------------*/
void vMQTTTask( void * pvParameters )
{
    ( void ) pvParameters;

    MqttMessage_s queueRxMsg;
    static NetworkContext_t xNetworkCtx;
    static TransportInterface_t xTransport;
    static MQTTContext_t xMqttCtx;
    MQTTFixedBuffer_t xBuffer = { .pBuffer = ucNetworkBuffer,
                                  .size    = sizeof( ucNetworkBuffer ) };

    for( ;; )
    {
        Socket_t xSocket = prvTcpConnect();
        if( xSocket == NULL )
        {
            printf( "TCP connection error\r\n" );
            vTaskDelay( pdMS_TO_TICKS( 5000 ) );
            continue;
        }

	xNetworkCtx.xTCPSocket = xSocket;
	xTransport.send = prvTransportSend;
	xTransport.pNetworkContext = &xNetworkCtx;
	xTransport.recv = prvTransportRecv;

	if( MQTT_Init( &xMqttCtx,
				   &xTransport,
				   prvGetTimeMs,
				   ( MQTTEventCallback_t )mqttEventCallback,
				   &xBuffer ) != MQTTSuccess )
	{
		FreeRTOS_closesocket( xSocket );
		vTaskDelay( pdMS_TO_TICKS( 5000 ) );
	}

	MQTTConnectInfo_t xConnect = { 0 };
	xConnect.cleanSession = true;
	xConnect.keepAliveSeconds = 60;
	xConnect.pClientIdentifier = mqttCLIENT_ID;
	xConnect.clientIdentifierLength = ( uint16_t ) strlen( mqttCLIENT_ID );

		bool xSessionPresent;
		MQTTStatus_t status;
		status = MQTT_Connect(&xMqttCtx, &xConnect, NULL, 2000, &xSessionPresent, NULL, NULL);
		if(status != MQTTSuccess){
			printf( "MQTT CONNECT ERROR: Code = %d (0x%X)\r\n", status, status );

			FreeRTOS_closesocket( xSocket );
			xSocket = FREERTOS_INVALID_SOCKET;
			vTaskDelay( pdMS_TO_TICKS( 5000 ) );
		}

		printf( "MQTT CONNECTED!\r\n" );

		TickType_t xLastPublish = xTaskGetTickCount();

		for( ;; )
		{
			static MQTTStatus_t stat = MQTTSuccess;

			stat = mqttSubscribeTopic(&xMqttCtx);
			if(stat != MQTTSuccess){
				printf("Subscribe Islemi Basarisiz!!!");
			}

			stat = MQTT_ProcessLoop( &xMqttCtx );
			if( stat == MQTTNeedMoreBytes){

			}else if(stat != MQTTSuccess){
				FreeRTOS_closesocket( xSocket );
				xSocket = FREERTOS_INVALID_SOCKET;
				printf( "Baglanti koptu\r\n" );
				break;
			}

			if( ( xTaskGetTickCount() - xLastPublish ) >= pdMS_TO_TICKS( 5000 ) )
			{
				xLastPublish = xTaskGetTickCount();

				char pcPayload[ 64 ];
				int iLen = snprintf( pcPayload, sizeof( pcPayload ),
									 "{\"tick\": %lu}",
									 ( unsigned long ) xLastPublish );

				MQTTPublishInfo_t xPub = { 0 };
				xPub.qos             = MQTTQoS0;
				xPub.pTopicName      = mqttTOPIC;
				xPub.topicNameLength = ( uint16_t ) strlen( mqttTOPIC );
				xPub.pPayload        = pcPayload;
				xPub.payloadLength   = ( size_t ) iLen;

				if( MQTT_Publish( &xMqttCtx,
								  &xPub,
								  0,
								  NULL ) == MQTTSuccess )
					printf( "Publish OK\r\n" );
			}
			if(xQueueReceive(TaskQueuesPtr->mqttTaskQueue, &queueRxMsg, MQTT_TASK_QUEUE_RECEIVE_TIMEOUT_MS))
			{

				switch (queueRxMsg.topic)
				{
					case MQTT_TOPIC_LED_GREEN:
						//PA5 is a green led for user
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,  GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_RESET);
						break;
					case MQTT_TOPIC_LED_BLUE:
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,  GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
						break;
					case MQTT_TOPIC_LED_RED:
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,  GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,  GPIO_PIN_RESET);
						break;
					default:
						break;
				}
			}


			vTaskDelay( pdMS_TO_TICKS( 10 ) );
		}

		FreeRTOS_closesocket( xSocket );
}
}

void vTCPInitializeTask( TaskAppQueues_s* pTaskQueues ){
	BaseType_t xReturned;

	TaskQueuesPtr = pTaskQueues;

	xReturned = xTaskCreate(
			vMQTTTask,
			"TCP TASK",
			MQTT_TASK_STACK_SIZE,
			( void * ) pTaskQueues,
			MQTT_TASK_PRIORITY,
			&TcpTaskHandle);

    if( xReturned == pdPASS )
    {
        /* The task was created. Use the task's handle to delete the task. */
//        vTaskDelete( CanTaskHandle );
    }
}

mqtt_topic_t GetTopicId(const char *topic)
{
	mqtt_topic_t ret = MQTT_TOPIC_UNKNOWN;

	if(strcmp(topic, "busrayar/green") == 0){
		ret = MQTT_TOPIC_LED_GREEN;
	}else if(strcmp(topic, "busrayar/blue") == 0){
		ret = MQTT_TOPIC_LED_BLUE;
	}else if(strcmp(topic, "busrayar/red") == 0){
		ret = MQTT_TOPIC_LED_RED;
	}

	return ret;
}


static MQTTStatus_t mqttSubscribeTopic( MQTTContext_t * pMqttContext )
{
    MQTTStatus_t xResult = MQTTSuccess;

    uint8_t retryAttempsForSubscribeTopic = 0;

    MQTTSubscribeInfo_t xMQTTSubscription[ NUMBER_OF_SUBSCRIPTIONS ];
    bool xFailedSubscribeToTopic = false;

    memset( ( void * ) &xMQTTSubscription, 0x00, sizeof( xMQTTSubscription ) );

    xMQTTSubscription[0].qos = MQTTQoS0;
    xMQTTSubscription[0].pTopicFilter = mqttGREENLedTOPIC;
    xMQTTSubscription[0].topicFilterLength = strlen( mqttGREENLedTOPIC );

    xMQTTSubscription[1].qos = MQTTQoS0;
    xMQTTSubscription[1].pTopicFilter = mqttBLUELedTOPIC;
    xMQTTSubscription[1].topicFilterLength = strlen( mqttBLUELedTOPIC );

    xMQTTSubscription[2].qos = MQTTQoS0;
    xMQTTSubscription[2].pTopicFilter = mqttREDLedTOPIC;
    xMQTTSubscription[2].topicFilterLength = strlen( mqttREDLedTOPIC );

    do {
    	uint16_t packetId = MQTT_GetPacketId(pMqttContext);

    	MQTTPropBuilder_t propertyBuilder;
		uint8_t propertyBuffer[ 100 ];
		size_t propertyBufferLength = sizeof( propertyBuffer );
		xResult = MQTTPropertyBuilder_Init( &propertyBuilder, propertyBuffer, propertyBufferLength );

    	xResult = MQTT_Subscribe(pMqttContext, xMQTTSubscription, NUMBER_OF_SUBSCRIPTIONS, packetId, &propertyBuilder);
    	retryAttempsForSubscribeTopic++;

    	if( xResult == MQTTSuccess ){
    		xFailedSubscribeToTopic = true;
    	}

	} while (retryAttempsForSubscribeTopic < NUMBER_OF_SUBSCRIPTIONS &&
			xFailedSubscribeToTopic);

    return xResult;

}

static void mqttEventCallback( MQTTContext_t * pMqttContext,
                              MQTTPacketInfo_t * pPacketInfo,
                              MQTTDeserializedInfo_t * pDeserializedInfo )
{
	( void ) pMqttContext;


	switch( pPacketInfo->type )
    {
        case MQTT_PACKET_TYPE_CONNACK:
            break;

        case MQTT_PACKET_TYPE_PUBLISH:
        {
        	mqtt_topic_t topicID;
        	topicID = GetTopicId(pDeserializedInfo->pPublishInfo->pTopicName);

        	MqttMessage_s mqttRxMessage = {0};

        	mqttRxMessage.topic = topicID;
        	mqttRxMessage.payloadLength = pDeserializedInfo->pPublishInfo->payloadLength;
        	memcpy(mqttRxMessage.payload, pDeserializedInfo->pPublishInfo->pPayload, mqttRxMessage.payloadLength);

        	xQueueSend(TaskQueuesPtr->mqttTaskQueue, &mqttRxMessage, 0);

        	break;
        }
        case MQTT_PACKET_TYPE_PUBACK:
            /* Incoming publish verisi */
            break;

        case MQTT_PACKET_TYPE_SUBACK:
            printf( "SUBACK alindi.\r\n" );
            break;

        case MQTT_PACKET_TYPE_PINGRESP:
            printf( "PINGRESP alindi.\r\n" );
            break;

        default:

            break;
    }
}
