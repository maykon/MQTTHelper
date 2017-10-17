/*
  MQTTHelper.h - Library for MQTT Helper.
  Created by Maykon L. Capellari, September 30, 2017.
  Released into the public domain.
*/
#ifndef MQTTHelper_h
#define MQTTHelper_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <MQTTHelper_Subscribe.h>

#define MAXSUBSCRIPTIONS 5
#define MAXTIMESCONNECTION 5

class MQTTHelper
{
private:
  char *_ssid;
  char *_password;
  char *_serverMQTT;
  int _port;
  char *_mqtt_user;
  char *_mqtt_password;
  bool _debug;
  WiFiClient _client;
  PubSubClient _mqtt;
  void reconnect();
  void debug(char *message);
  void debugln(char *message);
  MQTTHelper_Subscribe *subscriptions[MAXSUBSCRIPTIONS];
  void clearSubscriptions();
  int getSubscription(char *topic);
  void executeSubscriptionCallback(char *topic, byte *payload, unsigned int length);
  void subscribeAll();
public:
  MQTTHelper();
  ~MQTTHelper();
  void connectWifi(char *ssid, char *password);
  void connectMQTT(char *server, int port);
  void connectMQTT(char *server, int port, char *username, char *password);
  void loop();
  int publish(char *topic, char *message);
  int subscribe(char *topic);
  bool subscribe(MQTTHelper_Subscribe *sub);
  int unsubscribe(char *topic);
  bool unsubscribe(MQTTHelper_Subscribe *sub);
  void setDebug(bool debug);
};

#endif
