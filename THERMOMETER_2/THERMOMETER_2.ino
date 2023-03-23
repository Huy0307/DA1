#include <SPI.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Fingerprint.h>
#include <MLX_sensor.h>
#include <SimpleTimer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
SimpleTimer timer;
uint8_t id;
int enPin = 4;
int sensorPin = 5;
byte buttonPin = 7;  // Sử dụng chân số 13 để kết nối với nút bấm
uint8_t pass;
uint8_t finger_id;
byte mode = 2;
float object_temp, ambient_temp;
int sensorValue;
byte buzzer = 6;
void setup() {
  Serial.begin(9600);
  // Wire.begin();
  fingerprintSetup();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  MLXsetup();
  pinMode(buttonPin, INPUT_PULLUP);  // Đặt chân số 2 là INPUT_PULLUP để kết nối nút bấm
  pinMode(sensorPin, INPUT);
  pinMode(enPin, OUTPUT);
  // Kích hoạt cảm biến bằng cách đưa chân EN lên mức HIGH
  digitalWrite(enPin, HIGH);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  // timer.setInterval(100L, checkTime);
  timer.setInterval(10000L, CheckAdd);     //Set an internal timer every 10sec to check if there a new fingerprint in the website to add it.
  timer.setInterval(15000L, CheckDelete);  //Set an internal timer every 15sec to check wheater there an ID to delete in the website.
}
void readbutton() {
  if (digitalRead(buttonPin) == 0) {
    delay(200);
    mode++;
    if (mode > 3) {
      mode = 1;
    }
  }
}

void loop() {
  timer.run();
  checkTime();
  sensorValue = digitalRead(sensorPin);
  if (sensorValue == LOW) {
    Serial.print("Detected\n");
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
    readTemperature(object_temp, ambient_temp);
    Serial.print("Object temp: ");
    Serial.print(object_temp);
    Serial.print(", Ambient temp: ");
    Serial.println(ambient_temp);
    display.clearDisplay();
    display.setCursor(25, 10);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print(object_temp);
    display.println(" C");
    display.display();
    delay(1000);
    if (object_temp >= 37.5) {
      digitalWrite(buzzer, HIGH);
      Serial.print("ALERT!!!!\n ");
      delay(1500);
    }
    // digitalWrite(led_pin, LOW);     //LED tắt
    digitalWrite(buzzer, LOW);  //Buzzer tắt
    // digitalWrite(laser, LOW);       //laser tắt
    display.clearDisplay();
    display.setCursor(25, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println(" Waiting for motion.....");
    display.display();
    delay(1000);
  }
  if (mode == 2) {
    CheckID();
    delay(2000);
  }
}
void CheckAdd() {
  if (mode == 1) {
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    getFingerprintEnroll(id);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("ENROLL SUCCESS!!\n");
    display.println("Enroll ID #");
    display.println(id);
    display.display();
    delay(1000);
  }
}
void CheckDelete() {
  if (mode == 3) {
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    deleteFingerprint(id);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("DELETE SUCCESS!!");
    display.display();
    delay(1000);
  }
}
void CheckID() {
  if (mode == 2) {
    getFingerprintID(finger_id);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Found a print match!");
    display.println("Match found ID ");
    display.print(finger_id);
    display.display();
    delay(1000);
  }
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