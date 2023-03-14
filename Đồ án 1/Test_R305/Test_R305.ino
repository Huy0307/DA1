#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint fingerprint = Adafruit_Fingerprint(&mySerial);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Fingerprint sensor starting up...");

  mySerial.begin(57600);
  if (fingerprint.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }
}

void loop() {
  getFingerprintID();
}

void getFingerprintID() {
  uint8_t id;
  uint8_t confidence;

  Serial.println("Waiting for valid finger...");
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Waiting for valid finger...");
  display.display();

  while (fingerprint.getImage() != FINGERPRINT_OK);

  if (fingerprint.image2Tz() != FINGERPRINT_OK) {
    Serial.println("Failed to convert image");
    return;
  }

  if (fingerprint.fingerFastSearch() != FINGERPRINT_OK) {
    Serial.println("Finger not found");
    return;
  }

  id = fingerprint.fingerID;
  confidence = fingerprint.confidence;

  Serial.print("Found ID #");
  Serial.print(id);
  Serial.print(" with confidence of ");
  Serial.println(confidence);
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("ID:");
  display.print(id);
  display.setCursor(0,10);
  display.print("Confidence:");
  display.print(confidence);
  display.display();
}
