#ifndef CONFIG_H
#define CONFIG_H

// Time to stay awake when woken by fingerprint sensor or accelerometer (in milliseconds)
#define AWAKE_TIME_MS 30000UL 
#define WIFI_TIMEOUT_MS 20000

// Interrupt pins
#define FINGERSENSOR_INTERRUPT_PIN GPIO_NUM_32
#define ACCELEROMETER_INTERRUPT_PIN GPIO_NUM_14
#define WAKEUP_PINS_BITMASK ((1ULL << ACCELEROMETER_INTERRUPT_PIN) | (1ULL << FINGERSENSOR_INTERRUPT_PIN))

// GPS pin (uses hardware serial on TX/RX)
#define GPS_UART 1
#define GPS_TX 17
#define GPS_RX 16

#endif