#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <SoftwareSerial.h>
#include <SimpleTimer.h>
#include <ds1307.h>
// SimpleTimer timer;
#define laser 12
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
uint8_t id;
uint8_t finger_id;
byte buttonPin = 7;  // Sử dụng chân số 13 để kết nối với nút bấm
int mode = 2;
const int led_pin = 8;
const int sensor_pin = 5;  //sig_pin
const int buzzer_pin = 6;
const int en_pin = 4;
int authenticated = 0;

SoftwareSerial mySerial(2, 3);  // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

String readString() {
  while (true) {
    while (!Serial.available()) {
      // do nothing if no Serial data available
    }
    String input = Serial.readStringUntil('\n');
    input.trim();  // remove any leading/trailing whitespace
    if (input.length() > 0) {
      return input;
    }
  }
}
void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Đặt chân số 2 là INPUT_PULLUP để kết nối nút bấm
  Serial.begin(115200);
  ds1307setup();
  pinMode(en_pin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // Đặt chân số 2 là INPUT_PULLUP để kết nối nút bấm
  digitalWrite(en_pin, HIGH);        // Bật chế độ phát hiện mô-đun hồng ngoại
  pinMode(sensor_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);
  pinMode(laser, OUTPUT);
  digitalWrite(laser, LOW);
  mlx.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // timer.setInterval(1000L, readbutton);
  // timer.setInterval(15000L, CheckDelete);
  // timer.setInterval(10000L, CheckAdd);  //Set an internal timer every 10sec to check if there a new fingerprint in the website to add it.
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Sensor ready");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Sensor ready");
    display.display();
    delay_millis(2000);
  } else {
    Serial.println("Sensor not found");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Sensor not found");
    display.display();
    while (1) {
      delay_millis(1);
    }
  }
  display.clearDisplay();
}
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}
unsigned long lastButtonTime = millis();
void delay_millis(unsigned long ms) {
  unsigned long current_time = millis();  // Lấy thời điểm hiện tại
  while (millis() - current_time < ms) {  // Kiểm tra nếu thời gian chạy đã vượt quá khoảng thời gian cần delay
    // Chờ
  }
}
void loop() {
  // timer.run();
  int buttonValue = digitalRead(buttonPin);
  // kiểm tra xem nút nhấn đã được nhấn trong vòng 500ms trước đó hay chưa
  if (buttonValue == LOW && millis() - lastButtonTime > 1500) {
    // nếu nút nhấn được nhấn, và đã đủ 500ms kể từ lần nhấn trước đó
    // thì lưu lại thời điểm nhấn nút nhấn để sử dụng cho lần nhấn tiếp theo
    lastButtonTime = millis();
    mode = 1;
    // thực hiện các thao tác khi nút nhấn được nhấn ở đây
    CheckAdd();
  }
  checkTime();
  int sensor_state = digitalRead(sensor_pin);
  if (sensor_state == LOW) {
    digitalWrite(buzzer_pin, HIGH);
    delay_millis(250);
    digitalWrite(buzzer_pin, LOW);
    getTemp();
  }
  if (mode == 2) {
    getFingerprintID();
    delay_millis(2000);
  }
}
uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  display.clearDisplay();
  display.setCursor(15, 10);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Waiting for valid finger to enroll as #");
  display.println(id);
  delay_millis(1000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("Image taken");
        delay_millis(1000);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print(".");
        delay_millis(1000);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("Communication error");
        delay_millis(1000);
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("Imaging error");
        delay_millis(1000);
        break;
      default:
        Serial.println("Unknown error");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("Unknown error");
        delay_millis(1000);
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Image converted");
      delay_millis(1000);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Image too messy");
      delay_millis(1000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Communication error");
      delay_millis(1000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Could not find fingerprint features");
      delay_millis(1000);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Could not find fingerprint features");
      delay_millis(1000);
      return p;
    default:
      Serial.println("Unknown error");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Unknown error");
      delay_millis(1000);
      return p;
  }

  Serial.println("Remove finger");
  display.clearDisplay();
  display.setCursor(15, 10);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Remove finger");
  delay_millis(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  display.clearDisplay();
  display.setCursor(15, 10);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("ID: ");
  display.println(id);
  delay_millis(1000);
  p = -1;
  Serial.println("Place same finger again");
  display.clearDisplay();
  display.setCursor(15, 10);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Place same finger again");
  delay_millis(1000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("Image taken");
        delay_millis(1000);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print(".");
        delay_millis(1000);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("Communication error");
        delay_millis(1000);
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("Imaging error");
        delay_millis(1000);
        break;
      default:
        Serial.println("Unknown error");
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.print("Unknown error");
        delay_millis(1000);
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Image converted");
      delay_millis(1000);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Image too messy");
      delay_millis(1000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Communication error");
      delay_millis(1000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Could not find fingerprint features");
      delay_millis(1000);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Could not find fingerprint features");
      delay_millis(1000);
      return p;
    default:
      Serial.println("Unknown error");
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print("Unknown error");
      delay_millis(1000);
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");
  Serial.println(id);
  display.clearDisplay();
  display.setCursor(15, 10);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Creating model for #");
  display.print(id);
  delay_millis(1000);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Prints matched!");
    delay_millis(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Communication error");
    delay_millis(1000);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Fingerprints did not match");
    delay_millis(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Unknown error");
    delay_millis(1000);
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  display.clearDisplay();
  display.setCursor(15, 10);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("ID ");
  display.print(id);
  delay_millis(1000);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Stored!");
    delay_millis(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Communication error");
    delay_millis(1000);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Could not store in that location");
    delay_millis(1000);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Error writing to flash");
    delay_millis(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Unknown error");
    delay_millis(1000);
    return p;
  }

  return true;
}
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    authenticated = 1;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  finger_id = finger.fingerID;
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  // display.println("Image taken");
  // display.println("Image converted");
  display.println("Found a print match!");
  display.println("Match found ID ");
  display.print(finger_id);
  display.print(" with confidence of ");
  display.print(finger.confidence);
  display.display();
  delay_millis(2000);
  return finger.fingerID;
}
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}
void getTemp() {
  float temp_amb = mlx.readAmbientTempC();
  float temp_obj = mlx.readObjectTempC() + 1.65;

  digitalWrite(laser, HIGH);

  Serial.print("Room Temp = ");
  Serial.println(temp_amb);
  Serial.print("Object temp = ");
  Serial.println(temp_obj);
  display.clearDisplay();
  display.setCursor(25, 10);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print(temp_obj);
  display.println(" C");
  display.display();
  delay_millis(2000);
  if (temp_obj >= 37.5) {
    digitalWrite(buzzer_pin, HIGH);
    Serial.print("ALERT!!!!\n ");
    delay_millis(1500);
  }
  digitalWrite(led_pin, LOW);     //LED tắt
  digitalWrite(buzzer_pin, LOW);  //Buzzer tắt
  digitalWrite(laser, LOW);       //laser tắt
  display.clearDisplay();
  display.setCursor(25, 10);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(" Waiting for motion.....");
  display.display();
  delay_millis(2000);
  authenticated = 0;
}
void CheckAdd() {
  id = readnumber();
  if (id == 0) {  // ID #0 not allowed, try again!
    return;
  }
  getFingerprintEnroll();
  delay_millis(2000);
}
void CheckDelete() {
  if (mode == 3) {
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    deleteFingerprint();
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("DELETE SUCCESS!!");
    display.display();
    delay_millis(1000);
  }
}
uint8_t deleteFingerprint() {
  uint8_t p = -1;
  Serial.print("Waiting for valid finger to delete as #");
  Serial.println(id);
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x");
    Serial.println(p, HEX);
  }

  return p;
}