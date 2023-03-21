#ifndef DS1307_H
#define DS1307_H
#include <RTClib.h>
#include <Wire.h>
void ds1307_setup();
void checkArrivalTime(char& result, int& delayInMinutes);
// void Time(uint8_t &hour,uint8_t &minute,uint8_t &second);
// void checkLeaveTime(char& result1, int& earlyInMinutes);
#endif