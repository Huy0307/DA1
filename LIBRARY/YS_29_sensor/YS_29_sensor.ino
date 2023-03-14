#include "ir_sensor.h"
int sensorValue;
void setup() {
  ir_setup();
  Serial.begin(9600);
}

void loop() {
  readSensor(sensorValue);
  if (sensorValue == LOW) {
    Serial.print("Detected\n");
  }
  delay(2000);
}
