/*
 * Authors: Cam Bitter, Danny Smith, River Costello
 * Resources: 
 *  Deep sleep: https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
 *  ESP32 Pinout: https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts
*/

#include "accelerometer.h"
#include "gps.h"
#include "config.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "mqtt.h"
#include <WiFi.h>
#include <Adafruit_MQTT_Client.h>
#include "secrets.h"

// RTC_DATA persists through Deep Sleep
RTC_DATA_ATTR unsigned long wakeStart = 0;
RTC_DATA_ATTR bool isArmed = false;
GPS_Data initialGPSData = {};

void checkConnections(void);

unsigned long lastWifiAttempt = 0;
const unsigned long WIFI_RECONNECT_INTERVAL = 2000;  // ms

unsigned long lastMqttAttempt = 0;
const unsigned long MQTT_RECONNECT_INTERVAL = 2000;  // ms

/* 
 * WAKE INITIAL
*/
void setup() {

  Serial.begin(115200); 
  while (!Serial); 

  delay(500);

  /* INIT ACCELEROMETER */
  if (!lis.begin(LIS3DH_ADDR)) {
    Serial.println("Could not start LIS3DH");
  }
  Serial.println("Connected to LIS3DH");

  /* INIT ACCELEROMETER */
  init_ACC();

  /* INIT GPS */
  initGPS();

  /* INIT MQTT */
  initMqtt();

  /* INIT OLED */

  /* STORE INTIAL GPS READING */

  /* WAKEUP FROM INTERRUPT */
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1) {
    uint64_t wakeStatus = esp_sleep_get_ext1_wakeup_status();

    if (wakeStatus & (1ULL << ACCELEROMETER_INTERRUPT_PIN)) {
        Serial.println("Accelerometer triggered wake-up");

    }
    if (wakeStatus & (1ULL << FINGERSENSOR_INTERRUPT_PIN)) {
        Serial.println("Fingerprint triggered wake-up");

    }
    
    wakeStart = millis();
  }

  // Configure wakeup interrupts for fingerprint sensor and accelerometer
  pinMode(FINGERSENSOR_INTERRUPT_PIN, INPUT);
  pinMode(ACCELEROMETER_INTERRUPT_PIN, INPUT);
  esp_sleep_enable_ext1_wakeup(WAKEUP_PINS_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
  rtc_gpio_pulldown_en(FINGERSENSOR_INTERRUPT_PIN);
  rtc_gpio_pulldown_en(ACCELEROMETER_INTERRUPT_PIN);
}

/*
 *  NORMAL AWAKE MONITORING MODE
*/
void loop() {

  // HANDLE ACCELEROMETER INTERRUPT
  if (digitalRead(ACCELEROMETER_INTERRUPT_PIN) == HIGH) {
    Serial.println("Restarting interrupt count down.");
    wakeStart = millis(); 
    clearAccelerometerInterrupt();
  }

  // HANDLE FINGER SENSOR INTERRUPT
  if (digitalRead(FINGERSENSOR_INTERRUPT_PIN) == HIGH) {

  }

  // HANDLE WIFI & MQTT
  checkConnections();

  // HANDLE GPS DATA
  GPS_Data gpsData = readGPS();
  if (gpsData.fixQuality > 0) {
    // STORE INITIAL GPS STATE
    if (!initialGPSData.fix) {
      initialGPSData = gpsData;
      Serial.print("Stored initial GPS data, fix quality: ");
      Serial.print(gpsData.fixQuality);
      Serial.print(", # sats: ");
      Serial.println(gpsData.satellites);

      if (isConnected()) {
        publishGpsData(gpsData);
      }
    }
  }

  // IF NO ACTIVITY, GO TO SLEEP
  if ((millis() - wakeStart) > AWAKE_TIME_MS) {
    Serial.println("Timer expired, going back to deep sleep...");
    esp_deep_sleep_start();
  }

  delay(50);
}
