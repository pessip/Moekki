#define Led 15

#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h>

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;
int postN = 0;

const char *ssid = "DNA-WLAN-2G-AA38";
const char *password = "88780760472";

void connect()
{
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
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

  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);
  connect();

  pinMode(Led, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  client.loop();

  if (!client.connected())
  {
    connect();
  }

  // publish a message roughly every 10 seconds.
  if (millis() - lastMillis > 10000)
  {
    String content = "Request: " + String(postN);
    lastMillis = millis();
    client.publish("/moekki", content);
    ++postN;
  }
}