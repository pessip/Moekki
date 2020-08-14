// Import Libraries
#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Define Pin locations and temperature precision
#define ONE_WIRE_BUS 15
#define TEMPERATURE_PRECISION 9

// Setup instances
WiFiClient net;                      // WiFi Client
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

// Print incoming message to Serial Monitor
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

  // Start up the library
  sensors.begin();

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

  // // Pin Modes
  // pinMode(Led, OUTPUT);
  // pinMode(LED_BUILTIN, OUTPUT);
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
    Serial.print(" Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperature readings
    Serial.println("DONE");
    /********************************************************************/
    Serial.print("Temperature is: ");
    Serial.println(sensors.getTempCByIndex(0));
    String content = "Tempreature is: " + String(sensors.getTempCByIndex(0));
    lastMillis = millis();
    client.publish("/moekki", content);
    ++postN;
  }
}