#include "ds1307.h"
#include "display.h"
char result, result1;
uint8_t hour;
uint8_t minute;
uint8_t second;
int delayInMinutes;
void setup() {
  ds1307_setup();
  // initOLED();
}
void loop() {
  // Time(hour,minute,second);
  checkArrivalTime(result, delayInMinutes);
  // checkLeaveTime(result1, earlyInMinutes);
  switch (result) {
    case 'O':
      Serial.println("Arrived on time");
      // displayData_text(0,0,1,"On Time");
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

  delay(1000);
}
