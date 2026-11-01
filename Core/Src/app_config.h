#ifndef SRC_APP_CONFIG_H_
#define SRC_APP_CONFIG_H_

#include "rtos.h"
#include "mqtt_task.h"

#define MAX_MQTT_MSG_LENGTH       64u

#define MQTT_TASK_QUEUE_LENGTH    3U
#define CAN_TASK_QUEUE_LENGTH     3U

typedef enum
{
    MQTT_SUBS_MSG_RCV = 0,
    MQTT_PUBL_MSG_SEND,
    MQTT_GET_STATUS,
    MQTT_SETTINGS,
    CAN_MSG_RECEIVED
} AppMessageType;

//36 byte
typedef struct
{
    uint32_t canId;
    uint8_t dataLen;
    uint8_t data[8];
} CanMessage_s;

//67 byte
typedef struct
{
    mqtt_topic_t topic;
    uint16_t payloadLength;
    char payload[MAX_MQTT_MSG_LENGTH];
} MqttMessage_s;


typedef struct
{
	AppMessageType TypeId;

    union{
        MqttMessage_s mqttMsg;
        CanMessage_s canMsg;
    };

} AppMessage_t;

#endif /* SRC_APP_CONFIG_H_ */
