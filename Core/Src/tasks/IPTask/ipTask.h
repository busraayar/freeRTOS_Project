/*
 * ethTask.h
 *
 *  Created on: 2 Tem 2026
 *      Author: busra.ayar
 */

#ifndef SRC_TASKS_IPTASK_IPTASK_H_
#define SRC_TASKS_IPTASK_IPTASK_H_

#include <FreeRTOS-Plus/FreeRTOS-Plus-TCP/source/include/FreeRTOS_Routing.h>
#include <FreeRTOSIPConfig.h>

void vStartNetwork( void );
void vTCPInitializeTask( void );

#endif /* SRC_TASKS_IPTASK_IPTASK_H_ */
