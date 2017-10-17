#include "Arduino.h"
#include "MQTTHelper.h"

void (*forceReset)(void) = 0;

MQTTHelper::MQTTHelper()
{
    _mqtt.setClient(_client);
    _debug = false;
    _mqtt_user = "";
    _mqtt_password = "";
    clearSubscriptions();
}

MQTTHelper::~MQTTHelper()
{
    clearSubscriptions();
    free(subscriptions);
    _client.stop();
    _mqtt.disconnect();
}

void MQTTHelper::setDebug(bool debug)
{
    _debug = debug;
}

void MQTTHelper::debug(char *message)
{
    if (!_debug)
        return;
    if (!Serial)
        return;
    Serial.print(message);
}

void MQTTHelper::debugln(char *message)
{
    if (!_debug)
        return;
    if (!Serial)
        return;
    Serial.println(message);
}

void MQTTHelper::connectWifi(char *ssid, char *password)
{
    _ssid = ssid;
    _password = password;
    debug("Conectando na rede: ");
    debugln(_ssid);
    uint8_t ncon = 0;
    debugln(_ssid);
    debugln(_password);
    for (ncon = 0; ncon < MAXTIMESCONNECTION; ncon++)
    {
        WiFi.begin(_ssid, _password);
        uint8_t ntimes = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            debug(".");
            delay(500);
            if (ntimes > MAXTIMESCONNECTION)
                break;
            ntimes++;
        }
        if (WiFi.status() == WL_CONNECTED)
            return;
        ncon++;
    }
    if (ncon > MAXTIMESCONNECTION)
        forceReset();
}

void MQTTHelper::connectMQTT(char *server, int port)
{
    _serverMQTT = server;
    _port = port;
    debug("Conectando no servidor MQTT: ");
    debugln(_serverMQTT);
    _mqtt.setServer(_serverMQTT, _port);
    _mqtt.setCallback(std::bind(&MQTTHelper::executeSubscriptionCallback, this, std::placeholders::_1,
                                std::placeholders::_2, std::placeholders::_3));
}

void MQTTHelper::connectMQTT(char *server, int port, char *username, char *password)
{
    _mqtt_user = username;
    _mqtt_password = password;
    connectMQTT(server, port);
}

void MQTTHelper::reconnect()
{
    debugln("Conectando ao Broker MQTT.");
    if (_mqtt_user != "")
    {
        _mqtt.connect("mqtthelper_id", _mqtt_user, _mqtt_password, 0, 0, true, 0);
    }
    else
    {
        _mqtt.connect("mqtthelper_id", 0, 0, true, 0);
    }

    if (_mqtt.connected())
    {
        debugln("Conectado com Sucesso ao Broker");
    }
    else
    {
        debugln("Falha ao Conectador, rc= ");
        char *test = (char *)_mqtt.state();
        debug(test);
        debug(" tentando se reconectar...");
        delay(3000);
    }
    if (_mqtt.connected())
        subscribeAll();
}

void MQTTHelper::loop()
{
    if (!_mqtt.connected())
        reconnect();
    _mqtt.loop();
}

int MQTTHelper::publish(char *topic, char *message)
{
    debug("Publishing topic: ");
    debugln(topic);
    return _mqtt.publish(topic, message, true);
}

int MQTTHelper::subscribe(char *topic)
{
    debug("Subscribing topic: ");
    debugln(topic);
    _mqtt.subscribe(topic);
}

void MQTTHelper::clearSubscriptions()
{
    debugln("Clear subscriptions");
    for (uint8_t i = 0; i < MAXSUBSCRIPTIONS; i++)
        subscriptions[i] = 0;
}

bool MQTTHelper::subscribe(MQTTHelper_Subscribe *sub)
{
    uint8_t i;
    debug("Subscribing: ");
    debugln(sub->topic);
    for (i = 0; i < MAXSUBSCRIPTIONS; i++)
    {
        if (subscriptions[i] == sub)
            return true;
    }
    if (i == MAXSUBSCRIPTIONS)
    {
        for (i = 0; i < MAXSUBSCRIPTIONS; i++)
        {
            if (subscriptions[i] == 0)
            {
                subscriptions[i] = sub;
                this->subscribe(sub->topic);
                return true;
            }
        }
    }
    return false;
}

bool MQTTHelper::unsubscribe(MQTTHelper_Subscribe *sub)
{
    for (uint8_t i = 0; i < MAXSUBSCRIPTIONS; i++)
    {
        if (subscriptions[i] == sub)
        {
            this->unsubscribe(sub->topic);
            subscriptions[i] = 0;
            return true;
        }
    }
    return false;
}

int MQTTHelper::unsubscribe(char *topic)
{
    debug("Unsubscribed: ");
    debugln(topic);
    _mqtt.unsubscribe(topic);
}

int MQTTHelper::getSubscription(char *topic)
{
    for (int i = 0; i < MAXSUBSCRIPTIONS; i++)
    {
        if (this->subscriptions == 0)
            break;
        if (this->subscriptions[i] == 0)
            continue;
        if (strcmp(this->subscriptions[i]->topic, topic) == 0)
            return i;
    }
    return -1;
}

void MQTTHelper::subscribeAll()
{
    for (uint8_t i = 0; i < MAXSUBSCRIPTIONS; i++)
    {
        if (subscriptions[i] == 0)
            continue;
        this->subscribe(subscriptions[i]->topic);
    }
}

void MQTTHelper::executeSubscriptionCallback(char *topic, byte *payload, unsigned int length)
{
    debug("Topic subscripted: ");
    debugln(topic);
    int subcall = this->getSubscription(topic);
    if(subcall == -1) return;
    debug("Topic subscripted founded: ");
    debugln(this->subscriptions[subcall]->topic);
    if(this->subscriptions[subcall]->onExecute == 0) return;    
    this->subscriptions[subcall]->onExecute(topic, payload, length);
}