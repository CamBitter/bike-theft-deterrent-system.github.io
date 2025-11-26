#ifndef GPS_H
#define GPS_H

struct GPS_Data {
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

void initGPS();
GPS_Data readGPS();

#endif 