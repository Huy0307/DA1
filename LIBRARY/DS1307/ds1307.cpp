#include "ds1307.h"
RTC_DS1307 rtc;
DateTime scheduledTime(0, 0, 0, 8, 0, 0);
void ds1307_setup() {
  Wire.begin();
  rtc.begin();
  // Cài đặt thời gian gốc
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}
// void Time(uint8_t &hour,uint8_t &minute,uint8_t &second){
//   DateTime now = rtc.now();
//   Serial.print("Current time: ");
//   Serial.print(now.hour(), DEC);
//   Serial.print(':');
//   Serial.print(now.minute(), DEC);
//   Serial.print(':');
//   Serial.println(now.second(), DEC);
// }
void checkArrivalTime(char& result, int& delayInMinutes) {
  DateTime now = rtc.now();
  Serial.print("Current time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  // Kiểm tra xem có phải thời gian đến điểm danh hay không
  if (now.hour() >= 6 && (now.hour() < 9 || (now.hour() == 9 && now.minute() == 0))) {
    if (now.hour() >= 8) {
      delayInMinutes = (now.hour() - 8) * 60 + now.minute();  // Tính toán thời gian đến trễ bao nhiêu phút
      if (delayInMinutes == 0) {
        result = 'O';  // 'O' là ký tự tượng trưng cho đến đúng giờ
      } else {
        if (delayInMinutes > 0) {
          result = 'L';  // 'L' là ký tự tượng trưng cho đến trễ
        } else {
          result = 'E';                      // 'E' là ký tự tượng trưng cho đến sớm
          delayInMinutes = -delayInMinutes;  // Đổi dấu của thời gian đến sớm
        }
      }
    } 
  } else {
    result = 'N';  // 'N' là ký tự tượng trưng cho không phải thời gian điểm danh
  }
}
