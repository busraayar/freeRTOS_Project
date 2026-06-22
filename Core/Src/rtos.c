/*
 * rtos.c
 *
 *  Created on: May 4, 2026
 *      Author: busra.ayar
 */


#include <rtos.h>
#include <FreeRTOS_IP.h>


ETH_DMADescTypeDef TxDescTab_Ethernet[ETH_RX_DESC_CNT];
ETH_DMADescTypeDef RxDescTab_Ethernet[ETH_TX_DESC_CNT];

ETH_HandleTypeDef hETH;
ETH_TxPacketConfigTypeDef configTx;

CAN_HandleTypeDef hCAN1;

void ethernetInit();

void rtos_start( void )
{
	ethernetInit();

	HAL_ETH_Start_IT( &hETH );
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
void ethernetInit(){

	hETH.Instance = ETH;

	MACAddress[0] = 0x00;
	MACAddress[1] = 0x80;
	MACAddress[2] = 0xE1;
	MACAddress[3] = 0x00;
	MACAddress[4] = 0x00;
	MACAddress[5] = 0x00;

	hETH.Init.MACAddr = MACAddress;
	hETH.Init.MediaInterface = SYSCFG_PMC_MII_RMII_SEL;
	hETH.Init.TxDesc = TxDescTab_Ethernet;
	hETH.Init.RxDesc = RxDescTab_Ethernet;
	hETH.Init.RxBuffLen = 1536u;

	if(HAL_OK != HAL_ETH_Init(&hETH)){
		//error
	}
	memset(&configTx, 0, sizeof(ETH_TxPacketConfigTypeDef));
	configTx.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
	configTx.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
	configTx.CRCPadCtrl = ETH_CRC_PAD_INSERT;
}

