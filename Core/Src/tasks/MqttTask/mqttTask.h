/*
 * mqttTask.h
 *
 *  Created on: 2 Tem 2026
 *      Author: busra.ayar
 */

#ifndef SRC_TASKS_MQTTTASK_MQTTTASK_H_
#define SRC_TASKS_MQTTTASK_MQTTTASK_H_

void MqttTaskCreate( void );

typedef enum {
    TCP_STATE_INIT = 0,
    TCP_STATE_CREATE_SOCKET,
    TCP_STATE_SET_OPT,
    TCP_STATE_BIND,
    TCP_STATE_LISTEN,
    TCP_STATE_ACCEPT,
    TCP_STATE_CONNECTED,
    TCP_STATE_ERROR,
    TCP_STATE_CLOSE
} TCP_ServerState_t;

typedef enum {
	TCP_SOCSTATE_INIT = 0,
	TCP_SOCSTATE_TCP_RECV_HANDLER,
	TCP_SOCSTATE_TCP_SENT_HANDLER,
	TCP_SOCSTATE_SO_RCVTIMEO,
	TCP_SOCSTATE_SO_SNDTIMEO,
	TCP_SOCSTATE_ERR,
}TCP_SetSocketState_t;

#endif /* SRC_TASKS_MQTTTASK_MQTTTASK_H_ */
