/*
  MQTTHelper_Subscribe.h - Calss Subscribe for MQTTHelper.
  Created by Maykon L. Capellari, October 03, 2017.
  Released into the public domain.
*/
#ifndef MQTTHelper_Subscribe_h
#define MQTTHelper_Subscribe_h

#include "Arduino.h"

//#define MQTT_SUBSCRIBE_CALLBACK void (*callback)(uint8_t*, unsigned int)
typedef void (*MQTTSubscribeCallback)(char*, byte*, unsigned int);

class MQTTHelper_Subscribe{
    public:
        MQTTHelper_Subscribe(char *topic, MQTTSubscribeCallback callback = 0);
        ~MQTTHelper_Subscribe();
        void setCallback(MQTTSubscribeCallback callback);
        char* topic;
        MQTTSubscribeCallback onExecute;
};

#endif