#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Adafruit_MLX90614.h>
#include <DS1307RTC.h>
#include <Fingerprint.h>
#include <SD.h>
#include <SPI.h>
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
// Define proper RST_PIN if required.
#define RST_PIN -1
SSD1306AsciiAvrI2c oled;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
const int chipSelect = 10;  // chọn chân kết nối với SD card reader
const byte buttonPin1 = 2;  // Sử dụng chân số 13 để kết nối với nút bấm
const byte buttonPin2 = 3;  // Sử dụng chân số 13 để kết nối với nút bấm
const byte enPin = 4;
const byte sensorPin = 5;
const byte buzzer = 8;
byte mode = 2;
float tempC;
uint8_t id;
uint8_t finger_id;
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
  // Bắt đầu SPI
  SPI.begin();
  // Chọn thẻ nhớ SD bằng chân kết nối
  pinMode(chipSelect, OUTPUT);
  oled.setFont(Adafruit5x7);
  fingerprintSetup();
  pinMode(sensorPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);  // Đặt chân số 2 là INPUT_PULLUP để kết nối nút bấm
  pinMode(buttonPin2, INPUT_PULLUP);  // Đặt chân số 3 là INPUT_PULLUP để kết nối nút bấm
  attachInterrupt(digitalPinToInterrupt(buttonPin1), buttonInterrupt1, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), buttonInterrupt2, FALLING);
  digitalWrite(buzzer, LOW);
  pinMode(enPin, OUTPUT);
  // Kích hoạt cảm biến bằng cách đưa chân EN lên mức HIGH
  digitalWrite(enPin, HIGH);
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Không tìm thấy thẻ nhớ SD."));
    return;
  }
// Initialize oled
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else   // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif  // RST_PIN >= 0
  mlx.begin();
}
void buttonInterrupt1() {
  buttonPressed1 = true;
}
void buttonInterrupt2() {
  buttonPressed2 = true;
}
void loop() {
  byte buttonState1 = digitalRead(buttonPin1);
  if (buttonPressed1 == true && buttonState1 == LOW) {
    buttonPressed1 = false;
    mode = 3;
    Serial.print(mode);
    delay_millis(1000);
  }
  byte buttonState2 = digitalRead(buttonPin2);
  if (buttonPressed2 == true && buttonState2 == LOW) {
    buttonPressed2 = false;
    mode = 1;
    Serial.print(mode);
    delay_millis(1000);
  }
  // Check if object detected by YS-29 sensor
  // oled current time
  tmElements_t tm;
  if (RTC.read(tm)) {
    oled.clear();
    oled.set2X();
    oled.setCursor(0, 10);
    oled.print(F("Time: \n"));
    oled.print(tm.Hour);
    oled.print(F(":"));
    oled.print(tm.Minute);
    oled.print(F(":"));
    oled.println(tm.Second);
    delay_millis(1000);
  }
  byte sensorValue = digitalRead(sensorPin);
  if (sensorValue == LOW) {
    tempC = mlx.readObjectTempC();
    digitalWrite(buzzer, HIGH);
    delay_millis(250);
    digitalWrite(buzzer, LOW);
    oled.clear();
    oled.set2X();
    oled.print("\n");
    oled.setCursor(40, 40);
    oled.print(F("Temp: \n"));
    oled.setCursor(30, 40);
    oled.println(tempC);
    // Update OLED oled
    delay_millis(1000);
  }
  if (mode == 1) {
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    getFingerprintEnroll(id);
    oled.clear();
    oled.set2X();
    oled.setCursor(0, 10);
    oled.println(F("ENROLLED!!"));
    delay_millis(1000);
    mode = 2;
  }
  if (mode == 2) {
    getFingerprintID(finger_id);
    if (finger_id != 0) {
      oled.clear();
      oled.set1X();
      oled.println(F("\n"));
      oled.set1X();
      oled.setCursor(10, 10);
      oled.println(F("Found a print match!\n"));
      oled.set1X();
      oled.setCursor(10, 15);
      oled.println(F("Match found ID\n"));
      oled.set1X();
      oled.setCursor(10, 20);
      oled.print(finger_id);
      delay_millis(1000);
    }
    finger_id = 0;
  }
  if (mode == 3) {
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    deleteFingerprint(id);
    oled.clear();
    oled.set2X();
    oled.setCursor(0, 10);
    oled.println(F("DELETED!"));
    delay_millis(1000);
    mode = 2;
  }
  File dataFile = SD.open("data.csv", FILE_WRITE);  // Mở file data.txt và chế độ ghi
  if (dataFile) {                                   // Nếu file đã được mở
    Serial.println(F("Opening file"));
    dataFile.print(id);                             // Lưu id vào file
    dataFile.print(",");                            // Phân cách giữa id và tempC
    dataFile.print(tempC);                          // Lưu tempC vào file
    dataFile.print(",");                            // Phân cách
    dataFile.print(tm.Hour);
    dataFile.print(":");
    dataFile.print(tm.Minute);
    dataFile.print(":");
    dataFile.print(tm.Second);
    dataFile.println();                             // Xuống dòng để lưu thông tin tiếp theo
    dataFile.close();                               // Đóng file
  } else {
    Serial.println(F("Error opening file"));  // In ra thông báo lỗi nếu không mở được file
  }
}