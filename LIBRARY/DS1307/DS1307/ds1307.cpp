#include <SPI.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
void ds1307setup() {
  Wire.begin();
}
void checkTime() {
  tmElements_t tm;
  if (RTC.read(tm)) {
    Serial.print(tm.Hour);
    Serial.print(':');
    Serial.print(tm.Minute);
    Serial.print(':');
    Serial.print(tm.Second);
    Serial.print(' ');
    Serial.print(tm.Day);
    Serial.print('/');
    Serial.print(tm.Month);
    Serial.print('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.print('\n');
  } else {
    if (RTC.chipPresent()) {
      Serial.print("DS1307 stopped,run set time");
    } else {
      Serial.print("DS1307 read error,check circuit");
    }
  }
}