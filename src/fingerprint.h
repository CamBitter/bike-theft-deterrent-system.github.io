#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>

#define FINGERPRINT_RX 16
#define FINGERPRINT_TX 17

extern bool enroll;
extern Adafruit_Fingerprint finger;
extern bool currentlyHandlingFinger;

void initFingerprint();
bool checkFingerprint(bool &isArmed);
uint8_t getFingerprintID(uint8_t p);
bool enrollFingerprint();
void setArmed(bool &isArmed, bool state);
void setEnrollMode(bool state);
void toggleLock(bool &isArmed);
int getNextFreeID();
bool deleteFingerprint(uint8_t id);
bool deleteAllFingerprints();
void enrollPressed();
void fingerLightWakeup();
void fingerLightSleep();
void startFingerprintTask();

#endif