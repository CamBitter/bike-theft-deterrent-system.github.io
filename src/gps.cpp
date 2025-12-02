#include <Adafruit_GPS.h>
#include <Arduino.h>
#include "config.h"
#include "gps.h"
#include "mqtt.h"

HardwareSerial GPS_Serial(GPS_UART);
Adafruit_GPS GPS(&GPS_Serial);
GPS_Data initialGPSData = {};

void initGPS()
{
  GPS_Serial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
}

GPS_Data readGPS()
{

  GPS_Data data = {};
  data.fix = 100;

  GPS.read();
  if (GPS.newNMEAreceived())
  {
    if (!GPS.parse(GPS.lastNMEA()))
      return data;
  }

  data.fix = GPS.fix;
  data.latitude = GPS.latitudeDegrees;
  data.longitude = GPS.longitudeDegrees;
  data.speed = GPS.speed;
  data.fixQuality = GPS.fixquality;
  data.satellites = GPS.satellites;
  data.hdop = GPS.HDOP;
  data.altitude = GPS.altitude;
  data.geoidHeight = GPS.geoidheight;
  data.hour = GPS.hour;
  data.minute = GPS.minute;
  data.seconds = GPS.seconds;
  data.angle = GPS.angle;

  return data;
}

void GpsTask(void *parameters)
{
  // HANDLE GPS DATA
  GPS_Data gpsData = readGPS();
  if (gpsData.fixQuality > 0)
  {
    // STORE INITIAL GPS STATE
    if (!initialGPSData.fix)
    {
      initialGPSData = gpsData;
      Serial.print("Stored initial GPS data, fix quality: ");
      Serial.print(gpsData.fixQuality);
      Serial.print(", # sats: ");
      Serial.println(gpsData.satellites);

      if (isMqttConnected())
      {
        publishGpsData(gpsData);
      }
    }
  }
  vTaskDelay(5000 / portTICK_PERIOD_MS);
}

void startGpsTask()
{
}