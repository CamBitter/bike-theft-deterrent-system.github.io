#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>

extern bool enroll;
extern Adafruit_Fingerprint finger;
extern bool currentlyHandlingFinger;

void initFingerprint();
bool checkFingerprint();
uint8_t getFingerprintID(uint8_t p);
bool enrollFingerprint();
void setArmed(bool state);
void setEnrollMode(bool state);
void toggleLock();
int getNextFreeID();
bool deleteFingerprint(uint8_t id);
bool deleteAllFingerprints();
void enrollPressed();
void fingerLightWakeup();
void fingerLightSleep();
void startFingerprintTask();

#endif