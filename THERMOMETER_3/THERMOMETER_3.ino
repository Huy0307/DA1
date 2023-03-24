#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Fingerprint.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
byte buttonPin = 8;  // Sử dụng chân số 13 để kết nối với nút bấm
int enPin = 4;
int sensorPin = 5;
byte buzzer = 6;
byte mode = 2;
uint8_t id;
uint8_t finger_id;
int sensorValue;
float tempC;
unsigned long lastButtonTime = millis();

void delay_millis(unsigned long ms) {
  unsigned long current_time = millis();  // Lấy thời điểm hiện tại
  while (millis() - current_time < ms) {  // Kiểm tra nếu thời gian chạy đã vượt quá khoảng thời gian cần delay
    // Chờ
  }
}

void setup() {
  Serial.begin(9600);
  fingerprintSetup();
  pinMode(sensorPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // Đặt chân số 2 là INPUT_PULLUP để kết nối nút bấm
  digitalWrite(buzzer, LOW);
  pinMode(enPin, OUTPUT);
  // Kích hoạt cảm biến bằng cách đưa chân EN lên mức HIGH
  digitalWrite(enPin, HIGH);
  // Initialize display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay_millis(2000);

  // Initialize I2C
  Wire.begin();

  mlx.begin();
}

void loop() {
  int buttonValue = digitalRead(buttonPin);
  // kiểm tra xem nút nhấn đã được nhấn trong vòng 500ms trước đó hay chưa
  if (buttonValue == LOW && millis() - lastButtonTime > 1500) {
    // nếu nút nhấn được nhấn, và đã đủ 500ms kể từ lần nhấn trước đó
    // thì lưu lại thời điểm nhấn nút nhấn để sử dụng cho lần nhấn tiếp theo
    lastButtonTime = millis();
    mode = mode + 1;
    if (mode > 3) {
      mode = 1;
    }
    Serial.print(mode);
    // thực hiện các thao tác khi nút nhấn được nhấn ở đây
  }
  // Check if object detected by YS-29 sensor
  // Display current time
  tmElements_t tm;
  if (RTC.read(tm)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Time: ");
    display.print(tm.Hour);
    display.print(":");
    display.print(tm.Minute);
    display.print(":");
    display.println(tm.Second);
    display.display();
    delay_millis(1000);
  }
  sensorValue = digitalRead(sensorPin);
  if (sensorValue == LOW) {
    float tempC = mlx.readObjectTempC();
    digitalWrite(buzzer, HIGH);
    delay_millis(250);
    digitalWrite(buzzer, LOW);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.print("Temp: ");
    display.print(tempC);
    display.println(" C");
    // Update OLED display
    display.display();
    delay_millis(1000);
  }
  if (mode == 1) {
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    getFingerprintEnroll(id);
  }
  if (mode == 2) {
    getFingerprintID(finger_id);
  }
  if (mode == 3) {
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    deleteFingerprint(id);
  }
}
