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
#include "config.h"
#include "secrets.h"

const char *aio_url = "io.adafruit.com";
const uint16_t aio_port = 1883;
const char *gps_channel = "CamBitter/feeds/Bike-GPS";

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, aio_url, aio_port, aio_username, aio_key);
Adafruit_MQTT_Publish gps_feed(&mqtt, gps_channel);

void MqttWifiKeepAlive(void *parameters)
{
  for (;;)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Connecting to wifi");
      WiFi.begin(ssid, key);

      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS)
      {
        vTaskDelay(250 / portTICK_PERIOD_MS); // prevent watchdog
      };
      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("[WiFi] Failed to establish connection");
        vTaskDelay(20000 / portTICK_PERIOD_MS);
        continue;
      }
    }

    if (WiFi.status() == WL_CONNECTED && !mqtt.connected())
    {

      mqtt.connect();

      unsigned long startAttemptTime = millis();
      while (!mqtt.connected() && millis() - startAttemptTime < WIFI_TIMEOUT_MS)
      {
        vTaskDelay(250 / portTICK_PERIOD_MS); // prevent watchdog
      };
      if (!mqtt.connected())
      {
        Serial.println("[MQTT] Failed to establish connection");
        vTaskDelay(20000 / portTICK_PERIOD_MS);
        continue;
      }
    }

    if (WiFi.status() == WL_CONNECTED && mqtt.connected())
    {
      mqtt.ping(); // ping server to keep connection alive
    }

    // Serial.print("Connected to WiFi and MQTT: ");
    // Serial.println(WiFi.localIP());
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

void startMqttWifiTask()
{
  xTaskCreatePinnedToCore(
      MqttWifiKeepAlive,
      "Keep MQTT and WiFi alive",
      15000,
      NULL,
      1,
      NULL,
      CONFIG_ARDUINO_RUNNING_CORE);
}

void publishGpsData(GPS_Data data)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected, skipping publish.");
    return;
  }
  if (!mqtt.connected())
  {
    Serial.println("MQTT not connected, skipping publish.");
    return;
  }
  char payload[100];
  char *p = payload;

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

  if (gps_feed.publish(payload))
  {
    Serial.println("Published GPS CSV!");
  }
  else
  {
    Serial.println("Failed to publish.");
  }
}

bool isMqttConnected()
{
  return WiFi.status() == WL_CONNECTED && mqtt.connected();
}