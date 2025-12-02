/*
 * Author: Cam Bitter
 * LIS3DH datasheet: https://cdn-learn.adafruit.com/assets/assets/000/085/846/original/lis3dh.pdf?1576396666
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "accelerometer.h"

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

unsigned int readRegister(byte reg)
{
  Wire.beginTransmission(LIS3DH_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(LIS3DH_ADDR, 1);
  return Wire.read();
}

void writeRegister(byte reg, byte data)
{
  Wire.beginTransmission(LIS3DH_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void init_ACC()
{
  writeRegister(0x20, 0x3F);                                     // CTRL_REG1: enable X,Y,Z axes, 25hz, enable LPen bit
  writeRegister(0x21, 0x09);                                     // CTRL_REG2: HPF enabled
  writeRegister(0x22, 0x40);                                     // CTRL_REG3: AOI1 routed to INT1
  writeRegister(0x23, 0x00);                                     // CTRL_REG4: +/-2g, disable high-resolution mode
  writeRegister(0x24, 0x08);                                     // CTRL_REG5: no latching
  writeRegister(0x32, 0x7F & ACCELEROMETER_INTERRUPT_THRESHOLD); // INT1_THS: threshold
  writeRegister(0x33, 0x00);                                     // INT1_DURATION
  writeRegister(0x30, 0x2A);                                     // INT1_CFG: enable XL/YL/ZL interrupt
}

void clearAccelerometerInterrupt()
{
  readRegister(0x21);
  readRegister(0x26);
  readRegister(LIS3DH_REG_INT1SRC);
}