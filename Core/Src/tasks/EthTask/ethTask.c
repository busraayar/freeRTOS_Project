/*
 * ethTask.c
 *
 *  Created on: 2 Tem 2026
 *      Author: busra.ayar
 */
#include "rtos.h"
#include "ethTask.h"

const uint8_t ucIPAddress[ 4 ] = { 192, 168, 2, 125 };
const uint8_t ucNetMask[ 4 ] = { 255, 255, 255, 0 };
const uint8_t ucGatewayAddress[ 4 ] = { 192, 168, 2, 1 };
const uint8_t ucDNSServerAddress[ 4 ] = { 8, 8, 8, 8 };
const uint8_t ucMACAddress[ 6 ] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 }; // Sizin MAC adresiniz


const uint8_t pcIPAddress[ 4 ]       = { 192, 168, 2, 10 };
const uint8_t pcNetMask[ 4 ]         = { 255, 255, 255, 0 };
const uint8_t pcGatewayAddress[ 4 ]   = { 192, 168, 2, 0 };
const uint8_t pcDNSServerAddress[ 4 ] = { 8, 8, 8, 8 };

// Ethernet driver function of ST
extern NetworkInterface_t * pxSTM32_FillInterfaceDescriptor( BaseType_t xEMACIndex, NetworkInterface_t * pxInterface );
/* USER CODE END Includes */


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
