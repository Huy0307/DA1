#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 
#define laser 12

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

uint8_t id;
int mode = 2;

double temp_amb;
double temp_obj;

const int led_pin = 8;
const int sensor_pin = 5;  //sig_pin
const int buzzer_pin = 13;
const int en_pin = 4;

const long interval = 1000;
unsigned long current_time = millis();
unsigned long last_trigger = 0;
boolean timer_on = false;
int authenticated = 0;

void movement_detection() {
  Serial.println("Motion was detected");
  digitalWrite(led_pin, HIGH);
  digitalWrite(led_pin, LOW);
  digitalWrite(buzzer_pin, LOW);
  timer_on = true;
  last_trigger = millis();
}

void setup() {
  Serial.begin(115200);
  mlx.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  pinMode(en_pin, OUTPUT);
  digitalWrite(en_pin, HIGH);  // Bật chế độ phát hiện mô-đun hồng ngoại
  pinMode(sensor_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);
  pinMode(laser, OUTPUT);
  digitalWrite(laser, LOW);
  pinMode(2, INPUT_PULLUP);  // Khai báo chân số 2 là chân input với điện trở kéo lên
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor");
  // display.clearDisplay();
  // display.setCursor(25, 15);
  // display.setTextSize(1);
  // display.setTextColor(WHITE);
  // display.println(" Thermometer");
  // display.setCursor(25, 35);
  // display.setTextSize(1);
  // display.print("Initializing");
  // display.display();
  // delay(500);
  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    // display.clearDisplay();
    // display.setCursor(20, 30);
    // display.setTextSize(1);
    // display.setTextColor(WHITE);
    // display.println("No Fingerprint");
  } else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
  }
  delay(2000);
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

void loop() {
  if (digitalRead(2) == LOW) {     // Kiểm tra nút bấm được nhấn hay không
    delay(50);                     // Đợi một chút để đảm bảo nút bấm được nhấn đúng lần
    if (digitalRead(2) == LOW) {   // Kiểm tra lại xem nút bấm có được nhấn đúng lần không
      mode = (mode == 1) ? 2 : 1;  // Thay đổi giá trị của biến mode
    }
  }
  if (mode == 2) {
    getFingerprintID();
    if (authenticated == 1) {
      int sensor_state = digitalRead(sensor_pin);
      if (sensor_state == LOW) {
        movement_detection();
      }
      current_time = millis();
      if (timer_on && (current_time - last_trigger <= interval)) {
        temp_amb = mlx.readAmbientTempC();
        temp_obj = mlx.readObjectTempC() + 1.65;

        digitalWrite(laser, HIGH);

        Serial.print("Room Temp = ");
        Serial.println(temp_amb);
        Serial.print("Object temp = ");
        Serial.println(temp_obj);
        // display.clearDisplay();
        // display.setCursor(25, 10);
        // display.setTextSize(1);
        // display.setTextColor(WHITE);
        // display.println(" Temperature");
        // display.setCursor(25, 30);
        // display.setTextSize(2);
        // display.setTextColor(WHITE);
        // display.print(temp_obj);
        // display.println(" C");
        // display.display();
        // delay(250);
        if (temp_obj >= 37.5) {
          digitalWrite(buzzer_pin, HIGH);
          Serial.print("ALERT!!!!\n ");
        }
        
        timer_on = false;  //Tắt timer
        Serial.println("Motion has stopped");
        digitalWrite(led_pin, LOW);     //LED tắt
        digitalWrite(buzzer_pin, LOW);  //Buzzer tắt
        digitalWrite(laser, LOW);       //laser tắt
        // display.clearDisplay();
        // display.setCursor(25, 10);
        // display.setTextSize(1);
        // display.setTextColor(WHITE);
        // display.println(" Waiting for motion.....");
        // display.display();
        // delay(250);
        authenticated = 0;
      }
    }
    delay(2000);
  } else if (mode == 1) {
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    // display.clearDisplay();
    // display.setCursor(25, 10);
    // display.setTextSize(1);
    // display.setTextColor(WHITE);
    // display.println("Enroll Fingerprint");
    // display.display();
    // delay(250);
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);
    // display.clearDisplay();
    // display.setCursor(25, 20);
    // display.setTextSize(2);
    // display.setTextColor(WHITE);
    // display.println("Enrolling ID #");
    // display.print(id);
    // delay(250);
    while (!getFingerprintEnroll())
      ;
  }
}
uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
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

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
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
  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
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
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
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
