#ifndef MLX_SENSOR_H
#define MLX_SENSOR_H
#include <Wire.h>//Thư viện hỗ trợ giao tiếp I2C, Search ở GitHUb, Link: https://github.com/esp8266/Arduino (Nhiều tác giả)
#include <Adafruit_MLX90614.h>//Thư viện hỗ trợ giao tiếp cảm biến MLX90614, Search ở GitHub, Link: https://github.com/adafruit/Adafruit-MLX90614-Library (Adafruit)
void MLXsetup();
void readTemperature(float &object_temp, float &ambient_temp);
#endif