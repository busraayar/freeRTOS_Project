#ifndef FREERTOS_IP_CONFIG_H
#define FREERTOS_IP_CONFIG_H

#include "FreeRTOS.h"

/* USER CONFIGS */

#define ipconfigUSE_DHCP                    1
#define ipconfigUSE_TCP                     1
#define ipconfigUSE_TCP_WIN                 1
#define ipconfigUSE_IPv4                  	1
#define ipconfigUSE_IPv6                  	0
#define ipconfigUSE_TCP                  	1

/*Constants Affecting the TCP/IP Stack Task Execution Behaviour*/
#define ipconfigEVENT_QUEUE_LENGTH								ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS + 5
#define ipconfigIP_TASK_PRIORITY								( 3 )
#define ipconfigIP_TASK_STACK_SIZE_WORDS						( configMINIMAL_STACK_SIZE * 5 )
#define ipconfigPROCESS_CUSTOM_ETHERNET_FRAMES					0
#define ipconfigUSE_NETWORK_EVENT_HOOK							0

#define ipconfigBYTE_ORDER										pdFREERTOS_LITTLE_ENDIAN
#define ipconfigNETWORK_MTU										1500//default degerde tutuyorum
#define ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS					12//8
#define ipconfigZERO_COPY_RX_DRIVER								1
#define ipconfigZERO_COPY_TX_DRIVER								1
#define ipconfigSUPPORT_NETWORK_DOWN_EVENT						0
//#define ipconfigBUFFER_ALLOC_FIXED_SIZE							1//belki sonradan 0 a cekeriz

#define ipconfigSOCK_DEFAULT_RECEIVE_BLOCK_TIME					( 5000 / portTICK_PERIOD_MS )
#define ipconfigSOCK_DEFAULT_SEND_BLOCK_TIME					( 5000 / portTICK_PERIOD_MS )
#define ipconfigREPLY_TO_INCOMING_PINGS							1

/*TCP Specific Constants*/
//#define ipconfigIGNORE_UNKNOWN_PACKETS
//#define ipconfigTCP_HANG_PROTECTION
//#define ipconfigTCP_HANG_PROTECTION_TIME
//#define ipconfigTCP_KEEP_ALIVE
//#define ipconfigTCP_KEEP_ALIVE_INTERVAL
//#define ipconfigTCP_MSS
//#define ipconfigTCP_RX_BUFFER_LENGTH
//#define ipconfigTCP_TX_BUFFER_LENGTH
//#define ipconfigTCP_TIME_TO_LIVE
//#define ipconfigTCP_WIN_SEG_COUNT
//#define ipconfigUSE_TCP
//#define ipconfigUSE_TCP_TIMESTAMPS
//#define ipconfigUSE_TCP_WIN
//#define ipconfigTCP_SRTT_MINIMUM_VALUE_MS

/*UDP Specific Constants*/
//#define ipconfigUDP_MAX_RX_PACKETS
//#define ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS
//#define ipconfigUDP_PASS_ZERO_CHECKSUM_PACKETS
//#define ipconfigUDP_TIME_TO_LIVE
//
///*Other Constants Effecting Socket Behaviour*/
//#define ipconfigALLOW_SOCKET_SEND_WITHOUT_BIND
//#define ipconfigINCLUDE_FULL_INET_ADDR
//#define ipconfigSELECT_USES_NOTIFY
//#define ipconfigSOCKET_HAS_USER_SEMAPHORE
//#define ipconfigSOCKET_HAS_USER_WAKE_CALLBACK
//#define ipconfigSUPPORT_SELECT_FUNCTION
//#define ipconfigSUPPORT_SIGNALS
//#define ipconfigUSE_CALLBACKS

/*Constants Affecting the ARP Behaviour*/
//#define ipconfigARP_CACHE_ENTRIES
//#define ipconfigARP_STORES_REMOTE_ADDRESSES
//#define ipconfigARP_USE_CLASH_DETECTION
//#define ipconfigMAX_ARP_AGE
//#define ipconfigMAX_ARP_RETRANSMISSIONS
//#define ipconfigUSE_ARP_REMOVE_ENTRY
//#define ipconfigUSE_ARP_REVERSED_LOOKUP
//
///*Constants Affecting DHCP and Name Service Behaviour*/
//#define ipconfigDHCP_FALL_BACK_AUTO_IP
//#define ipconfigDHCP_REGISTER_HOSTNAME
//#define ipconfigDNS_CACHE_ADDRESSES_PER_ENTRY
//#define ipconfigDNS_CACHE_ENTRIES
//#define ipconfigDNS_CACHE_NAME_LENGTH
//#define ipconfigDNS_REQUEST_ATTEMPTS
//#define ipconfigDNS_USE_CALLBACKS
//#define ipconfigMAXIMUM_DISCOVER_TX_PERIOD
//#define ipconfigUSE_DHCP
//#define ipconfigUSE_DHCPv6
//#define ipconfigUSE_DHCP_HOOK
//#define ipconfigUSE_DNS
//#define ipconfigUSE_DNS_CACHE
//#define ipconfigUSE_LLMNR
//#define ipconfigUSE_NBNS
//#define ipconfigUSE_MDNS

///*Constants Affecting IP and ICMP Behaviour*/
//#define ipconfigUSE_IPv4
//#define ipconfigUSE_IPv6
//#define ipconfigFORCE_IP_DONT_FRAGMENT
//#define ipconfigICMP_TIME_TO_LIVE
//#define ipconfigIP_PASS_PACKETS_WITH_IP_OPTIONS
//#define ipconfigSUPPORT_OUTGOING_PINGS

/*Constants Affecting ND Behaviour*/
//#define ipconfigND_CACHE_ENTRIES

/*Constants Affecting RA Behaviour*/
//#define ipconfigUSE_RA
//#define ipconfigRA_SEARCH_COUNT
//#define ipconfigRA_IP_TEST_COUNT

/*Constants Providing Target Support*/
//#define ipconfigHAS_INLINE_FUNCTIONS
//#define ipconfigIS_VALID_PROG_ADDRESS
//#define ipconfigPORT_SUPPRESS_WARNING

/*Backward Compatibility*/
//#define ipconfigCOMPATIBLE_WITH_SINGLE
//#define ipconfigIPv4_BACKWARD_COMPATIBLE


#endif
