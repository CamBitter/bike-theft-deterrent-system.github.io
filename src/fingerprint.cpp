#include "fingerprint.h"

bool enroll = false;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

void initFingerprint()
{
    Serial1.begin(57600, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
    finger.begin(57600);
    delay(5);

    if (finger.verifyPassword())
    {
        Serial.println("Found fingerprint sensor!");
    }
    else
    {
        Serial.println("Did not find fingerprint sensor :(");
        while (1)
        {
            delay(1);
        }
    }

    Serial.println(F("Reading sensor parameters"));
    finger.getParameters();
    Serial.print(F("Sensor contains "));
    finger.getTemplateCount();
    Serial.print(finger.templateCount);
    Serial.println(F(" templates"));

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
    Serial.println("Finger detected...");

    if (enroll && !isArmed)
    {
        Serial.println("=== ENROLL MODE ===");
        enrollFingerprint();
        return false;
    }
    else
    {
        Serial.println("=== CHECKING FINGERPRINT ===");
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
        Serial.println("Failed to capture image");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(1000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return 255;
    }

    Serial.println("Image captured");

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)
    {
        Serial.println("Failed to convert image");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(1000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return 255;
    }

    Serial.println("Image converted, searching...");

    p = finger.fingerSearch();

    if (p == FINGERPRINT_OK)
    {
        Serial.println("*** MATCH FOUND! ***");
        Serial.print("ID #");
        Serial.print(finger.fingerID);
        Serial.print(" | Confidence: ");
        Serial.println(finger.confidence);

        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);

        return finger.fingerID;
    }
    else if (p == FINGERPRINT_NOTFOUND)
    {
        Serial.println("*** NO MATCH ***");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return 255;
    }
    else
    {
        Serial.println("Error during search");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(1000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return 255;
    }
}

bool enrollFingerprint()
{
    Serial.println("enrolling");

    int id = getNextFreeID();
    if (id == -1)
    {
        Serial.println("ERROR Database full");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }

    int p = -1;
    Serial.println("Place finger");
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);

    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
    }

    p = finger.image2Tz(1);
    if (p != FINGERPRINT_OK)
    {
        Serial.println("bad image, please restart");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }

    Serial.println("Remove finger");
    finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_PURPLE);
    delay(2000);

    p = 0;
    while (p != FINGERPRINT_NOFINGER)
    {
        p = finger.getImage();
    }

    p = -1;
    Serial.println("Place same finger again");
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);

    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
    }

    p = finger.image2Tz(2);
    if (p != FINGERPRINT_OK)
    {
        Serial.println("bad image, please restart");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }

    p = finger.createModel();
    if (p != FINGERPRINT_OK)
    {
        Serial.println("bad convert, please restart");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }

    Serial.print("Storing finger #");
    Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Finger enrolled");
        finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_BLUE, 5);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);
        return true;
    }
    else
    {
        Serial.println("saving failed");
        finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
        delay(2000);
        finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
        return false;
    }
}

void setArmed(bool &isArmed, bool state)
{
    isArmed = state;
    Serial.print("Armed: ");
    Serial.println(isArmed ? "true" : "false");
}

void setEnrollMode(bool state)
{
    enroll = state;
    Serial.print("Enroll mode: ");
    Serial.println(enroll ? "true" : "false");
}

void toggleLock(bool &isArmed)
{
    Serial.println("\n*** TOGGLING LOCK ***");
    isArmed = !isArmed;

    Serial.println("Lock toggled!");

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
        Serial.print("Deleted ID #");
        Serial.println(id);
        return true;
    }
    else
    {
        Serial.print("Failed to delete ID #");
        Serial.println(id);
        return false;
    }
}

bool deleteAllFingerprints()
{
    uint8_t p = finger.emptyDatabase();

    if (p == FINGERPRINT_OK)
    {
        Serial.println("Database emptied!");
        return true;
    }
    else
    {
        Serial.println("Failed to empty database");
        return false;
    }
}

void enrollPressed()
{
    enroll = true;
}