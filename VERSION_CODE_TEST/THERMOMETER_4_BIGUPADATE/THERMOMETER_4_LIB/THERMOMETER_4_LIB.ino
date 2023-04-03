#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <DS1307RTC.h>
#include "Fingerprint.h"
#include "oled_display.h"
#include <SD.h>
#include <SPI.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
const int chipSelect = 10;  // chọn chân kết nối với SD card reader
const byte buttonPin1 = 2;  // Sử dụng chân số 13 để kết nối với nút bấm
const byte buttonPin2 = 3;  // Sử dụng chân số 13 để kết nối với nút bấm
const byte enPin = 4;
const byte sensorPin = 5;
const byte buzzer = 8;
oled_display oled;
struct Data {
  float tempC;
  uint8_t id;
  uint8_t finger_id;
  tmElements_t tm;
  byte mode = 2;
  int late_minutes;
};
Data data;
volatile bool buttonPressed1 = false;
volatile bool buttonPressed2 = false;
void delay_millis(unsigned long ms) {
  unsigned long current_time = millis();  // Lấy thời điểm hiện tại
  while (millis() - current_time < ms) {  // Kiểm tra nếu thời gian chạy đã vượt quá khoảng thời gian cần delay
    // Chờ
  }
}
void setup() {
  Serial.begin(9600);
  SPI.begin();// Bắt đầu SPI
  pinMode(chipSelect, OUTPUT);// Chọn thẻ nhớ SD bằng chân kết nối
  oled.begin();
  fingerprintSetup();
  pinMode(sensorPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);  // Đặt chân số 2 là INPUT_PULLUP để kết nối nút bấm
  pinMode(buttonPin2, INPUT_PULLUP);  // Đặt chân số 3 là INPUT_PULLUP để kết nối nút bấm
  attachInterrupt(digitalPinToInterrupt(buttonPin1), buttonInterrupt1, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), buttonInterrupt2, FALLING);
  digitalWrite(buzzer, LOW);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH);// Kích hoạt cảm biến bằng cách đưa chân EN lên mức HIGH
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Không tìm thấy thẻ nhớ SD."));
    return;}
  mlx.begin();
}
void loop() {
  readstate();
  readFinger();
  readTemp();// Check if object detected by YS-29 sensor
  Time();// oled current time
  SDcard();
  data.tempC = 0;
  data.finger_id = 0;
}
void buttonInterrupt1() {
  buttonPressed1 = true;
}
void buttonInterrupt2() {
  buttonPressed2 = true;
}
void readstate() {
  byte buttonState1 = digitalRead(buttonPin1);
  if (buttonPressed1 == true && buttonState1 == LOW) {
    buttonPressed1 = false;
    data.mode = 3;
    Serial.print(data.mode);
    delay_millis(1000);
  }
  byte buttonState2 = digitalRead(buttonPin2);
  if (buttonPressed2 == true && buttonState2 == LOW) {
    buttonPressed2 = false;
    data.mode = 1;
    Serial.print(data.mode);
    delay_millis(1000);
  }
}
void readTemp() {
  byte sensorValue = digitalRead(sensorPin);
  if (sensorValue == LOW) {
    data.tempC = mlx.readObjectTempC();
    digitalWrite(buzzer, HIGH);
    delay_millis(250);
    digitalWrite(buzzer, LOW);
    oled.clear();
    oled.print_text_2x("\n", 0, 0);
    oled.print_text_2x("Temp: \n", 40, 40);
    oled.print_float_2x(data.tempC, 30, 40);
    // Update OLED oled
    delay_millis(1000);
  }
}
void readFinger() {
  if (data.mode == 1) {
    data.id = readnumber();
    if (data.id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    getFingerprintEnroll(data.id);
    oled.clear();
    oled.print_text_2x("ENROLLED!!", 0, 10);
    delay_millis(1000);
    data.mode = 2;
  }
  if (data.mode == 2) {
    getFingerprintID(data.finger_id);
    if (data.finger_id != 0) {
      oled.clear();
      oled.print_text_1x("\n", 1, 1);
      oled.print_text_1x("Found a print match!\n", 10, 10);
      oled.print_text_1x("Match found ID\n", 10, 15);
      oled.print_uint8t_1x(data.finger_id, 10, 20);
      delay_millis(1000);
    }
  }
  if (data.mode == 3) {
    data.id = readnumber();
    if (data.id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    deleteFingerprint(data.id);
    oled.clear();
    oled.print_text_2x("DELETED!", 0, 10);
    delay_millis(1000);
    data.mode = 2;
  }
}
void Time() {
  if (RTC.read(data.tm)) {
    if (data.finger_id != 0) {
      if (data.tm.Hour >= 7 && data.tm.Hour < 9) {
        // tính thời gian điểm danh so với 21h15p
        data.late_minutes = (data.tm.Hour - 8) * 60 + data.tm.Minute;
        if (data.late_minutes <= 0) {
          Serial.print(F("E"));
          oled.clear();
          oled.print_text_2x("Arrived\n", 10, 10);  // đến đúng giờ
          oled.print_text_2x("early", 12, 10);  // đến đúng giờ
          delay_millis(2000);
        } else if (data.late_minutes <= 15) {
          Serial.print(F("O"));
          oled.clear();
          oled.print_text_2x("On Time", 10, 10);  // đến sớm
          delay_millis(2000);
        } else {
          Serial.print(F("L\n"));
          oled.clear();
          oled.print_text_2x("Late: ", 10, 10);
          oled.print_int2x(data.late_minutes-15);
          oled.print_text2x("minutes");  // đến trễ
          delay_millis(2000);
        }
      } else if (data.tm.Hour < 7 || data.tm.Hour > 9) {
        Serial.print(F("Time out"));
        oled.clear();
        oled.print_text_2x("Time out", 10, 10);  // thời gian điểm danh đã kết thúc
        delay_millis(2000);
      }
    }
    oled.clear();
    oled.print_text_2x("Time: \n", 0, 10);
    oled.print_int2x(data.tm.Hour);
    oled.print_text2x(":");
    oled.print_int2x(data.tm.Minute);
    oled.print_text2x(":");
    oled.print_int2x(data.tm.Second);
    delay_millis(1000);
  }
}
void SDcard() {
  File dataFile = SD.open("data.csv", FILE_WRITE);  // Mở file data.txt và chế độ ghi
  if (dataFile) {                                   // Nếu file đã được mở
    if (data.finger_id != 0 || data.tempC != 0) {
      Serial.println(F("Opening file"));
      if (data.finger_id != 0) {
        dataFile.print(F("ID: "));        
        dataFile.print(data.finger_id);  // Lưu id vào file
      }
      dataFile.print(F(","));  // Phân cách giữa id và tempC
      if (data.tempC != 0) {
        dataFile.print(F("TEMP: "));
        dataFile.print(data.tempC);  // Lưu tempC vào file
      }
      dataFile.print(F(","));  // Phân cách
      dataFile.print(data.tm.Hour);
      dataFile.print(F(":"));
      dataFile.print(data.tm.Minute);
      dataFile.print(F(":"));
      dataFile.print(data.tm.Second);
      dataFile.println();  // Xuống dòng để lưu thông tin tiếp theo
    }
    dataFile.close();  // Đóng file
  } else {
    Serial.println(F("Error opening file"));  // In ra thông báo lỗi nếu không mở được file
  }
}
