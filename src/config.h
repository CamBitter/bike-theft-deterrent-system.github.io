#ifndef CONFIG_H
#define CONFIG_H

// Time to stay awake when woken by fingerprint sensor or accelerometer (in milliseconds)
#define AWAKE_TIME_MS 20000UL
#define WIFI_TIMEOUT_MS 20000UL

// Alarm thresholds
#define CHECK_LAST_MOVEMENT_MS 5000UL
#define MOVEMENT_DISTANCE_THRESHOLD 0.005

// Interrupt pins
#define FINGERSENSOR_INTERRUPT_PIN GPIO_NUM_27
#define ACCELEROMETER_INTERRUPT_PIN GPIO_NUM_33

#define WAKEUP_PINS_BITMASK ((1ULL << ACCELEROMETER_INTERRUPT_PIN) | (1ULL << FINGERSENSOR_INTERRUPT_PIN))

// GPS pin (uses hardware serial on TX/RX)
#define GPS_UART 2
#define GPS_TX 21
#define GPS_RX -1

#define FINGERPRINT_RX 16
#define FINGERPRINT_TX 17

// Alarm pin
#define ALARM_CONTROL_PIN 12

#endif