#include "MLX_sensor.h"
float object_temp, ambient_temp;

void setup() {
  Serial.begin(9600);
  MLXsetup();
}

void loop() {
  readTemperature(object_temp, ambient_temp);
  Serial.print("Object temp: ");
  Serial.print(object_temp);
  Serial.print(", Ambient temp: ");
  Serial.println(ambient_temp);
  delay(1000);
}
