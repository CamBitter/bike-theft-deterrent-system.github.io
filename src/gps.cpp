#include <Adafruit_GPS.h>
#include <Arduino.h>
#include "config.h"
#include "gps.h"
#include "mqtt.h"
#include "oled.h"

HardwareSerial GPS_Serial(GPS_UART);
Adafruit_GPS GPS(&GPS_Serial);
GPS_Data initialGPSData = {};
GPS_Data lastGPSData = {};

/******************************************
  Source - https://stackoverflow.com/a/10205532
  Posted by MrTJ, modified by community. See post 'Timeline' for change history
  Retrieved 2025-12-10, License - CC BY-SA 3.0
*/

#include <math.h>
#define earthRadiusKm 6371.0

double deg2rad(double deg)
{
  return (deg * M_PI / 180);
}

/**
 * Returns the distance between two points on the Earth.
 * Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
 * @return The distance between the inital and most recent GPS reading
 */
double getGpsMovementDistance()
{
  if (!initialGPSData.fix || !lastGPSData.fix)
  {
    return -1;
  }
  double lat1r, lon1r, lat2r, lon2r, u, v;
  lat1r = deg2rad(initialGPSData.latitude);
  lon1r = deg2rad(initialGPSData.longitude);
  lat2r = deg2rad(lastGPSData.latitude);
  lon2r = deg2rad(lastGPSData.longitude);
  u = sin((lat2r - lat1r) / 2);
  v = sin((lon2r - lon1r) / 2);
  return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}
/*********************************************************************/

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
  for (;;)
  {
    // HANDLE GPS DATA
    GPS_Data gpsData = readGPS();
    if (gpsData.fixQuality > 0)
    {
      // STORE INITIAL GPS STATE
      if (!initialGPSData.fix)
      {
        initialGPSData = gpsData;
        Serial.print("[GPS]: Initial fix quality: ");
        Serial.print(gpsData.fixQuality);
        Serial.print(", sats: ");
        Serial.println(gpsData.satellites);
      }

      lastGPSData = gpsData;

      // Concatenate GPS stats and display on OLED
      char buffer[64];
      snprintf(buffer, sizeof(buffer), "Fix:%d\nLat:%.5f\nLon:%.5f", gpsData.fixQuality, gpsData.latitude, gpsData.longitude);
      screens.gps_status = buffer;

      if (isMqttConnected())
      {
        Serial.println("[GPS] Publishing GPS Data to Adafruit IO");
        publishGpsData(gpsData);
      }
    }

    // NO GPS
    screens.gps_status = "GPS:\nNO SATS";

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void startGpsTask()
{
  Serial.println("[GPS] Started task.");
  xTaskCreate(
      GpsTask,
      "Gps_Task",
      4096,
      NULL,
      1,
      NULL);
}
