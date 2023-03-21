#include "Fingerprint.h"
const int buttonPin = 8;     // Pin của nút bấm
int buttonState = HIGH;      // Trạng thái ban đầu của nút bấm (nút chưa được nhấn)
int lastButtonState = HIGH;  // Trạng thái trước đó của nút bấm
uint8_t id;
int success;
uint8_t finger_id;
int mode = 2;
int f;
int detect;
int a;
void setup() {
  Serial.begin(9600);
  fingerprintSetup(f);
  pinMode(buttonPin, INPUT_PULLUP);  // Cấu hình chân nút bấm là INPUT_PULLUP để tránh nhiễu
}
/**
*@brief (Mô tả chức năng)
-	Hàm thiết lập chế độ hoạt động của cảm biến.
*@param (tham số truyền vào dạng tham chiếu)
-	int mode: Chế độ hoạt động của cảm biến. 
*@retval (kết quả trả về)
*/
void loop() {
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
    getFingerprintEnroll(id,success);
    delay(2000);
  } else if (mode == 2) {
    getFingerprintID(finger_id,detect);
    delay(2000);
  } else if (mode == 3) {
    id = readnumber();
    if (id == 0) {
      return;
    }
    deleteFingerprint(id,a);
    delay(2000);
  }
  delay(50);
}
