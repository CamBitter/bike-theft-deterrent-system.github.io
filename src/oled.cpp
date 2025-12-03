#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "oled.h"
#include "fingerprint.h"

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

int currentScreen = LOCK_SCREEN;

screens_t screens = {
    "LOCK:\nDISARMED",
    "Enroll\nFinger",
    "GPS:\n"};

void resetDisplay()
{
    display.clearDisplay();
    display.display();
    display.setCursor(0, 0);
}

void dannyWrite(const char *msg)
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(msg);
    display.display();
}

void oledTask(void *parameters)
{
    for (;;)
    {
        if (!digitalRead(BUTTON_A))
        {
            currentScreen = LOCK_SCREEN;
        }
        else if (!digitalRead(BUTTON_B))
        {
            currentScreen = ENROLL_SCREEN;
        }
        else if (!digitalRead(BUTTON_C))
        {
            currentScreen = GPS_SCREEN;
        }

        switch (currentScreen)
        {
        case 0:
            dannyWrite(screens.lock_status);
            break;

        case 1:
            dannyWrite(screens.enroll_status);
            break;

        case 2:
            dannyWrite(screens.gps_status);
            break;

        default:
            dannyWrite("hmm..");
            break;
        }
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
}

void startOledTask()
{
    xTaskCreate(
        oledTask,
        "Oled Task",
        5000,
        NULL,
        1,
        NULL);
}

void initOled()
{

    display.begin(0x3C, true);
    resetDisplay();
    display.setRotation(1);

    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
    pinMode(BUTTON_C, INPUT_PULLUP);
    pinMode(BATTERY, INPUT);

    // text display tests
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);

    dannyWrite("Bike lock \nonline.");
    startOledTask();
}
