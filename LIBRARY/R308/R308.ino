#include "Fingerprint.h"
void setup() {
  Serial.begin(9600);
  fingerprintSetup();
}

void loop() {
  fingerprintLoop();
}
