#ifndef DISPLAY_H
#define DISPLAY_H
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
void initOLED();
void displayData_text(int x, int y, int z, String data);
void displayData_number_float(int x, int y, int z, float data);
void displayData_number_int(int x, int y, int z, int data);
void displayData_all_float(int x, int y, int z, String label, float data);
void displayData_all_int(int x, int y, int z, String label, int data);
#endif