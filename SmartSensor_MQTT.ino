/*
  ArduinoMqttClient - WiFi Echo

  This example connects to a MQTT broker and subscribes to a single topic,
  it also publishes a message to the same topic once a second.
  When a message is received it prints the message to the Serial Monitor.

  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev2 board

  This example code is in the public domain.
*/
#include "DHT.h"
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define REPORT_MQTT_SEPERATE true
#define REPORT_MQTT_JSON true
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


const char broker[] = "0580dbe4c5a2458e82d55b87587a6c9b.s1.eu.hivemq.cloud";
int port = 8883;

const char topicHumi[]  = "data/humi";
const char topicTemp[]  = "data/temp";
const char topicHUMI[]  = "data/HUMI";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  Serial.print("Subscribing to topic: ");
  Serial.println(topicTemp);
  Serial.println(topicHumi);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(topicTemp);
  mqttClient.subscribe(topicHumi);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(topicTemp);
  Serial.println(topicHumi);
  Serial.println();
}

void send_data(float data, char dataName[], char topicName[]) {


    Serial.print("Sending message to topic: ");
    Serial.println(topicName);
    Serial.println(dataName);
    Serial.print(data);
    

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topicName);
    mqttClient.print(dataName);
    mqttClient.print(data);
    mqttClient.endMessage();

    Serial.println();

  
}


void jsonfile(float data, float data2, char topics[]){
  StaticJsonDocument<32> doc;
  char output[55];
  doc["t"] = data;
  doc["h"] = data2;
  serializeJson(doc, output);
  mqttClient.beginMessage(topics, (unsigned long)measureJson(doc));
  Serial.println(output);
  serializeJson(doc, mqttClient);
  mqttClient.endMessage();


} 

  void loop(){


    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;
    float temp = dht.readTemperature();
    send_data(temp,"Temperature :", topicTemp);
    float humi = dht.readHumidity();
    send_data(humi,"Humidite :", topicHumi);
    jsonfile(temp,humi,topicHUMI);
    }
    
    
  }

