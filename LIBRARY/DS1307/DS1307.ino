#include"ds1307.h"
char result;
int delayInMinutes;
void setup () {
  ds1307setup();
  Serial.begin(9600);
}

void loop () {
  checkTime();
  delay(100);
}
