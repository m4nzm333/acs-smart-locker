#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi settings
const char *ssid = "Unlisted-MobileHotspot";
const char *password = "anuku123456";
const char *deviceId = "lockerA1";

// MQTT Broker settings
const char *mqtt_broker = "178.128.125.24";
// const char *mqtt_username = "emqx";
// const char *mqtt_password = "public";
const int mqtt_port = 1883;
bool lock_state = true;
const char *mqtt_discovery_topic = "homeassistant/device/lockerA1/config";
const char *mqtt_state_topic = "homeassistant/device/lockerA1/lock/state";
const char *mqtt_command_topic = "homeassistant/device/lockerA1/lock/set";

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void connectToWiFi();

void connectToMQTTBroker();

void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("\n\n\nStarting....");
  connectToWiFi();
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqttCallback);
  connectToMQTTBroker();
}

void setLockState(bool state)
{
  lock_state = state;
  digitalWrite(LED_BUILTIN, state ? LOW : HIGH);
  Serial.println(state ? "Locker is LOCKED" : "Locker is UNLOCKED");
  mqtt_client.publish(mqtt_state_topic, state ? "LOCKED" : "UNLOCKED", true);
}

void connectToWiFi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to the WiFi network");
}

void connectToMQTTBroker()
{
  mqtt_client.setBufferSize(1024);
  while (!mqtt_client.connected())
  {
    String client_id = "esp8266-client-" + String(WiFi.macAddress());
    Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
    if (mqtt_client.connect(client_id.c_str()))
    {
      Serial.println("Connected to MQTT broker");
      mqtt_client.subscribe(mqtt_command_topic);
      // Publish message upon successful connection
      // Fix: Home Assistant cant recognize the payload
      char discovery_payload[] = "{\"device\":{\"identifiers\":[\"lockerA1\"],\"name\":\"Locker A1\",\"manufacturer\":\"Custom\",\"model\":\"Relay Door Lock\"},\"o\":{\"name\":\"foobar\"},\"cmps\":{\"locka1-main\":{\"p\":\"lock\",\"name\":\"Locker A1 Lock\",\"unique_id\":\"lockerA1-lock-main\",\"command_topic\":\"homeassistant/device/lockerA1/lock/set\",\"state_topic\":\"homeassistant/device/lockerA1/lock/state\",\"payload_lock\":\"LOCK\",\"payload_unlock\":\"UNLOCK\",\"state_locked\":\"LOCKED\",\"state_unlocked\":\"UNLOCKED\",\"optimistic\":false,\"qos\":1,\"retain\":true}}}";
      // char discovery_payload[] = "{\"dev\":{\"ids\":\"ea334450945afc\",";
      mqtt_client.publish(mqtt_discovery_topic, discovery_payload, true);
      Serial.println(discovery_payload);
    }
    else
    {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String messageTemp;
  for (unsigned int i = 0; i < length; i++)
  {
    messageTemp += (char)payload[i];
  }
  Serial.println(messageTemp);
  if (messageTemp == "LOCK")
  {
    setLockState(true);
  }
  else if (messageTemp == "UNLOCK")
  {
    setLockState(false);
  }
  Serial.println("-----------------------");
}



void loop()
{
  if (!mqtt_client.connected())
  {
    connectToMQTTBroker();
  }
  mqtt_client.loop();
}
