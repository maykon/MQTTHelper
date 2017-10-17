#include <MQTTHelper.h>
#include <MQTTHelper_Subscribe.h>
#include <ButtonDebounce.h>

MQTTHelper mqtt;
void topicTHMKSUBExecuted(char *topic, byte *payload, unsigned int length);
void topicTHMK85Executed(char *topic, byte *payload, unsigned int length);
MQTTHelper_Subscribe topicTHMKSUB("THMKSUB", &topicTHMKSUBExecuted);
MQTTHelper_Subscribe topicTHMK85("THMK85", &topicTHMK85Executed);

ButtonDebounce button(D6, 5000);
const unsigned int LED_RED_PIN = D7;
const unsigned int LED_GREEN_PIN = D8;

void changeButton(int state){
  Serial.print("changed: ");
  Serial.println(state);
  digitalWrite(LED_RED_PIN, !state);
  digitalWrite(LED_GREEN_PIN, state);
}

void btn_callback(int state)
{
  changeButton(state);
  mqtt.publish("THMK85", const_cast<char*>(String(state).c_str()));
}

void topicTHMKSUBExecuted(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Execução exclusiva do tópico: ");
  Serial.println(topic);
}

void topicTHMK85Executed(char *topic, byte *payload, unsigned int length){
  char msg = (char) payload[0];
  int state = msg - '0';
  Serial.print("topicTHMK85Executed: ");
  Serial.println(state);
  changeButton(state);
}

void setup()
{
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  while (!Serial)
    delay(10);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, LOW);
  button.setCallback(btn_callback);

  //mqtt.setDebug(true);
  mqtt.connectWifi("MK_HPOT", "123mudar");
  //mqtt.connectMQTT("test.mosquitto.org", 1883);
  mqtt.connectMQTT("m13.cloudmqtt.com", 17114, "yfdtioqc", "5x7EbnnbzW9J");
  delay(1000);
  mqtt.subscribe(&topicTHMKSUB);
  mqtt.subscribe(&topicTHMK85);
}

void loop()
{
  button.update();
  mqtt.loop();
}
