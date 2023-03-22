#include "display.h"
#include "Arduino.h"
Adafruit_SSD1306 display(OLED_RESET);

void initOLED() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // 0x3C là địa chỉ của màn hình OLED
  display.clearDisplay();
}

void displayData_text(int x, int y, int z, String data) {
  display.clearDisplay();
  display.setTextSize(z);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(data);
  display.display();
  delay(1000);
}

void displayData_number_float(int x, int y, int z, float data) {
  display.clearDisplay();
  display.setTextSize(z);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(data);
  display.display();
  delay(1000);
}

void displayData_number_int(int x, int y, int z, int data) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(data);
  display.display();
  delay(1000);
}

void displayData_all_float(int x, int y, int z, String label, float data) {
  display.clearDisplay();
  display.setTextSize(z);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.print(label);
  display.print(": ");
  display.println(data);
  display.display();
  delay(1000);
}

void displayData_all_int(int x, int y, int z, String label, int data) {
  display.clearDisplay();
  display.setTextSize(z);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.print(label);
  display.print(": ");
  display.println(data);
  display.display();
  delay(1000);
}
void displayData_all_uint8(int x, int y, int z, String label, uint8_t data) {
  display.clearDisplay();
  display.setTextSize(z);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.print(label);
  display.print(": ");
  display.println(data);
  display.display();
  delay(1000);
}
