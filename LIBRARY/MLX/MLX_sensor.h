#ifndef MLX_SENSOR_H
#define MLX_SENSOR_H
#include <Wire.h>
#include <Adafruit_MLX90614.h>
void MLXsetup();
void readTemperature(float &object_temp, float &ambient_temp);
#endif