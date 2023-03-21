#include <ir_sensor.h>
#include <Fingerprint.h>
#include <display.h>
#include <MLX_sensor.h>
#include <ds1307.h>
uint8_t id;
int buttonPin = 13;  // Sử dụng chân số 2 để kết nối với nút bấm
int success;
uint8_t finger_id;
int mode = 2;
char resullt;
int delayInMinutes;
float object_temp, ambient_temp;
int sensorValue;
int buzzer = 6;
int detect;
int f;
int a;
void setup() {
  Serial.begin(9600);
  MLXsetup();
  int mode = 2;//phải khai báo int mode trong setup nếu không sẽ bị xung đột dẫn đến treo hệ thống
  pinMode(buttonPin, INPUT_PULLUP);  // Đặt chân số 2 là INPUT_PULLUP để kết nối nút bấm
  // digitalWrite(buttonPin, HIGH);
  initOLED();
  fingerprintSetup(f);
  if (f == 1) {
    displayData_text(0, 5, 1, "Fingerprint Sensor Ready");
  } else {
    displayData_text(0, 5, 1, "Fingerprint Sensor Not Ready");
  }
  ir_setup();
  ds1307_setup();
  pinMode(buttonPin, INPUT_PULLUP);  // Cấu hình chân nút bấm là INPUT_PULLUP để tránh nhiễu
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
}
void readbutton(){
  if (digitalRead(buttonPin) == LOW) {  // Kiểm tra xem nút bấm đã được bấm hay chưa
    mode++;                             // Tăng giá trị mode lên 1
    if (mode > 3) {                     // Nếu mode đạt giá trị 3
      mode = 1;                         // Đặt lại giá trị mode là 1
    }
  }
}
void loop() {
  // readbutton();

  readSensor(sensorValue);
  if (sensorValue == LOW) {
    Serial.print("Detected\n");
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
    readTemperature(object_temp, ambient_temp);
    Serial.print("Object temp: ");
    Serial.print(object_temp+1.65);
    Serial.print(", Ambient temp: ");
    Serial.println(ambient_temp);
    displayData_all_float(0,0,1,"Temperature",object_temp+1.65);
    delay(1000);
  }
  if (mode == 1) {
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    getFingerprintEnroll(id, success);
    if (success == 1) {
      displayData_text(0, 0, 1, "ENROLL SUCCESS!!");
      displayData_all_uint8(0, 0, 1, "Enroll ID # ", id);
    } else if (success == 0) {
      displayData_text(0, 0, 1, "ENROLL FAILED!!");
    } else {
      displayData_text(0, 0, 1, "NO FINGER, DO AGAIN!!");
    }
  } else if (mode == 2) {
    getFingerprintID(finger_id, detect);
    if (detect == 1) {
      displayData_text(0, 0, 1, "Found Match ID");
      displayData_all_uint8(0, 0, 1, "Match found ID ", finger_id);
    } 
    else {
      displayData_text(0, 0, 1, "NOT FOUND, DO AGAIN!!");
    }
  } else {
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    deleteFingerprint(id, a);
    if (a == 1) {
      displayData_text(0, 0, 1, "DELETE SUCCESS!!");
    }
  }
}
