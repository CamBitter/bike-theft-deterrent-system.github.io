#ifndef GPS_H
#define GPS_H

struct GPS_Data
{
    bool fix;
    double latitude;
    double longitude;
    float speed;
    int fixQuality;
    int satellites;
    float hdop;
    float altitude;
    float geoidHeight;
    int hour, minute, seconds;
    float angle;
};

extern GPS_Data initialGPSData;

void initGPS();
GPS_Data readGPS();
void startGpsTask();

#endif