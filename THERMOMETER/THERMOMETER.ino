#include <display.h>
#include <ir_sensor.h>
#include <Fingerprint.h>
#include <MLX_sensor.h>
#include <TaskScheduler.h>
const int buttonPin = 8;     // Pin của nút bấm
int buttonState = HIGH;      // Trạng thái ban đầu của nút bấm (nút chưa được nhấn)
int lastButtonState = HIGH;  // Trạng thái trước đó của nút bấm
uint8_t id;
uint8_t finger_id;
int success;
int mode = 2;
int buzzer = 6;
int sensorValue;
float object_temp, ambient_temp;
// Khai báo các hằng số và biến
const int task1Interval = 500;   // Thời gian chạy tác vụ 1 là 0.5 giây
const int task2Interval = 500;  // Thời gian chạy tác vụ 2 là 1 giây

// Khởi tạo các tác vụ
Task task1(task1Interval, TASK_FOREVER, []() {
  // Đo nhiệt độ bằng cảm biến MLX90614
  readSensor(sensorValue);
  if (sensorValue == LOW) {
    Serial.print("Detected\n");
    readTemperature(object_temp, ambient_temp);
    Serial.print("Object temp: ");
    Serial.print(object_temp);
    Serial.print(", Ambient temp: ");
    Serial.println(ambient_temp);
    digitalWrite(buzzer,HIGH);
    delay(250);
    digitalWrite(buzzer,LOW);
    displayData_all_float(5,0,1,"Temperature: ",object_temp);
    delay(500);
  }
});

Task task2(task2Interval, TASK_FOREVER, []() {
  // Thực hiện quét vân tay bằng cảm biến R308
  buttonState = digitalRead(buttonPin);  // Đọc trạng thái hiện tại của nút bấm
  // Nếu nút bấm được nhấn
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Tăng mode lên 1 đơn vị
    mode++;
    // Nếu mode vượt quá giá trị 3, đặt lại về 1
    if (mode > 3) {
      mode = 1;
    }
  }
  // Lưu trạng thái hiện tại của nút bấm
  lastButtonState = buttonState;
  if (mode == 1) {
    id = readnumber();
    if (id == 0) {
      return;
    }
    displayData_all_uint8_t(5,0,1,"ID Enroll : ", id);
    delay(1000);
    getFingerprintEnroll(id,success);
    if(success==1){
      displayData_text(5, 0, 1, "Success Enroll!!");
    }
    delay(2000);
  } else if (mode == 2) {
    getFingerprintID(finger_id);
    displayData_all_uint8_t(5,0,1,"Found a ID Match: ",finger_id);
    delay(2000);
  } else if (mode == 3) {
    id = readnumber();
    if (id == 0) {return;}
    displayData_all_uint8_t(5,0,1,"ID Delete: ", id);
    delay(1000);
    deleteFingerprint(id);
    displayData_text(5, 0, 1, "Success Delete!!");
    delay(2000);
  }
  delay(50);
});
// Khởi tạo TaskScheduler
Scheduler taskScheduler;
void setup() {
  // Khởi tạo kết nối Serial
  Serial.begin(9600);
  initOLED();
  fingerprintSetup();
  ir_setup();
  MLXsetup();
  pinMode(buzzer,OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // Cấu hình chân nút bấm là INPUT_PULLUP để tránh nhiễu
  Wire.begin();
  // Thêm các tác vụ vào TaskScheduler
  taskScheduler.addTask(task1);
  taskScheduler.addTask(task2);
  // Bắt đầu chạy các tác vụ
  task1.enable();
  task2.enable();
}
void loop() {
  // Chạy TaskScheduler
  taskScheduler.execute();
}
