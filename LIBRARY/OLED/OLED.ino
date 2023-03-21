#include"display.h"
float mode = 2;
void setup() {
  Serial.begin(9600);
  initOLED();
}

void loop() {
  displayData_text(10,15,1,"String data");
  delay(2000);
  displayData_text(0,0,1,"String data");
  delay(2000);
  displayData_all_float(0,0,1,"MODE:",mode);
  delay(2000);
}
