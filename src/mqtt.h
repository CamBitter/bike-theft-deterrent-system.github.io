#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <Adafruit_MQTT_Client.h>

extern WiFiClient client;
extern Adafruit_MQTT_Client mqtt;
extern Adafruit_MQTT_Publish gps_feed;

extern const char *aio_url;
extern const uint16_t aio_port;
extern const char *gps_channel;

void startMqttWifiTask(void);
void MqttWifiKeepAlive(void *parameters);
void publishGpsData(GPS_Data data);
bool isMqttConnected();

#endif
