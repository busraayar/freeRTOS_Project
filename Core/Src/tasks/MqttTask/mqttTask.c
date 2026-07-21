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

static BaseType_t vTCP_ConfigureServerSocket();
BaseType_t vTCP_ConfigureConnectedSocket(Socket_t socket);
static BaseType_t vMyTCPReceiveHandler( Socket_t xSocket, void * pData, size_t xLength  );
static void vMyTCPSentHandler ( Socket_t xSocket, size_t xLength );
static void create_socket_TCPserver();
static void create_socket_TCPclient();
void tcpserver_init( void );

TaskHandle_t MqttTaskHandle = NULL;

Socket_t xServerSocket;
Socket_t xClientSocket;
Socket_t xConnectedSocket;

F_TCP_UDP_Handler_t xHandler;

static void MqttTaskFunction( void * pvParameters ){

    /* The parameter value is expected to be 1 as 1 is passed in the
       pvParameters value in the call to xTaskCreate() below. */

    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

//	create_socket_TCPclient();
	for(;;){
		create_socket_TCPserver();
		/* Task code goes here. */
	}
}

void MqttTaskCreate( void ){
	BaseType_t xReturned;

	xReturned = xTaskCreate(
			MqttTaskFunction,
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

void create_socket_TCPclient(){

	static TCP_ServerState_t eTCPState = TCP_STATE_CREATE_SOCKET;
	static BaseType_t retVal;
	struct freertos_sockaddr xBindAddress;
	memset( &xBindAddress, 0, sizeof(xBindAddress) );
	const BaseType_t xBacklog = 20;
	socklen_t xSize = sizeof( xBindAddress );

    switch(eTCPState){
		case TCP_STATE_INIT:
		case TCP_STATE_CREATE_SOCKET:
		{
			while( FreeRTOS_IsNetworkUp() == pdFALSE )
			{
				vTaskDelay( pdMS_TO_TICKS( 500 ) ); // Ağ gelene kadar bekle
			}
			xClientSocket = FreeRTOS_socket( FREERTOS_AF_INET,
											 FREERTOS_SOCK_STREAM ,
											 FREERTOS_IPPROTO_TCP );

			/* Check the socket was created successfully. */
			if( xClientSocket != FREERTOS_INVALID_SOCKET )
			{
				eTCPState = TCP_STATE_SET_OPT;

			}else{
				/* There was insufficient FreeRTOS heap memory available for the socket
				   to be created. */
			}
		}
		case TCP_STATE_SET_OPT:
		{
			retVal = vTCP_ConfigureServerSocket(xClientSocket);
			if( retVal != pdFREERTOS_ERRNO_NONE ) {
				eTCPState = TCP_STATE_ERROR;
				break;
			}

			eTCPState = TCP_STATE_BIND;
		}
		case TCP_STATE_BIND:
		{
			 /* Set the listening port to 10000. */
			xBindAddress.sin_port = ( uint16_t ) 10000;
			xBindAddress.sin_port = FreeRTOS_htons( xBindAddress.sin_port );
			xBindAddress.sin_family = FREERTOS_AF_INET4; /* FREERTOS_AF_INET6 to be used for IPv6 */

			retVal = FreeRTOS_bind( xClientSocket, &xBindAddress, sizeof( xBindAddress ));

			if( retVal != pdFREERTOS_ERRNO_NONE ) {
				eTCPState = TCP_STATE_ERROR;
				break;
			}
			eTCPState = TCP_STATE_LISTEN;
			break;
		}
		case TCP_STATE_ERROR:
			if( xClientSocket != FREERTOS_INVALID_SOCKET ) {
				FreeRTOS_closesocket( xClientSocket );
				xClientSocket = FREERTOS_INVALID_SOCKET;
			}
		case TCP_STATE_CONNECTED:
		case TCP_STATE_CLOSE:

    }

//    vTaskDelay( pdMS_TO_TICKS( 5000 ) ); // 5 saniye bekle ve yeniden dene
	eTCPState = TCP_STATE_CREATE_SOCKET;

    return;


}

static void create_socket_TCPserver(){
    static TCP_ServerState_t eTCPState = TCP_STATE_CREATE_SOCKET;
    static BaseType_t retVal;
    struct freertos_sockaddr xBindAddress;
    const BaseType_t xBacklog = 20;
    socklen_t xSize = sizeof( xBindAddress );
    static const TickType_t xReceiveTimeOut = portMAX_DELAY;

    switch(eTCPState){
        case TCP_STATE_CREATE_SOCKET:
        {
            xServerSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );

            if( xServerSocket != FREERTOS_INVALID_SOCKET || xServerSocket != NULL)
            {
                eTCPState = TCP_STATE_SET_OPT;
            }else{
            	FreeRTOS_closesocket(xServerSocket);
            }
            break;
        }

        case TCP_STATE_SET_OPT:
		{
//		    FreeRTOS_setsockopt( xServerSocket,
//		                         0,
//		                         FREERTOS_SO_RCVTIMEO,
//		                         &xReceiveTimeOut,
//		                         sizeof( xReceiveTimeOut ) );

		    retVal = vTCP_ConfigureServerSocket(xServerSocket);

		    if( retVal != pdFREERTOS_ERRNO_NONE ) {
				eTCPState = TCP_STATE_ERROR;
				break;
			}
			eTCPState = TCP_STATE_BIND;
		}

        case TCP_STATE_BIND:
        {
            memset( &xBindAddress, 0, sizeof(xBindAddress) );
            xBindAddress.sin_port = FreeRTOS_htons(15000);
            xBindAddress.sin_family = FREERTOS_AF_INET4; /* FREERTOS_AF_INET6 to be used for IPv6 */


            /* Bind the socket to the port that the client RTOS task will send to. */
            retVal = FreeRTOS_bind( xServerSocket, &xBindAddress, sizeof( xBindAddress ) );
            if( retVal != pdFREERTOS_ERRNO_NONE ) {
                break;
            } else {
                eTCPState = TCP_STATE_LISTEN;
            }
        }

        case TCP_STATE_LISTEN:
        {
            retVal = FreeRTOS_listen( xServerSocket, xBacklog );
            if( retVal != pdFREERTOS_ERRNO_NONE ) {
				break;
            } else {
                eTCPState = TCP_STATE_ACCEPT;
            }
        }

        case TCP_STATE_ACCEPT:
		{
			static size_t freeHeap = 0;
			freeHeap = xPortGetFreeHeapSize();
			xConnectedSocket = FreeRTOS_accept( xServerSocket, &xBindAddress, &xSize);
			freeHeap = xPortGetFreeHeapSize();
			if( xConnectedSocket == FREERTOS_INVALID_SOCKET || xConnectedSocket == NULL)
			{
                eTCPState = TCP_STATE_ACCEPT;
				break;
			}

		}

        case TCP_STATE_CONNECTED:
        {
            // Hercules şu an bağlı!
            // vMyTCPReceiveHandler fonksiyonun tetiklenecektir veri geldikçe.
            vTaskDelay( pdMS_TO_TICKS( 100 ) );
            break;
        }

        case TCP_STATE_ERROR:
        {
            eTCPState = TCP_STATE_CREATE_SOCKET;
            break;
        }

        default:
            break;
    }
}

//BaseType_t vTCP_ConfigureServerSocket(Socket_t socket)
//{
//	static TCP_SetSocketState_t socketStat = TCP_SOCSTATE_TCP_RECV_HANDLER;
//	static BaseType_t retVal;
//
 //	xHandler.pxOnTCPSent = vMyTCPSentHandler;
//
//	static TickType_t xTimeout_ticks;
//	xTimeout_ticks = TCP_CONNECTION_TIMEOUT_TICK;
//
//	switch(socketStat){
//	case TCP_SOCSTATE_TCP_RECV_HANDLER:
//    	retVal = FreeRTOS_setsockopt( socket,
//    								  0,
//									  FREERTOS_SO_TCP_RECV_HANDLER,
//									  &xHandler,
//									  sizeof(xHandler) );
//    	socketStat = TCP_SOCSTATE_TCP_SENT_HANDLER;
//    	if( retVal != pdFREERTOS_ERRNO_NONE ){
//        	socketStat = TCP_SOCSTATE_ERR;
//    		break;
//    	}
//
//	case TCP_SOCSTATE_TCP_SENT_HANDLER:
//    	retVal = FreeRTOS_setsockopt( socket,
//    								  0,
//									  FREERTOS_SO_TCP_SENT_HANDLER,
//									  &xHandler,
//									  sizeof(xHandler) );
//    	if( retVal != pdFREERTOS_ERRNO_NONE ){
//        	socketStat = TCP_SOCSTATE_TCP_RECV_HANDLER;
//    		break;
//    	}
//    	socketStat = TCP_SOCSTATE_SO_RCVTIMEO;
//
//	case TCP_SOCSTATE_SO_RCVTIMEO:
//    	retVal = FreeRTOS_setsockopt( socket,
//    								  0,
//									  FREERTOS_SO_RCVTIMEO,
//									  &xTimeout_ticks,
//									  sizeof(xTimeout_ticks) );
//    	if( retVal != pdFREERTOS_ERRNO_NONE ){
//        	socketStat = TCP_SOCSTATE_TCP_RECV_HANDLER;
//    		break;
//    	}
//    	socketStat = TCP_SOCSTATE_SO_SNDTIMEO;
//
//	case TCP_SOCSTATE_SO_SNDTIMEO:
//    	retVal = FreeRTOS_setsockopt( socket,
//    								  0,
//									  FREERTOS_SO_SNDTIMEO,
//									  &xTimeout_ticks,
//									  sizeof(xTimeout_ticks) );
//    	if( retVal != pdFREERTOS_ERRNO_NONE ){
//        	socketStat = TCP_SOCSTATE_TCP_RECV_HANDLER;
//    		break;
//    	}
//    	socketStat = TCP_SOCSTATE_SO_SNDTIMEO;
//
//	}
//	return retVal;
//}
BaseType_t vTCP_ConfigureConnectedSocket(Socket_t socket)
{
    BaseType_t retVal;
    TickType_t xTimeout_ticks = TCP_CONNECTION_TIMEOUT_TICK;

    xHandler.pxOnTCPReceive = vMyTCPReceiveHandler;
    xHandler.pxOnTCPSent = vMyTCPSentHandler;

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
BaseType_t vTCP_ConfigureServerSocket(Socket_t socket)
{
    BaseType_t retVal;
    TickType_t xTimeout_ticks = portMAX_DELAY;
    BaseType_t xTrue = pdTRUE;

    xHandler.pxOnTCPReceive = vMyTCPReceiveHandler;
    xHandler.pxOnTCPSent = vMyTCPSentHandler;

    FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_REUSE_LISTEN_SOCKET, &xTrue, sizeof( xTrue ) );
    // Receive Handler
    retVal = FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_TCP_RECV_HANDLER, &xHandler, sizeof(xHandler) );
    if( retVal != pdFREERTOS_ERRNO_NONE ) return retVal;

    // Sent Handler
    retVal = FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_TCP_SENT_HANDLER, &xHandler, sizeof(xHandler) );
    if( retVal != pdFREERTOS_ERRNO_NONE ) return retVal;

    // RCV Timeout
    retVal = FreeRTOS_setsockopt( socket, 0, FREERTOS_SO_RCVTIMEO, &xTimeout_ticks, sizeof(xTimeout_ticks) );
    if( retVal != pdFREERTOS_ERRNO_NONE ) return retVal;

    // SND Timeout
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
