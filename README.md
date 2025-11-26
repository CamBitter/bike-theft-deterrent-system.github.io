# Bike Theft Prevention System (BTPS)

A proof-of-concept embedded system designed to deter bike theft, authenticate owners with biometrics, and provide location tracking after a theft event.

---

## Overview

BTPS arms and disarms through a fingerprint sensor. When armed, it continuously monitors motion using an accelerometer. If sustained abnormal movement is detected, the system:

1. Activates a car-horn–based auditory deterrent  
3. Sends location updates to an Adafruit IO dashboard  

An onboard OLED displays system status and fingerprint-management options.

---

## Motivation

Bike losses on campus are frequent, and traditional locks are slow, bulky, and often used incorrectly. Recovery tools like pubsafe rarely help. BTPS aims to provide a compact, always-mounted, fast-arming system that deters theft and helps locate a stolen bike.

---

## System Components

- **Fingerprint Authentication (FPA)**  
  Controls arming/disarming using registered profiles.

- **Bike Theft Event Detection Device (BTEDD)**  
  Accelerometer sampled over a sustained period to avoid false positives.

- **Auditory Deterrent**  
  12V car horn controlled via MOSFET.

- **GPS Tracking Module**  
  Transmits location data via campus Wi-Fi using Adafruit IO and MQTT.

- **OLED Display + LED Indicator**  
  Shows lock status and supports fingerprint profile management.

---

## Development Milestones

### 1. Fingerprint Authentication (~1 week)  
Connect sensor → MCU; enroll and verify fingerprints.  
*Owners: Danny, River*

### 2. Accelerometer Integration (~1 week)  
Configure SPI → read motion → detect BTE.  
*Owners: River, Cam*

### 3. Horn + Power System (~1 week)  
MOSFET wiring → 12V battery → MCU control → regulator wiring.  
*Owners: Cam, Danny*

### 4. GPS Tracking (~1 week)  
Attach GPS → configure sleep/wake → push location to Adafruit IO.  
*Owners: River, Cam*

### 5. Adafruit IO Dashboard (~1 week)  
Set up dashboard → receive GPS + status data.  
*Owners: Cam, River*

### 6. OLED Display (~1 week)  
Display armed/disarmed status → fingerprint admin → LED indicator.  
*Owners: Danny, Cam*

---

## Hardware List

| Item                | Supplier | Cost    |
|---------------------|----------|---------|
| Fingerprint sensor  | Adafruit | $39.95  |
| Feather OLED        | Adafruit | $15.95  |
| GPS Module          | Adafruit | $29.95  |
| Car Horn            | Amazon   | $12.99  |
| Battery Pack        | Amazon   | $18.56  |
| Multicolor LED      | Vaccari  | —       |
| Accelerometer       | Adafruit | $4.95   |
| Regulator           | Adafruit | $1.25   |
| MOSFET              | Adafruit | $2.25   |
| **Total**           | —        | **$124.85** |

---

## Behavior Summary

- Detect sustained large movement → trigger horn for 1 minute → auto-shutoff after 1 minute  
- GPS sends location data to Adafruit IO Dashboard
- Fingerprint reader pinned to MCU (standard Adafruit wiring)

---

## License

MIT

