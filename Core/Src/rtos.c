/*
 * rtos.c
 *
 *  Created on: May 4, 2026
 *      Author: busra.ayar
 */


#include "rtos.h"
#include "rng.h"

#include <FreeRTOS_IP.h>
/* FreeRTOS-Plus-TCP sockets include. */
#include "FreeRTOS_sockets.h"

QueueHandle_t xPingReplyQueue;

void rtos_start( void )
{

}

/* Defined by the application code, but called by FreeRTOS-Plus-TCP when the network
   connects/disconnects (if ipconfigUSE_NETWORK_EVENT_HOOK is set to 1 in
   FreeRTOSIPConfig.h). */

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include <stdio.h>

/* User-defined network event hook for multi-interface TCP/IP */
void vApplicationIPNetworkEventHook_Multi(eIPCallbackEvent_t eNetworkEvent,
                                          struct xNetworkEndPoint *pxEndPoint)
{
    if (pxEndPoint == NULL) {
        return; // Safety check
    }

    switch (eNetworkEvent)
    {
        case eNetworkUp:
            printf("Network UP on interface: %s\n", pxEndPoint->pxNetworkInterface->pcName);
            printf("IP Address: %lu.%lu.%lu.%lu\n",
                   (pxEndPoint->ipv4_settings.ulIPAddress >> 0) & 0xFF,
                   (pxEndPoint->ipv4_settings.ulIPAddress >> 8) & 0xFF,
                   (pxEndPoint->ipv4_settings.ulIPAddress >> 16) & 0xFF,
                   (pxEndPoint->ipv4_settings.ulIPAddress >> 24) & 0xFF);

            /* Start network-dependent tasks here */
            break;

        case eNetworkDown:
            printf("Network DOWN on interface: %s\n", pxEndPoint->pxNetworkInterface->pcName);
            /* Stop or suspend network-dependent tasks here */
            break;

        default:
            /* Unknown event */
            break;
    }
}

void configureInetrrupt(){
	uint32_t High_PreemptPriority = 0;
	uint32_t Low_PreemptPriority = 1;
	uint32_t WWDG_IRQn_SubPriority = 0;
	uint32_t CAN1_RX0_IRQn_SubPriority = 1;
	uint32_t CAN1_RX1_IRQn_SubPriority = 2;
	uint32_t ETH_IRQn_SubPriority = 3;
	uint32_t ETH_WKUP_IRQn_SubPriority = 4;


	HAL_NVIC_EnableIRQ(WWDG_IRQn);
	HAL_NVIC_SetPriority(WWDG_IRQn, High_PreemptPriority, WWDG_IRQn_SubPriority);

	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, High_PreemptPriority, CAN1_RX0_IRQn_SubPriority);

	HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn, High_PreemptPriority, CAN1_RX1_IRQn_SubPriority);

	HAL_NVIC_EnableIRQ(ETH_IRQn);
	HAL_NVIC_SetPriority(ETH_IRQn, High_PreemptPriority, ETH_IRQn_SubPriority);

	HAL_NVIC_EnableIRQ(ETH_WKUP_IRQn);
	HAL_NVIC_SetPriority(ETH_WKUP_IRQn, Low_PreemptPriority, ETH_WKUP_IRQn_SubPriority);

}


#if (configSUPPORT_STATIC_ALLOCATION == 1)
/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
   implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
   used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
       function then they must be declared static - otherwise they will be allocated on
       the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
       state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
       Note that, as the array is necessarily of type StackType_t,
       configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
   application must provide an implementation of vApplicationGetTimerTaskMemory()
   to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
       function then they must be declared static - otherwise they will be allocated on
       the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
       task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
       Note that, as the array is necessarily of type StackType_t,
      configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif
#if (configUSE_MALLOC_FAILED_HOOK == 1)
void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
#endif

/*
 * Callback that provides the inputs necessary to generate a randomized TCP
 * Initial Sequence Number per RFC 6528.  THIS IS ONLY A DUMMY IMPLEMENTATION
 * THAT RETURNS A PSEUDO RANDOM NUMBER SO IS NOT INTENDED FOR USE IN PRODUCTION
 * SYSTEMS.
 */
extern uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                                    uint16_t usSourcePort,
                                                    uint32_t ulDestinationAddress,
                                                    uint16_t usDestinationPort )
{
    ( void ) ulSourceAddress;
    ( void ) usSourcePort;
    ( void ) ulDestinationAddress;
    ( void ) usDestinationPort;

    return HAL_RNG_GetRandomNumber(&hrng);
}

/*
 * Set *pulNumber to a random number, and return pdTRUE. When the random number
 * generator is broken, it shall return pdFALSE.
 * The macros ipconfigRAND32() and configRAND32() are not in use
 * anymore in FreeRTOS+TCP.
 *
 * THIS IS ONLY A DUMMY IMPLEMENTATION THAT RETURNS A PSEUDO RANDOM NUMBER SO IS
 * NOT INTENDED FOR USE IN PRODUCTION SYSTEMS.
 */
BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
    *pulNumber = HAL_RNG_GetRandomNumber(&hrng);
    return pdTRUE;
}

eDHCPCallbackAnswer_t xApplicationDHCPHook_Multi( eDHCPCallbackPhase_t eDHCPPhase,
                                                  struct xNetworkEndPoint * pxEndPoint,
                                                  IP_Address_t * pxIPAddress
                                                )
{
	return eDHCPContinue;
}

/* If ipconfigSUPPORT_OUTGOING_PINGS is set to 1 in FreeRTOSIPConfig.h then
   vApplicationPingReplyHook() is called by the TCP/IP stack when the stack receives a
   ping reply. */
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier )
{
    switch( eStatus )
    {
        case eSuccess    :
            /* A valid ping reply has been received. Post the sequence number
               on the queue that is read by the vSendPing() function below. Do
               not wait more than 10ms trying to send the message if it cannot be
               sent immediately because this function is called from the TCP/IP
               RTOS task - blocking in this function will block the TCP/IP RTOS task. */
            xQueueSend( xPingReplyQueue, &usIdentifier, 10 / portTICK_PERIOD_MS );
            break;

        case eInvalidChecksum :
        case eInvalidData :
            /* A reply was received but it was not valid. */
            break;
    }
}


BaseType_t vSendPing( const int8_t *pcIPAddress )
{
	uint16_t usRequestSequenceNumber, usReplySequenceNumber;
	uint32_t ulIPAddress;

	/* The pcIPAddress parameter holds the destination IP address as a string in
	   decimal dot notation (for example, "192.168.0.200"). Convert the string into
	   the required 32-bit format. */
	ulIPAddress = FreeRTOS_inet_addr( pcIPAddress );

	/* Send a ping containing 8 data bytes. Wait (in the Blocked state) a
	   maximum of 100ms for a network buffer into which the generated ping request
	   can be written and sent. */
	usRequestSequenceNumber = FreeRTOS_SendPingRequest( ulIPAddress, 8, 100 / portTICK_PERIOD_MS );

	if( usRequestSequenceNumber == pdFAIL )
	{
		/* The ping could not be sent because a network buffer could not be
		   obtained within 100ms of FreeRTOS\_SendPingRequest() being called. */
	}
	else
	{
		/* The ping was sent. Wait 200ms for a reply. The sequence number from
		   each reply is sent from the vApplicationPingReplyHook() on the
		   xPingReplyQueue queue (this is not standard behaviour, but implemented in
		   the example function above). It is assumed the queue was created before
		   this function was called! */
		if( xQueueReceive( xPingReplyQueue,
						   &usReplySequenceNumber,
						   200 / portTICK_PERIOD_MS ) == pdPASS )
		{
			/* A ping reply was received. Was it a reply to the ping just sent? */
			if( usRequestSequenceNumber == usReplySequenceNumber )
			{
				/* This was a reply to the request just sent. */
			}
		}
	}
}


#if (configCHECK_FOR_STACK_OVERFLOW != 0)
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) xTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
#endif
