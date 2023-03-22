#include <ir_sensor.h>
#include <SPI.h>
#include <Fingerprint.h>
#include <display.h>
#include <MLX_sensor.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <ds1307.h>
uint8_t id;
byte buttonPin = 7;  // Sử dụng chân số 13 để kết nối với nút bấm
byte success;
uint8_t pass;
uint8_t finger_id;
byte mode = 2;
float object_temp, ambient_temp;
int sensorValue;
byte buzzer = 6;
byte detect;
byte f;
byte a;
void setup() {
  Serial.begin(9600);
  fingerprintSetup(f);
  MLXsetup();
  initOLED();
  ds1307setup();
  pinMode(buttonPin, INPUT_PULLUP);  // Đặt chân số 2 là INPUT_PULLUP để kết nối nút bấm
  ir_setup();
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
}
void readbutton(){
  if (digitalRead(buttonPin) == 0) {
    delay(200);
    mode++;
    if (mode > 3) {  // Nếu mode đạt giá trị 3
      mode = 1;      // Đặt lại giá trị mode là 1
    }
    return mode;
  }
}
void loop() {
  readbutton();
  checkTime();
  Serial.println(mode);
  readSensor(sensorValue);
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
    displayData_all_float(0, 0, 1, "Temperature", object_temp);
    delay(1000);
  }
  // if (mode == 1) {
  //   id = readnumber();
  //   if (id == 0) {  // ID #0 not allowed, try again!
  //     return;
  //   }
  //   getFingerprintEnroll(id, success);
  //   if (success == 1) {
  //     displayData_text(0, 0, 1, "ENROLL SUCCESS!!");
  //     displayData_all_uint8(0, 0, 1, "Enroll ID # ", id);
  //   } else if (success == 0) {
  //     displayData_text(0, 0, 1, "ENROLL FAILED!!");
  //   } else {
  //     displayData_text(0, 0, 1, "NO FINGER, DO AGAIN!!");
  //   }
  // }
  if (mode == 2) {
    getFingerprintID(finger_id, detect);
    if (detect == 1) {
      displayData_text(0, 0, 1, "Found Match ID");
      displayData_all_uint8(0, 0, 1, "Match found ID", finger_id);
    } else {
      displayData_text(0, 0, 1, "NOT FOUND, DO AGAIN!!");
    }
  }
  // if (mode == 3) {
  //   id = readnumber();
  //   if (id == 0) {  // ID #0 not allowed, try again!
  //     return;
  //   }
  //   deleteFingerprint(id, a);
  //   if (a == 1) {
  //     displayData_text(0, 0, 1, "DELETE SUCCESS!!");
  //   }
  // }
}