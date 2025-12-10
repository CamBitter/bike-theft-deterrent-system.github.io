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
#include "fingerprint.h"
#include <WiFi.h>
#include <Adafruit_MQTT_Client.h>
#include "secrets.h"
#include "oled.h"

// RTC_DATA persists through Deep Sleep
RTC_DATA_ATTR unsigned long wakeStart = 0;
RTC_DATA_ATTR bool isArmed = false;

/*
 * WAKE INITIAL
 */
void setup()
{

  Serial.begin(115200);

  delay(2000);

  /* INIT ACCELEROMETER */
  if (!lis.begin(LIS3DH_ADDR))
  {
    Serial.println("[LIS3DH]: Could not start.");
  }
  Serial.println("[LIS3DH]: Connected.");

  /* INIT ACCELEROMETER */
  init_ACC();

  /* INIT GPS */
  initGPS();
  startGpsTask();

  /* INIT FINGERPRINT */
  initFingerprint();

  /* INIT MQTT */
  startMqttWifiTask();

  /* INIT OLED */
  initOled();

  /* STORE INTIAL GPS READING */

  /* WAKEUP FROM INTERRUPT */
  esp_sleep_wakeup_cause_t wakeupCause = esp_sleep_get_wakeup_cause();

  switch (wakeupCause)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("[Wakeup]: EXT0 (Fingerprint)");
    fingerLightWakeup();
    break;

  case ESP_SLEEP_WAKEUP_EXT1:
  {
    uint64_t status = esp_sleep_get_ext1_wakeup_status();
    if (status & (1ULL << ACCELEROMETER_INTERRUPT_PIN))
    {
      Serial.println("[Wakeup]: EXT1 (Accelerometer)");
      clearAccelerometerInterrupt();
    }
    break;
  }

  default:
    Serial.println("[Wakeup]: Not EXT0/EXT1/Timer");
    break;
  }

  // Configure fingerprint sensor on EXT0 (active-low)
  pinMode(FINGERSENSOR_INTERRUPT_PIN, INPUT);
  rtc_gpio_pullup_en(FINGERSENSOR_INTERRUPT_PIN);              // Enable internal pull-up
  esp_sleep_enable_ext0_wakeup(FINGERSENSOR_INTERRUPT_PIN, 0); // 0 = wake on LOW

  // Accelerometer stays on EXT1 (active-high)
  pinMode(ACCELEROMETER_INTERRUPT_PIN, INPUT);
  rtc_gpio_pulldown_en(ACCELEROMETER_INTERRUPT_PIN);
  esp_sleep_enable_ext1_wakeup(1ULL << ACCELEROMETER_INTERRUPT_PIN, ESP_EXT1_WAKEUP_ANY_HIGH);

  wakeStart = millis();
}

/*
 *  NORMAL AWAKE MONITORING MODE
 */
void loop()
{

  // HANDLE ACCELEROMETER INTERRUPT
  if (digitalRead(ACCELEROMETER_INTERRUPT_PIN) == HIGH)
  {
    Serial.println("[LIS3DH]: Interrupt reset.");
    wakeStart = millis();
    clearAccelerometerInterrupt();
  }

  // IF NO ACTIVITY, GO TO SLEEP
  if ((millis() - wakeStart) > AWAKE_TIME_MS && !currentlyHandlingFinger)
  {
    Serial.println("[Main]: Starting deep sleep.");
    fingerLightSleep();
    oledShutdown();
    delay(50);
  }
