#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

// Threshhold to trigger an interrupt on INT output, desired_threshold = milli-g's * 16
#define ACCELEROMETER_INTERRUPT_THRESHOLD 0x10 

// I2C address
#define LIS3DH_ADDR 0x18

#include <Adafruit_LIS3DH.h>
extern Adafruit_LIS3DH lis;

void init_ACC();
unsigned int readRegister(byte reg);
void writeRegister(byte reg, byte data);
void clearAccelerometerInterrupt();

#endif