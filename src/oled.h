#ifndef OLED_H
#define OLED_H

#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14
#define BATTERY 35

#define LOCK_SCREEN 0
#define ENROLL_SCREEN 1
#define GPS_SCREEN 2
#define ALARM_SCREEN 3

struct screens_t
{
    const char *lock_status;
    const char *enroll_status;
    const char *gps_status;
    const char *alarm_status;
};

extern screens_t screens;
extern int currentScreen;

void initOled();
void dannyWrite(const char *msg);
void resetDisplay();
void oledShutdown();

#endif