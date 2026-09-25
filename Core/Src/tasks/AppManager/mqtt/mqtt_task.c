#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP.h"
#include "core_mqtt.h"
#include "core_mqtt_config.h"

TaskHandle_t TcpTaskHandle = NULL;
F_TCP_UDP_Handler_t xHandler;

#define mqttBROKER_IP     "192.168.2.10"
//#define mqttBROKER_IP     "broker.hivemq.com"
#define mqttBROKER_PORT   ( 1883U )
#define mqttCLIENT_ID     "stm32-gateway-01"
//#define mqttTOPIC         "gateway/test"
#define mqttTOPIC         "busrayar/test"

static uint8_t ucNetworkBuffer[ 1024 ];

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
    ( void ) pContext; ( void ) pDeserializedInfo;

    if( ( pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        printf( "Gelen MQTT mesaji var!\r\n" );
    }
}

static void mqttEventCallback( MQTTContext_t * pMqttContext,
                              MQTTPacketInfo_t * pPacketInfo,
                              MQTTDeserializedInfo_t * pDeserializedInfo )
{
    /* Null pointer kontrolü */
    if( ( pMqttContext == NULL ) || ( pPacketInfo == NULL ) || ( pDeserializedInfo == NULL ) )
    {
        return;
    }

    /* Gelen paket tiplerini ayrıştırın */
    switch( pPacketInfo->type )
    {
        case MQTT_PACKET_TYPE_CONNACK:
            printf( "CONNACK paketi alindi. Baglanti basarili!\r\n" );
            break;

        case MQTT_PACKET_TYPE_PUBLISH:
            /* Incoming publish verisi */
            break;

        case MQTT_PACKET_TYPE_SUBACK:
            printf( "SUBACK alindi.\r\n" );
            break;

        case MQTT_PACKET_TYPE_PINGRESP:
            printf( "PINGRESP alindi.\r\n" );
            break;

        default:
            //led yak;

            break;
    }
}
static int32_t prvTransportSend( NetworkContext_t * pCtx,
                                 const void * pBuffer, size_t xBytes )
{
    return FreeRTOS_send( pCtx->xTCPSocket, pBuffer, xBytes, 0 );
}

int32_t prvTransportRecv( NetworkContext_t * pNetworkContext,
                        void * pBuffer,
                        size_t bytesToRecv )
{
    int32_t socketStatus;

    int32_t lResult = FreeRTOS_recv( pNetworkContext->xTCPSocket, pBuffer, bytesToRecv, 0 );

    if( lResult < 0 )
    {
        /* Soket hatası */
        return -1;
    }

    return lResult; /* Okunan bayt sayısı (0 veya daha fazla) */
}

/*------------------ TCP connection ------------------*/
static Socket_t prvTcpConnect( void )
{
    Socket_t xSocket = FreeRTOS_socket( FREERTOS_AF_INET,
                                        FREERTOS_SOCK_STREAM,
                                        FREERTOS_IPPROTO_TCP );

    if( xSocket == FREERTOS_INVALID_SOCKET ) return NULL;

    struct freertos_sockaddr xServer = { 0 };
    xServer.sin_family = FREERTOS_AF_INET;
    xServer.sin_port = FreeRTOS_htons( mqttBROKER_PORT );
    xServer.sin_address.ulIP_IPv4 = FreeRTOS_inet_addr( mqttBROKER_IP );

    TickType_t xReceiveTimeout = pdMS_TO_TICKS( 500 );

    FreeRTOS_setsockopt( xSocket,
                         0,
                         FREERTOS_SO_RCVTIMEO,
                         &xReceiveTimeout,
                         sizeof( xReceiveTimeout ) );

    if(FreeRTOS_connect( xSocket, &xServer, sizeof( xServer ) ) != 0)
    {
    	return NULL;
    }
    return xSocket;
}

/*------------------ MQTT TASK ------------------*/
void vMQTTTask( void * pvParameters )
{
    ( void ) pvParameters;

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
            continue;
        }

        MQTTConnectInfo_t xConnect = { 0 };
        xConnect.cleanSession = true;
        xConnect.keepAliveSeconds = 60;
        xConnect.pClientIdentifier = mqttCLIENT_ID;
        xConnect.clientIdentifierLength = ( uint16_t ) strlen( mqttCLIENT_ID );

        bool xSessionPresent;
        MQTTStatus_t status;
        status = MQTT_Connect(&xMqttCtx, &xConnect, NULL, 5000, &xSessionPresent, NULL, NULL);
        if(status != MQTTSuccess){
        	printf( "MQTT CONNECT ERROR: Code = %d (0x%X)\r\n", status, status );

			FreeRTOS_closesocket( xSocket );
			xSocket = FREERTOS_INVALID_SOCKET;
			vTaskDelay( pdMS_TO_TICKS( 5000 ) );
			continue;
        }

        printf( "MQTT CONNECTED!\r\n" );

        TickType_t xLastPublish = xTaskGetTickCount();

        for( ;; )
        {
            if( MQTT_ProcessLoop( &xMqttCtx ) != MQTTSuccess )
            {
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
                xPub.payloadLength   = (size_t)strlen("Hello World");//( size_t ) iLen;

                if( MQTT_Publish( &xMqttCtx,
                				  &xPub,
								  0,
								  NULL ) == MQTTSuccess )
                    printf( "Publish OK\r\n" );
            }

            vTaskDelay( pdMS_TO_TICKS( 10 ) );
        }

        FreeRTOS_closesocket( xSocket );
    }
}

void vTCPInitializeTask( void ){
	BaseType_t xReturned;

	xReturned = xTaskCreate(
			vMQTTTask,
			"TCP TASK",
			MQTT_TASK_STACK_SIZE,
			( void * )1,
			MQTT_TASK_PRIORITY,
			&TcpTaskHandle);

    if( xReturned == pdPASS )
    {
        /* The task was created. Use the task's handle to delete the task. */
//        vTaskDelete( CanTaskHandle );
    }
}


