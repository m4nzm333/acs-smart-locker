#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi settings
const char *ssid = "Unlisted-MobileHotspot";         // Replace with your WiFi name
const char *password = "anuku123456"; // Replace with your WiFi password
const char *deviceId = "locker-A1";

// MQTT Broker settings
const char *mqtt_broker = "178.128.125.24"; // EMQX broker endpoint
// const char *mqtt_username = "emqx";         // MQTT username for authentication
// const char *mqtt_password = "public";       // MQTT password for authentication
const int mqtt_port = 1883;
bool lock_state = true;               // MQTT port (TCP)
const char *mqtt_discovery_topic = "homeassistant/kitchen/lock/config";    // MQTT topic
const char *mqtt_sub_topic = "homeassistant/kitchen/lock/set"; // MQTT subscription topic

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void connectToWiFi();

void connectToMQTTBroker();

void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup()
{
  Serial.begin(115200);
  connectToWiFi();
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqttCallback);
  connectToMQTTBroker();
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
      mqtt_client.subscribe(mqtt_sub_topic);
      // Publish message upon successful connection
      // Fix: Home Assistant cant recognize the payload
      char discovery_payload[] = "{\"dev\":{\"ids\":\"ea334450945afc\",\"name\":\"Kitchen Door Lock\",\"mf\":\"Bla electronics\",\"mdl\":\"xya\",\"sw\":\"1.0\",\"sn\":\"ea334450945afc\",\"hw\":\"1.0rev2\"},\"o\":{\"name\":\"bla2mqtt\",\"sw\":\"2.1\",\"url\":\"https://bla2mqtt.example.com/support\"},\"name\":\"Kitchen Door Lock\",\"unique_id\":\"lock01ae\",\"state_topic\":\"homeassistant/kitchen/lock/state\",\"command_topic\":\"homeassistant/kitchen/lock/set\",\"payload_lock\":\"LOCK\",\"payload_unlock\":\"UNLOCK\",\"state_locked\":\"LOCKED\",\"state_unlocked\":\"UNLOCKED\",\"optimistic\":false,\"qos\":2,\"retain\":true}";
      // char discovery_payload[] = "{\"dev\":{\"ids\":\"ea334450945afc\",";
      mqtt_client.publish(mqtt_discovery_topic, discovery_payload, true);
      Serial.println(discovery_payload);
    } else {
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
  // const String messageTemp = "";
  // memcpy((char *)messageTemp.c_str(), payload, length);
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
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
