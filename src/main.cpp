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
  // Begin Serial Monitor
  Serial.begin(115200);

  // Wifi Connection
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

  // Wifi Info
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // COnnect to MQTT Broker
  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);
  connect();

  // Pin Modes
  pinMode(Led, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // MQTT CLient loop
  client.loop();

  // Reconnect if connection is lost
  if (!client.connected())
  {
    connect();
  }

  // publish a message counting up roughly every second.
  if (millis() - lastMillis > 1000)
  {
    String content = "Request: " + String(postN);
    lastMillis = millis();
    client.publish("/moekki", content);
    ++postN;
    // Turn LED on if request number is even
    if (postN % 2 == 0)
    {
      digitalWrite(BUILTIN_LED, LOW);
    }
    else
    {
      digitalWrite(BUILTIN_LED, HIGH);
    }
  }
}