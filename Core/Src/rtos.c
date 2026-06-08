/*
 * rtos.c
 *
 *  Created on: May 4, 2026
 *      Author: busra.ayar
 */


#include <rtos.h>
#include <FreeRTOS_IP.h>


ETH_DMADescTypeDef TxDesc_Ethernet[BufferLen];
ETH_DMADescTypeDef RxDesc_Ethernet[BufferLen];
ETH_HandleTypeDef hETH;

void ethernetInit();


void ethernetInit(){

	hETH.Instance = ETH;

	hETH.Init.MACAddr = MACAddress;
	hETH.Init.MediaInterface = SYSCFG_PMC_MII_RMII_SEL;
	hETH.Init.TxDesc = TxDesc_Ethernet;
	hETH.Init.RxDesc = RxDesc_Ethernet;
	hETH.Init.RxBuffLen = 1536u;

	if(HAL_OK != HAL_ETH_Init(&hETH)){

	}
}
