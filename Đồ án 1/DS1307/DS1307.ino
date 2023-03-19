#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;
DateTime scheduledTime(0, 0, 0, 8, 0, 0);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  // Cài đặt thời gian gốc
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

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
    } else {
      result = 'O';  // 'O' là ký tự tượng trưng cho đến đúng giờ
    }
  } else {
    result = 'N';  // 'N' là ký tự tượng trưng cho không phải thời gian điểm danh
  }
}
void loop() {
  char result;         // Biến để lưu kết quả đến đúng giờ hay đến trễ ('O', 'L' hoặc 'E')
  int delayInMinutes;  // Biến để lưu thời gian đến trễ (nếu có)

  checkArrivalTime(result, delayInMinutes);  // Gọi hàm để kiểm tra thời gian đến đúng giờ hay đến trễ

  // In ra kết quả
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
  delay(1000);
}
