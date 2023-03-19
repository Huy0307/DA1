#include "ds1307.h"
char result, result1;
int delayInMinutes, earlyInMinutes;
void setup() {
  ds1307_setup();
}
void loop() {
  checkArrivalTime(result, delayInMinutes);
  checkLeaveTime(result1, earlyInMinutes);
  switch (result) {
    case 'O':
      Serial.println("Arrived on time");
      break;
    case 'L':
      Serial.print("Arrived ");
      Serial.print(delayInMinutes);
      Serial.println(" minutes late");
      break;
    case 'E':
      Serial.print("Arrived ");
      Serial.print(delayInMinutes);
      Serial.println(" minutes early");
      break;
    case 'N':
      Serial.print("None ");
      Serial.print(delayInMinutes);
      Serial.print("\n");
      break;
  }
  switch (result1) {
    case 'D':
      Serial.println("Do Full Time");
      break;
    case 'S':
      Serial.print("Leave ");
      Serial.print(earlyInMinutes);
      Serial.println(" minutes late");
      break;
    case 'N':
      Serial.print("None ");
      Serial.print(delayInMinutes);
      Serial.print("\n");
      break;
  }
  delay(1000);
}
