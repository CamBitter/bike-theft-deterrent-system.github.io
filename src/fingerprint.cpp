#include "fingerprint.h"
#include "config.h"
#include "main.h"

bool enroll = false;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
bool currentlyHandlingFinger = false;

void fingerLightWakeup()
{
    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 20, FINGERPRINT_LED_PURPLE, 3);
}

void fingerLightSleep()
{
    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 20, FINGERPRINT_LED_RED, 3);
}

void initFingerprint()
{
    Serial1.begin(57600, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
    finger.begin(57600);
    delay(5);

    if (finger.verifyPassword())
    {
        Serial.println("[Finger] Connected.");
    }
    else
    {
        Serial.println("[Finger] Failed to connect.");
        return;
    }

    Serial.println("[Finger]: Reading parameters.");
    finger.getParameters();
    Serial.print("[Finger]: Templates: ");
    finger.getTemplateCount();
    Serial.print(finger.templateCount);
    Serial.println(".");

    finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_PURPLE);
}

bool checkFingerprint(bool &isArmed)
{
    uint8_t p = finger.getImage();

    if (p == FINGERPRINT_NOFINGER)
    {
        return false;
    }

    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);
    Serial.println("[Finger]: Detected.");

    if (enroll && !isArmed)
    {
        Serial.println("[Finger]: Enroll mode.");
        enrollFingerprint();
        return false;
    }
    else
    {
        Serial.println("[Finger]: Checking.");
        uint8_t result = getFingerprintID();
        if (result != 255)
        {
            toggleLock(isArmed);
            return true;
        }
    }
    return false;
}

uint8_t getFingerprintID()
{
    uint8_t p = finger.getImage();

    if (p != FINGERPRINT_OK)
    {
        Serial.println("[Finger]: Capture failed.");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(1000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return 255;
    }

    Serial.println("[Finger]: Image captured.");

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)
    {
        Serial.println("[Finger]: Convert failed.");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(1000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return 255;
    }

    Serial.println("[Finger]: Searching.");

    p = finger.fingerSearch();

    if (p == FINGERPRINT_OK)
    {
        Serial.println("[Finger]: Match found.");
        Serial.print("[Finger]: ID ");
        Serial.print(finger.fingerID);
        Serial.print(", confidence: ");
        Serial.println(finger.confidence);

        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);

        return finger.fingerID;
    }
    else if (p == FINGERPRINT_NOTFOUND)
    {
        Serial.println("[Finger]: No match.");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return 255;
    }
    else
    {
        Serial.println("[Finger]: Search error.");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(1000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return 255;
    }
}

bool enrollFingerprint()
{
    Serial.println("[Finger]: Enrolling.");

    int id = getNextFreeID();
    if (id == -1)
    {
        Serial.println("[Finger]: Database full.");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }

    int p = -1;
    Serial.println("[Finger]: Place finger.");
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);

    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
    }

    p = finger.image2Tz(1);
    if (p != FINGERPRINT_OK)
    {
        Serial.println("[Finger]: Bad image.");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }

    Serial.println("[Finger]: Remove finger.");
    finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_PURPLE);
    delay(2000);

    p = 0;
    while (p != FINGERPRINT_NOFINGER)
    {
        p = finger.getImage();
    }

    p = -1;
    Serial.println("[Finger]: Place again.");
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);

    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
    }

    p = finger.image2Tz(2);
    if (p != FINGERPRINT_OK)
    {
        Serial.println("[Finger]: Bad image.");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }

    p = finger.createModel();
    if (p != FINGERPRINT_OK)
    {
        Serial.println("[Finger]: Bad convert.");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }

    Serial.print("[Finger]: Store ID ");
    Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK)
    {
        Serial.println("[Finger]: Enrolled.");
        finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_BLUE, 5);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);
        return true;
    }
    else
    {
        Serial.println("[Finger]: Save failed.");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }
}

void setArmed(bool &isArmed, bool state)
{
    isArmed = state;
    Serial.print("[Finger]: Armed: ");
    Serial.println(isArmed ? "true" : "false");
}

void setEnrollMode(bool state)
{
    enroll = state;
    Serial.print("[Finger]: Enroll: ");
    Serial.println(enroll ? "true" : "false");
}

void toggleLock(bool &isArmed)
{
    Serial.println("[Finger]: Toggling lock.");
    isArmed = !isArmed;

    Serial.println("[Finger]: Toggled.");

    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_BLUE, 3);
    delay(1000);
}

int getNextFreeID()
{
    for (int id = 1; id <= 127; id++)
    {
        uint8_t p = finger.loadModel(id);
        if (p == FINGERPRINT_PACKETRECIEVEERR)
        {
            return id;
        }
    }
    return -1;
}

bool deleteFingerprint(uint8_t id)
{
    uint8_t p = finger.deleteModel(id);

    if (p == FINGERPRINT_OK)
    {
        Serial.print("[Finger]: Deleted ID ");
        Serial.println(id);
        return true;
    }
    else
    {
        Serial.print("[Finger]: Delete failed ID ");
        Serial.println(id);
        return false;
    }
}

bool deleteAllFingerprints()
{
    uint8_t p = finger.emptyDatabase();

    if (p == FINGERPRINT_OK)
    {
        Serial.println("[Finger]: Database emptied.");
        return true;
    }
    else
    {
        Serial.println("[Finger]: Empty failed.");
        return false;
    }
}

void enrollPressed()
{
    enroll = true;
}

void fingerTask(void *pvParameters)
{
    for (;;)
    {

        // If sensor active (LOW = finger)
        if (digitalRead(FINGERSENSOR_INTERRUPT_PIN) == LOW && !currentlyHandlingFinger)
        {

            currentlyHandlingFinger = true;
            wakeStart = millis();

            if (enroll && !isArmed)
            {
                enrollFingerprint(); // long blocking, safe inside task
                enroll = false;
            }
            else
            {
                checkFingerprint(isArmed);
            }

            currentlyHandlingFinger = false;
        }

        vTaskDelay(pdMS_TO_TICKS(50)); // small poll
    }
}

void startFingerprintTask()
{
    xTaskCreate(
        fingerTask,
        "finger_Task",
        4096,
        NULL,
        1,
        NULL);
}