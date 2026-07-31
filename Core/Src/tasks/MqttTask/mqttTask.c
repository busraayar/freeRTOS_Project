/*
 * mqttTask.c
 *
 *  Created on: 2 Tem 2026
 *      Author: busra.ayar
 */
#include "rtos.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "mqttTask.h"
//#include <../Inc/core_mqtt_agent.h>
#include "core_mqtt_agent.h"

#define tcpechoSHUTDOWN_DELAY    ( pdMS_TO_TICKS( 5000 ) )

BaseType_t vTCP_ConfigureConnectedSocket(Socket_t socket);
BaseType_t vMyTCPReceiveHandler( Socket_t xSocket, void * pData, size_t xLength  );
void vMyTCPSentHandler ( Socket_t xSocket, size_t xLength );
void create_socket_TCPserver();
void create_socket_TCPclient();

TaskHandle_t MqttTaskHandle = NULL;

Socket_t xServerSocket;
Socket_t xClientSocket;
Socket_t xConnectedSocket;

F_TCP_UDP_Handler_t xHandler;

void vTCPServerTask( void *pvParameters )
{
	static BaseType_t retval;
	retval = pdFREERTOS_ERRNO_NONE;
    Socket_t xListeningSocket, xConnectedSocket;
    struct freertos_sockaddr xBindAddress, xClientAddress;
    socklen_t xSize = sizeof( xClientAddress );

    xListeningSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );

    configASSERT( xListeningSocket != FREERTOS_INVALID_SOCKET );

    vTCP_ConfigureConnectedSocket(xListeningSocket);

    xBindAddress.sin_port = ( uint16_t ) 5000;
    xBindAddress.sin_port = FreeRTOS_htons( xBindAddress.sin_port );
    xBindAddress.sin_family = FREERTOS_AF_INET4;

    retval = FreeRTOS_bind( xListeningSocket, &xBindAddress, sizeof( xBindAddress ) );
    if( retval != pdFREERTOS_ERRNO_NONE ){
    	printf(" Binding a socket ERROR ");
    }

    retval = FreeRTOS_listen( xListeningSocket, 5 );
    if( retval != pdFREERTOS_ERRNO_NONE ){
    	printf(" Listening socket ERROR ");
    }

    for( ;; )
    {
        xConnectedSocket = FreeRTOS_accept( xListeningSocket, &xClientAddress, &xSize );

        configASSERT( xConnectedSocket != FREERTOS_INVALID_SOCKET );

    }
}
void MqttTaskCreate( void ){
	BaseType_t xReturned;

	xReturned = xTaskCreate(
			vTCPServerTask,
			"MQTT TASK",
			MQTT_TASK_STACK_SIZE,
			( void * )1,
			MQTT_TASK_PRIORITY,
			&MqttTaskHandle);

    if( xReturned == pdPASS )
    {
        /* The task was created. Use the task's handle to delete the task. */
//        vTaskDelete( CanTaskHandle );
    }
}
BaseType_t vTCP_ConfigureConnectedSocket(Socket_t socket)
{
    BaseType_t retVal;
    TickType_t xTimeout_ticks = TCP_CONNECTION_TIMEOUT_TICK;
    BaseType_t xTrue = pdTRUE;

    xHandler.pxOnTCPReceive = vMyTCPReceiveHandler;
    xHandler.pxOnTCPSent = vMyTCPSentHandler;

    retVal = FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_REUSE_LISTEN_SOCKET, &xTrue, sizeof( xTrue ) );
    if( retVal != pdFREERTOS_ERRNO_NONE ) return retVal;

    // 1. Receive Handler tanımla (Veri gelince tetiklenecek olan)
    retVal = FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_TCP_RECV_HANDLER, &xHandler, sizeof(xHandler) );
    if( retVal != pdFREERTOS_ERRNO_NONE ) return retVal;

    // 2. Sent Handler tanımla
    retVal = FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_TCP_SENT_HANDLER, &xHandler, sizeof(xHandler) );
    if( retVal != pdFREERTOS_ERRNO_NONE ) return retVal;

    // 3. Zaman aşımlarını tanımla
    retVal = FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_RCVTIMEO, &xTimeout_ticks, sizeof(xTimeout_ticks) );
    if( retVal != pdFREERTOS_ERRNO_NONE ) return retVal;

    retVal = FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_SNDTIMEO, &xTimeout_ticks, sizeof(xTimeout_ticks) );

    return retVal;
}

BaseType_t vMyTCPReceiveHandler( Socket_t xSocket, void * pData, size_t xLength )
{
    // Hercules'ten gelen veri pData içinde, uzunluğu xLength içinde buraya düşer.

    // Test için: Gelen veriyi aynen geri gönder (Echo Server)
    FreeRTOS_send( xSocket, pData, xLength, 0 );

    // pdFALSE dönerek FreeRTOS'a bu veriyi sahiplendiğimizi söylüyoruz.
    return pdFALSE;
}

void vMyTCPSentHandler ( Socket_t xSocket, size_t xLength )
{

}
