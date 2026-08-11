/*
 * ethTask.c
 *
 *  Created on: 2 Tem 2026
 *      Author: busra.ayar
 */
#include "../IPTask/ipTask.h"
#include "mqtt/mqtt_task.h"

#include <FreeRTOS-Plus/FreeRTOS-Plus-TCP/source/include/FreeRTOS_Sockets.h>
#include "rtos.h"

const uint8_t ucIPAddress[ 4 ] = { 192, 168, 2, 125 };
const uint8_t ucNetMask[ 4 ] = { 255, 255, 255, 0 };
const uint8_t ucGatewayAddress[ 4 ] = { 192, 168, 2, 1 };
const uint8_t ucDNSServerAddress[ 4 ] = { 8, 8, 8, 8 };
const uint8_t ucMACAddress[ 6 ] = { 0x42, 0x55, 0x53, 0x52, 0x41, 0x41 };

#define tcpechoSHUTDOWN_DELAY    ( pdMS_TO_TICKS( 5000 ) )

// Ethernet driver function of ST
extern NetworkInterface_t * pxSTM32_FillInterfaceDescriptor( BaseType_t xEMACIndex, NetworkInterface_t * pxInterface );
static void vTCPServerTask( void *pvParameters );
void vTCPClientTask( void *pvParameters );
static BaseType_t vTCP_ConfigureConnectedSocket(Socket_t socket);
static void vMyTCPSentHandler ( Socket_t xSocket, size_t xLength );
BaseType_t vMyTCPReceiveHandler( Socket_t xSocket, void * pData, size_t xLength );
BaseType_t prvProcessData( Socket_t xSocket, void * pData, size_t xLength );

/* USER CODE END Includes */
TaskHandle_t TcpTaskHandle = NULL;
F_TCP_UDP_Handler_t xHandler;

static NetworkInterface_t xSTM32_Interface;
static NetworkEndPoint_t xSTM32_EndPoint;

void vStartNetwork( void )
{
	pxSTM32_FillInterfaceDescriptor( 0, &xSTM32_Interface );

	FreeRTOS_FillEndPoint(
	        &xSTM32_Interface,
	        &xSTM32_EndPoint,
	        ucIPAddress,
	        ucNetMask,
	        ucGatewayAddress,
	        ucDNSServerAddress,
	        ucMACAddress
	    );

    FreeRTOS_IPInit_Multi();
}

void vTCPInitializeTask( void ){
	BaseType_t xReturned;

//	xReturned = xTaskCreate(
//			vTCPServerTask,
//			"TCP TASK",
//			TCP_TASK_STACK_SIZE,
//			( void * )1,
//			TCP_TASK_PRIORITY,
//			&TcpTaskHandle);
//
//    if( xReturned == pdPASS )
//    {
//        /* The task was created. Use the task's handle to delete the task. */
////        vTaskDelete( CanTaskHandle );
//    }

	xReturned = xTaskCreate(
			vMQTTTask,
			"TCP TASK",
			TCP_TASK_STACK_SIZE,
			( void * )1,
			TCP_TASK_PRIORITY,
			&TcpTaskHandle);
//	xReturned = xTaskCreate(
//			vTCPServerTask,
//			"TCP TASK",
//			TCP_TASK_STACK_SIZE,
//			( void * )1,
//			TCP_TASK_PRIORITY,
//			&TcpTaskHandle);

    if( xReturned == pdPASS )
    {
        /* The task was created. Use the task's handle to delete the task. */
//        vTaskDelete( CanTaskHandle );
    }
}

void vTCPClientTask( void *pvParameters )
{
	Socket_t xClientSocket;
	socklen_t xSize = sizeof( struct freertos_sockaddr );
	static const TickType_t xTimeOut = pdMS_TO_TICKS( 2000 );

	/* Attempt to open the socket. */
	xClientSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );

	/* Check the socket was created. */
	configASSERT( xClientSocket != FREERTOS_INVALID_SOCKET );

	static char cRxedData[ 512 ];
	static char buffer[ 512 ];
	BaseType_t lBytesReceived;


	/* If FREERTOS_SO_RCVBUF or FREERTOS_SO_SNDBUF are to be used with
	   FreeRTOS_setsockopt() to change the buffer sizes from their default then do
	   it here!. (see the FreeRTOS_setsockopt() documentation. */

	/* If ipconfigUSE_TCP_WIN is set to 1 and FREERTOS_SO_WIN_PROPERTIES is to
	   be used with FreeRTOS_setsockopt() to change the sliding window size from
	   its default then do it here! (see the FreeRTOS_setsockopt()
	   documentation. */

	/* Set send and receive time outs. */
	FreeRTOS_setsockopt( xClientSocket,
						 0,
						 FREERTOS_SO_RCVTIMEO,
						 &xTimeOut,
						 sizeof( xTimeOut ) );

	FreeRTOS_setsockopt( xClientSocket,
						 0,
						 FREERTOS_SO_SNDTIMEO,
						 &xTimeOut,
						 sizeof( xTimeOut ) );

	/* Bind the socket, but pass in NULL to let FreeRTOS-Plus-TCP choose the port number.
	   See the next source code snipped for an example of how to bind to a specific
	   port number. */
	FreeRTOS_bind( xClientSocket, NULL, xSize );

	for(;;){
        /* Receive another block of data into the cRxedData buffer. */
        lBytesReceived = FreeRTOS_recv( xClientSocket, &cRxedData, 512, 0 );

        if( lBytesReceived > 0 )
        {
            /* Data was received, process it here. */
            prvProcessData( xClientSocket, cRxedData, lBytesReceived );
        }
        else if( lBytesReceived == 0 )
        {
            /* No data was received, but FreeRTOS\_recv() did not return an error.
               Timeout? */
        }
        else
        {
            /* Error (maybe the connected socket already shut down the socket?).
               Attempt graceful shutdown. */
            FreeRTOS_shutdown( xClientSocket, FREERTOS_SHUT_RDWR );
            break;
        }
    }

    /* The RTOS task will get here if an error is received on a read. Ensure the
 socket has shut down (indicated by FreeRTOS\_recv() returning a -pdFREERTOS\_ERRNO\_EINVAL
 error before closing the socket). */

    while( FreeRTOS_recv( xClientSocket, buffer, 512, 0 ) >= 0 )
    {
        /* Wait for shutdown to complete. If a receive block time is used then
           this delay will not be necessary as FreeRTOS\_recv() will place the RTOS task
           into the Blocked state anyway. */
        vTaskDelay( pdTICKS_TO_MS( 250 ) );

        /* Note - real applications should implement a timeout here, not just
           loop forever. */
    }

    /* Shutdown is complete and the socket can be safely closed. */
    FreeRTOS_closesocket( xClientSocket );

}
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

BaseType_t prvProcessData( Socket_t xSocket, void * pData, size_t xLength )
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
