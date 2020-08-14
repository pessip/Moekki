#define Led 15

#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h>

WiFiClient net;
MQTTClient client;

const char *ssid = "DNA-WLAN-5G-AA38";
const char *password = "88780760472";

void connect()
{
  Serial.print('Connecting to MQTT Broker!');
  while (!client.connect("ESP32", "try", "try"))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected!");
  client.subscribe("/moekki");
}

void messageReceived(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
}

void setup()
{
  Serial.begin(115200);
  pinMode(5, OUTPUT); // set the LED pin mode

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.begin("test.mosquitto.org", net);
  client.onMessage(messageReceived);

  pinMode(Led, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

int value = 0;

void loop()
{
  client.loop();

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(Led, HIGH);
  client.publish("/moekki", "Hi!");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(Led, LOW);
  delay(1000);
}