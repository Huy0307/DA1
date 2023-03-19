#ifndef DS1307_H
#define DS1307_H
#include <RTClib.h>
#include <Wire.h>
void ds1307_setup();
void checkArrivalTime(char& result, int& delayInMinutes);
void checkLeaveTime(char& result1, int& earlyInMinutes);
#endif