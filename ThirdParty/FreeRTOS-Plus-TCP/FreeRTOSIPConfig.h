#ifndef FREERTOS_IP_CONFIG_H
#define FREERTOS_IP_CONFIG_H

#include "FreeRTOS.h"

/* USER CONFIGS */

#define ipconfigUSE_DHCP                    1
#define ipconfigUSE_TCP                     1
#define ipconfigUSE_TCP_WIN                 1
#define ipconfigBYTE_ORDER                  pdFREERTOS_BIG_ENDIAN
#define ipconfigUSE_IPv4                  	1
#define ipconfigUSE_IPv6                  	1
#define ipconfigUSE_TCP                  	1

#define ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES   			1
#define ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM        			1
#define ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM        			1
#define ipconfigZERO_COPY_RX_DRIVER                   			1
#define ipconfigZERO_COPY_TX_DRIVER                   			1
#define ipconfigUSE_LINKED_RX_MESSAGES                			1
#define ipconfigSUPPORT_NETWORK_DOWN_EVENT            			1
#define ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS            		100
#define ipconfigEVENT_QUEUE_LENGTH       						ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS + 5

#endif
