#ifndef IR_SENSOR_H
#define IR_SENSOR_H
#include <Wire.h>
void ir_setup();
void readSensor(int &sensorValue);
#endif