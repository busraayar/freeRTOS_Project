#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP.h"
#include "core_mqtt.h"
#include "core_mqtt_config.h"

/*------------------ AYARLAR ------------------*/
#define mqttBROKER_IP     "192.168.2.10"
#define mqttBROKER_PORT   ( 1883U )
#define mqttCLIENT_ID     "stm32-gateway-01"
#define mqttTOPIC         "gateway/test"

/*------------------ MQTT buffer ------------------*/
static uint8_t ucNetworkBuffer[ 1024 ];
static uint8_t pPropertiesBuffer[ 1024 ];

/*------------------ Zaman fonksiyonu ------------------*/
static uint32_t prvGetTimeMs( void )
{
    return ( uint32_t ) ( xTaskGetTickCount() * portTICK_PERIOD_MS );
}

/*------------------ Olay callback'i ------------------*/
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

/*------------------ Transport: GÖNDER ------------------*/
static int32_t prvTransportSend( NetworkContext_t * pCtx,
                                 const void * pBuffer, size_t xBytes )
{
    return FreeRTOS_send( pCtx->xTCPSocket, pBuffer, xBytes, 0 );
}

/*------------------ Transport: AL ------------------*/
static int32_t prvTransportRecv( NetworkContext_t * pCtx,
                                 void * pBuffer, size_t xBytes )
{
    int32_t lResult = FreeRTOS_recv( pCtx->xTCPSocket, pBuffer, xBytes, 0 );

    if( lResult == -pdFREERTOS_ERRNO_EWOULDBLOCK )
    {
        lResult = 0;
    }
    return lResult;
}

/*------------------ TCP bağlantısı ------------------*/
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

    TickType_t xTimeout = pdMS_TO_TICKS( 3000 );
    FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_RCVTIMEO, &xTimeout, sizeof( xTimeout ) );
    FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_SNDTIMEO, &xTimeout, sizeof( xTimeout ) );

    if( FreeRTOS_connect( xSocket, &xServer, sizeof( xServer ) ) != 0 )
    {
        FreeRTOS_closesocket( xSocket );
        return NULL;
    }
    return xSocket;
}

/*------------------ MQTT TASK ------------------*/
void vMQTTTask( void * pvParameters )
{
    ( void ) pvParameters;

    vTaskDelay( pdMS_TO_TICKS( 3000 ) );

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
            printf( "TCP baglanti hatasi\r\n" );
            vTaskDelay( pdMS_TO_TICKS( 5000 ) );
            continue;
        }

        xNetworkCtx.xTCPSocket = xSocket;
        xTransport.pNetworkContext = &xNetworkCtx;
        xTransport.send = prvTransportSend;
        xTransport.recv = prvTransportRecv;

        if( MQTT_Init( &xMqttCtx,
        			   &xTransport,
					   prvGetTimeMs,
                       ( MQTTEventCallback_t )prvEventCallback,
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
        if( MQTT_Connect( &xMqttCtx, &xConnect, NULL, 5000,
                          &xSessionPresent, NULL ,NULL ) != MQTTSuccess )
        {
            printf( "MQTT CONNECT hatasi\r\n" );
            FreeRTOS_closesocket( xSocket );
            vTaskDelay( pdMS_TO_TICKS( 5000 ) );
            continue;
        }
        printf( "MQTT BAGLANDI!\r\n" );

        TickType_t xLastPublish = xTaskGetTickCount();

        for( ;; )   /* İÇ DÖNGÜ: ProcessLoop + publish */
        {
            if( MQTT_ProcessLoop( &xMqttCtx ) != MQTTSuccess )
            {
                printf( "Baglanti koptu\r\n" );
                break;   /* dış döngüye düş → yeniden bağlan */
            }

            /* 5 saniyede bir test mesajı yayınla */
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
								  0,						//QoS0 oldugu icin 0 kullanabiliriz
								  NULL ) == MQTTSuccess )
                    printf( "Publish OK\r\n" );
            }

            vTaskDelay( pdMS_TO_TICKS( 10 ) );
        }

        FreeRTOS_closesocket( xSocket );
        vTaskDelay( pdMS_TO_TICKS( 3000 ) );
    }
}
