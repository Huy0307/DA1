#include"display.h"
void setup() {
  Serial.begin(9600);
  initOLED();
}

void loop() {
  displayData_text(10,15,1,"String data");
  delay(2000);
}
