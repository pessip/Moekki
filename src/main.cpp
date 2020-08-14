// Import Libraries
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <MQTT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

// Define Pin locations and temperature precision
#define ONE_WIRE_BUS 15
#define TEMPERATURE_PRECISION 9

// Setup instances
WiFiClient net; // WiFi Client
WiFiUDP ntpUDP; // UDP Client for ntp
NTPClient timeClient(ntpUDP);
MQTTClient client;                   // MQTT Client
OneWire oneWire(ONE_WIRE_BUS);       // OneWire Bus
DallasTemperature sensors(&oneWire); // DallasTemperature and pass oneWire for OneWire comms.

// Arrays to hold device address
DeviceAddress waterThermometer;

// Setup variables
unsigned long lastMillis = 0;
int postN = 0;

// Wifi connection info
const char *ssid = "DNA-WLAN-2G-AA38";
const char *password = "88780760472";

// WiFi connect
void connect()
{
  // Check wifi connection
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  // Connect to MQTT Broker
  Serial.print("\nconnecting...");
  while (!client.connect("ESP32", "try", "try"))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected!");
  client.subscribe("/moekki"); // Subscribe to the mökki MQTT topic
}

// Wifi Info
void wifiInfo()
{
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Print incoming message to Serial Monitor
void messageReceived(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
}

// Sensor Info
void sensorInfo()
{
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode())
    Serial.println("ON");
  else
    Serial.println("OFF");
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
  wifiInfo();

  // Setup time
  timeClient.begin();
  timeClient.setTimeOffset(10800);

  // COnnect to MQTT Broker
  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);
  connect();

  // Start up the library
  sensors.begin();

  // Sensor Info
  sensorInfo();
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
    sensors.requestTemperatures(); // Send the command to get temperature readings

    // Build JSON
    // Init JSON
    const int capacity = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<capacity> doc;

    // Input Data
    doc["temperature"] = sensors.getTempCByIndex(0);
    doc["time"] = timeClient.getEpochTime();
    doc["postN"] = postN;

    // Serialize JSON
    String content;
    serializeJson(doc, content);

    // Publish to MQTT
    client.publish("/moekki", content);

    // Reset millis and increment postN
    lastMillis = millis();
    ++postN;
  }
}