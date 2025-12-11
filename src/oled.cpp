#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "oled.h"
#include "fingerprint.h"
#include "main.h"

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

int currentScreen = LOCK_SCREEN;

screens_t screens = {
    "LOCK:\nDISARMED",
    "Enroll\nFinger",
    "GPS:\n",
    "ALARM\nACTIVATED\nPUB SAFE\nINBOUND"};

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
    char buffer[64];
    for (;;)
    {
        if (!digitalRead(BUTTON_A))
        {
            currentScreen = LOCK_SCREEN;
            resetSleep();
        }
        else if (!digitalRead(BUTTON_B))
        {
            currentScreen = ENROLL_SCREEN;
            resetSleep();
        }
        else if (!digitalRead(BUTTON_C))
        {
            currentScreen = GPS_SCREEN;
            resetSleep();
        }

        switch (currentScreen)
        {
        case LOCK_SCREEN:
            screens.lock_status = isArmed ? "Lock:\nARMED" : "Lock:\nDISARMED";
            dannyWrite(screens.lock_status);
            break;

        case ENROLL_SCREEN:
            dannyWrite(screens.enroll_status);
            break;

        case GPS_SCREEN:
            dannyWrite(screens.gps_status);
            break;

        case ALARM_SCREEN:
            dannyWrite(screens.alarm_status);
            break;

        default:
            dannyWrite("hmm..");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void oledShutdown()
{
    display.clearDisplay();
    display.display();
    display.oled_command(SH110X_DISPLAYOFF);
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

    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);

    screens.lock_status = isArmed ? "Lock:\nARMED" : "Lock:\nDISARMED";
    screens.enroll_status = isArmed ? "Unlock\nTo Enroll" : "Enroll\nFinger";

    dannyWrite("Bike lock \nonline.");
    startOledTask();
}
