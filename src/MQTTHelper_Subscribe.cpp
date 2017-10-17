#include "Arduino.h"
#include "MQTTHelper_Subscribe.h"

MQTTHelper_Subscribe::MQTTHelper_Subscribe(char *topic, MQTTSubscribeCallback callback){
    this->topic = topic;
    this->setCallback(callback);
}

MQTTHelper_Subscribe::~MQTTHelper_Subscribe(){
    this->topic = NULL;
    this->onExecute = NULL;
}

void MQTTHelper_Subscribe::setCallback(MQTTSubscribeCallback callback){
    this->onExecute = callback;
}