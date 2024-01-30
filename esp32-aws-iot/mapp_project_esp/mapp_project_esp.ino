#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
#include <string.h>
#include <cstring>
#include <math.h>
#include "secrets.h";
#include <WifiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "Wifi.h"


#define PIC_NOTIFY_PIN 10

void publishMessage(char type, double voltage, double ampere);
//type is as follows : 1;solar 2;hydro 3;wind
void messageHandler(String &topic, String &payload);
void serialPicRecv();
double Voltage, Current;
bool interrupt;

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);
void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}


void IRAM_ATTR picIsr() {
  serialPicRecv();
}
/*
Main - 0x60
Solar - 0x61
Grid - 0x62
Hydro - 0x63
*/
constexpr int MAIN_INST = 0x60;
constexpr int SOLAR_INST = 0x61;
constexpr int GRID_INST = 0x62;
constexpr int HYDRO_INST = 0x63;




void serialPicRecv() {
  if (Serial2.available()) {
    // Transmission follows a fixed format
    // The ESP32 can parse strings, so that will not be a problem
    // The PIC can just sprintf into a char buffer.
    // The value will just be the analog value (0-1023)
    // Example: (for main)
    // ~`0204&0456#0#
    // The future byte indicates whether to persist the connection, and everything must end with #.
    // '0': terminate. anything else: continue.
    // Receiving will terminate after # and a new connection is to be initiated.

    Serial2.readBytesUntil('~')
    char source = Serial2.read();
    String voltageRaw = Serial2.readStringUntil('&');
    String currentRaw = Serial2.readStringUntil('#');
    char future = Serial2.read();
    Serial2.readBytesUntil("#");

    String type;
    switch (source) {
      case SOLAR_INST:
        type = "solar";
        break;
      case HYDRO_INST:
        type = "hydro";
        break;
      case GRID_INST:
        type = "grid";
        break;
      case MAIN_INST:
        type = "main";
        break;
      default:
        Serial.println("Received unknown type " + String(source));
        type = "Error";
        return;
    }
    // voltage decoding (binary -> decimal)
    // for (int i = 1; i++; i < 15) {
    //   double tempV = pow(2, voltageB[i]);
    //   double Voltage = Voltage + tempV;
    // }
    // // current decoding (binary -> decimal)
    // for (int i = 1; i++; i < 15) {
    //   double tempA = pow(2, voltageB[i]);
    //   double Current = Current + tempA;
    // }

    // Voltage decoding
    int voltageAnalog = voltageRaw.toInt();
    int currentAnalog = currentRaw.toInt();

    // As of now 0-25V measurement.
    // But we should do 0-10V measurement.
    // So 0: 0V, 1023: 10V
    // 10/1023 * 1023 = 10
    
    float voltage = 10/1023.0 * voltageAnalog; // This value can be adjusted accordingly

    // 5A sensor
    // According to datasheet from Allegro Microsystems,
    // The sensor has a characteristic of 185 mV / A.
    // Max: 701, Min: 322.
    // 701: 5 A
    // 322: -5 A
    // 512: 0 A
    // https://www.allegromicro.com/~/media/files/datasheets/acs712-datasheet.ashx

    int ci1 = currentAnalog - 512;
    // 512: 5A
    // 0: 0A
    // 512 * 5/512 = 5
    // -512 * 5/512 = 5 
    float current = ci1 * 5/512.0;
    publishMessage(type, voltage, current);

    if (future == '0') {
      // continue!
      serialPicRecv();
    }
  }
}




void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  connectAWS();

  // The ESP interrupt will only be triggered on rising state.
  attachInterrupt(PIC_NOTIFY_PIN, picIsr, RISING);
}

void loop() {
  
}

void publishMessage(String type, double voltage, double ampere) {
  JsonDocument doc;
  doc["type"] = type;
  doc["voltage"] = voltage;
  doc["current"] = ampere;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);  // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char *message = doc["message"];
}