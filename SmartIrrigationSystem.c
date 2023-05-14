//Copyright All rihgts reserved

#include <WiFi.h>
#include "ThingsBoard.h"
#include <ArduinoJson.h>
#define WIFI_SSID           "PTCLBB"
#define WIFI_PASSWORD       "epteck@123S"
#define TOKEN               "PccSh3SoCTfY0DEyd0TM"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"
// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;
int motor_pin1 = 26;
int motor_pin2 = 25;
int Moisture1 = 0;
int Moisture2 = 0;
int analog_input1 = 0;
int analog_input2 = 0;
int sensor_pin1 = 36;
int sensor_pin2 = 39;
int pump1_status = 0;
int pump2_status = 0;
void setup() 
{
  Serial.begin(9600);
  pinMode(motor_pin1, OUTPUT);
  pinMode(motor_pin2, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  InitWiFi();
}
void loop()
 {
  StaticJsonDocument<200> doc;
  if (WiFi.status() != WL_CONNECTED) 
  {
    reconnect();
  }
  if (!tb.connected())
   {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) 
    {
      Serial.println("Failed to connect");
      return;
    }
    Serial.println("Connected to thingsboard");
  }
  analog_input1 = analogRead(sensor_pin1);
  analog_input2 = analogRead(sensor_pin2);
  Moisture1 = 100 - ((analog_input1 * 100) / 4095);
  Moisture2 = 100 - ((analog_input2 * 100) / 4095);
  //============Sensor1======================
  if (Moisture1 < 40)
  {
    digitalWrite(motor_pin1, LOW);
    pump2_status = 1;
  }
  else if (Moisture1 > 60)
  {
    digitalWrite(motor_pin1, HIGH);
    pump2_status = 0;
  }
  //============Sensor2======================
  if (Moisture2 < 40)
  {
    digitalWrite(motor_pin2, LOW);
    pump1_status = 1;
  }
  else if (Moisture2 > 60)
  {
    digitalWrite(motor_pin2, HIGH);
    pump1_status = 0;
  }
  doc["Moisture1"] = Moisture1;
  doc["Moisture2"] = Moisture2;
  Serial.println();
  serializeJson(doc, Serial);
  tb.sendTelemetryFloat("Moisture1", Moisture1);
  tb.sendTelemetryFloat("Moisture2", Moisture2);
  tb.sendTelemetryInt("Pump1_status", pump1_status);
  tb.sendTelemetryInt("Pump2_status", pump2_status);
  tb.loop();
  delay(1100);
}
void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
   {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
}
void reconnect() 
{
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED)
   {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
     {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}