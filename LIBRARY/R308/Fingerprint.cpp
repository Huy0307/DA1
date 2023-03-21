#include "fingerprint.h"
#include <Arduino.h>  //Search ở Tools --> Boards Manager --> Arduino AVR Boards
// const int buttonPin = 8;     // Pin của nút bấm
// int buttonState = HIGH;      // Trạng thái ban đầu của nút bấm (nút chưa được nhấn)
// int lastButtonState = HIGH;  // Trạng thái trước đó của nút bấm
// uint8_t id;
// uint8_t finger_id;
// int mode = 2;

SoftwareSerial mySerial(2, 3);  // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

/**
*@brief (Mô tả chức năng)
-	Hàm thiết lập hệ thống cho cảm biến (khai báo chân, …).
*@param (tham số truyền vào dạng tham chiếu)
*@retval (kết quả trả về)
*/
void fingerprintSetup(int &f) {
  // pinMode(buttonPin, INPUT_PULLUP);  // Cấu hình chân nút bấm là INPUT_PULLUP để tránh nhiễu
  finger.begin(57600);
  if (finger.verifyPassword()) {
    f = 1;
    Serial.println("Sensor ready");
  } else {
    Serial.println("Sensor not found");
    f = 0;
    while (1) {
      delay(1);
    }
  }
}
/**
*@brief (Mô tả chức năng)
*-	Hàm đọc giá trị ID được nhập vào.
*@param (tham số truyền vào dạng tham chiếu)
*-	uint8_t num: Giá trị được dùng để gán cho ký tự được nhập vào từ bàn phím
*@retval (kết quả trả về)
*-	Hàm trả về giá trị số nguyên, ký tự và số thực
*/
uint8_t readnumber() {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}
/**
*@brief (Mô tả chức năng)
-	Nhập dữ liệu vân tay vào bộ nhớ Flash.
*@param (tham số truyền vào dạng tham chiếu)
-	id: Giá trị id của vân tay
*@retval (kết quả trả về)
*/
uint8_t getFingerprintEnroll(uint8_t &id, int &success) {
  success = 0;
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        success = 1;
        break;
      case FINGERPRINT_NOFINGER:
        success = 2;
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        success = 0;
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        success = 0;
        break;
      default:
        Serial.println("Unknown error");
        success = 0;
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      success = 1;
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      success = 0;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      success = 0;
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      success = 0;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      success = 0;
      return p;
    default:
      Serial.println("Unknown error");
      success = 0;
      return p;
  }

  Serial.println("Remove finger");
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
        success = 1;
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        success = 2;
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        success = 0;
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        success = 0;
        break;
      default:
        Serial.println("Unknown error");
        success = 0;
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      success = 1;
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      success = 0;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      success = 0;
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      success = 0;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      success = 0;
      return p;
    default:
      Serial.println("Unknown error");
      success = 0;
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    success = 1;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    success = 0;
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    success = 0;
    return p;
  } else {
    Serial.println("Unknown error");
    success = 0;
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    success = 1;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    success = 0;
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    success = 0;
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    success = 0;
    return p;
  } else {
    Serial.println("Unknown error");
    success = 0;
    return p;
  }

  return true;
}
/**
*@brief (Mô tả chức năng)
-	Hàm quét vân tay và so sánh dữ liệu vân tay với vân tay được quét có khớp với nhau hay không.
*@param (tham số truyền vào dạng tham chiếu)
*@retval (kết quả trả về)
-	Hàm trả về giá trị số nguyên của ID nếu dấu vân tay trùng.
*/
uint8_t getFingerprintID(uint8_t &finger_id, int &detect) {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      detect = 1;
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      detect = 0;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      detect = 0;
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      detect = 0;
      return p;
    default:
      Serial.println("Unknown error");
      detect = 0;
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      detect = 1;
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      detect = 0;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      detect = 0;
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      detect = 0;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      detect = 0;
      return p;
    default:
      Serial.println("Unknown error");
      detect = 0;
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    detect = 1;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    detect = 0;
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    detect = 0;
    return p;
  } else {
    Serial.println("Unknown error");
    detect = 0;
    return p;
  }

  // found a match!
  finger_id = finger.fingerID;
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}
/**
*@brief (Mô tả chức năng)
-	Hàm xóa dấu vân tay.
*@param (tham số truyền vào dạng tham chiếu)
-	int id: Giá trị id vân tay cần xóa.
*@retval (kết quả trả về)
*/
uint8_t deleteFingerprint(uint8_t &id, int &a) {
  uint8_t p = -1;
  Serial.print("Waiting for valid finger to delete as #");
  Serial.println(id);
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
    a = 1;
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