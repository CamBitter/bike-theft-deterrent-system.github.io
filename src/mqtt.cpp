/*
 * Authors: Cam Bitter
 * MQTT client to push GPS data to Adafruit IO Dashboard
*/

#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include "gps.h"
#include <esp_task_wdt.h>
#include "mqtt.h"
#include <stdlib.h>
#include "secrets.h"

const char* aio_url = "io.adafruit.com";
const uint16_t aio_port = 1883;
const char* gps_channel = "CamBitter/feeds/Bike-GPS";

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, aio_url, aio_port, aio_username, aio_key);
Adafruit_MQTT_Publish gps_feed(&mqtt, gps_channel);

void initMqtt(){

  Serial.print("Connecting to wifi");
 
  WiFi.begin(ssid, key);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    esp_task_wdt_reset();
    delay(100);

    if (millis() - start > 5000) {
        Serial.println("WiFi timeout");
        break;
    }
  }

  delay(500); // Ensure WiFi is stable before MQTT

  Serial.print("Connecting to mqtt");
  while (!mqtt.connected()) {
    int res = mqtt.connect();
    yield();
    Serial.print(".");
  }
  Serial.println("connected!");
  Serial.println("mqtt init done.");
}

void MqttKeepAlive() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping MQTT keepalive.");
    return;
  }
  if (!mqtt.connected()) {
    Serial.println("MQTT not connected, attempting reconnect.");
    mqtt.connect();
    return;
  }
  mqtt.ping();
}

void publishGpsData(GPS_Data data) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping publish.");
    return;
  }
  if (!mqtt.connected()) {
    Serial.println("MQTT not connected, skipping publish.");
    return;
  }
  char payload[100];
  char* p = payload;

  dtostrf(data.speed, 2, 6, p);
  p += strlen(p);
  *p++ = ',';

  dtostrf(data.latitude, 2, 6, p);
  p += strlen(p);
  *p++ = ','; 

  dtostrf(data.longitude, 2, 6, p);
  p += strlen(p);
  *p++ = ','; 

  dtostrf(data.altitude, 2, 6, p);
  p += strlen(p);

  *p = '\0';

  Serial.print("Publishing CSV: ");
  Serial.println(payload);

  if (gps_feed.publish(payload)) {
    Serial.println("Published GPS CSV!");
  } else {
    Serial.println("Failed to publish.");
  }
}

void checkConnections() {

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, key);
  } 

  if (mqtt.connected()) {
    mqtt.ping();

  } else {
    int res = mqtt.connect();
  }
}

bool isConnected() {
  return (WiFi.status() != WL_CONNECTED) & mqtt.connected();
}