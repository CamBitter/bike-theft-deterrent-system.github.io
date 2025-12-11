#include <Adafruit_GPS.h>
#include <Arduino.h>
#include "config.h"
#include "gps.h"
#include "mqtt.h"
#include "oled.h"

// Define the Serial Object
HardwareSerial GPS_Serial(GPS_UART); // Ensure GPS_UART is defined in config.h (e.g., 1 or 2)
Adafruit_GPS GPS(&GPS_Serial);

GPS_Data initialGPSData = {};
GPS_Data lastGPSData = {};

// Source - https://stackoverflow.com/a/10205532
// Posted by MrTJ, modified by community. See post 'Timeline' for change history
// Retrieved 2025-12-10, License - CC BY-SA 3.0
#include <math.h>
#define EARTH_RADIUS_KM 6371.0

double deg2rad(double deg)
{
  return deg * M_PI / 180.0;
}

/**
 * Returns the distance between two points on the Earth.
 * Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
 * @return The distance between the two points in meters
 */
double getGpsMovementDistance()
{
  if (!initialGPSData.fix || !lastGPSData.fix)
    return -1;

  double lat1r = deg2rad(initialGPSData.latitude);
  double lon1r = deg2rad(initialGPSData.longitude);
  double lat2r = deg2rad(lastGPSData.latitude);
  double lon2r = deg2rad(lastGPSData.longitude);

  double u = sin((lat2r - lat1r) / 2);
  double v = sin((lon2r - lon1r) / 2);

  return 1000 * 2.0 * EARTH_RADIUS_KM * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}

void initGPS()
{
  GPS_Serial.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);
  Serial.println("[GPS] Initialized");
}

void GpsTask(void *parameters)
{
  Serial.println("[GPS Task] Started");

  for (;;)
  {
    while (GPS_Serial.available())
    {
      char c = GPS.read();

      if (GPS.newNMEAreceived())
      {
        if (!GPS.parse(GPS.lastNMEA()))
        {
          continue;
        }

        lastGPSData.fix = GPS.fix;
        lastGPSData.satellites = (int)GPS.satellites;
        lastGPSData.fixQuality = (int)GPS.fixquality;

        // Time (UTC)
        lastGPSData.hour = GPS.hour;
        lastGPSData.minute = GPS.minute;
        lastGPSData.seconds = GPS.seconds;

        if (GPS.fix)
        {
          lastGPSData.latitude = (double)GPS.latitudeDegrees;
          lastGPSData.longitude = (double)GPS.longitudeDegrees;

          lastGPSData.speed = GPS.speed * 1.15078; // Convert Knots to MPH
          lastGPSData.angle = GPS.angle;
          lastGPSData.altitude = GPS.altitude;

          lastGPSData.hdop = GPS.HDOP;
          lastGPSData.geoidHeight = GPS.geoidheight;

          char buffer[64];
          snprintf(buffer, sizeof(buffer), "# Sats:%d\n%.6f\n%.6f\nMoved %.1fm", lastGPSData.satellites, lastGPSData.latitude, lastGPSData.longitude, getGpsMovementDistance());
          screens.gps_status = buffer;

          // Set Home Point if not set yet
          if (!initialGPSData.fix && lastGPSData.satellites > 6)
          {
            initialGPSData = lastGPSData;
            Serial.println("[GPS] Home Point Recorded!");
          }

          publishGpsData(lastGPSData);
        }
        else
        {
          char buffer[64];
          snprintf(buffer, sizeof(buffer), "GPS: Searching\nSats: %d", lastGPSData.satellites);
          screens.gps_status = buffer;
        }
      }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ---------------------------------------------------------
// Start GPS task
// ---------------------------------------------------------
void startGpsTask()
{
  Serial.println("[GPS] Started task.");
  initGPS();
  xTaskCreate(
      GpsTask,
      "Gps_Task",
      4096, // Stack size
      NULL,
      1, // Priority
      NULL);
}