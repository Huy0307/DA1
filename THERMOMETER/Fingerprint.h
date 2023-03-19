#ifndef FINGERPRINT_H
#define FINGERPRINT_H
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
uint8_t readnumber();
uint8_t getFingerprintEnroll();
uint8_t getFingerprintID();
uint8_t deleteFingerprint();
void fingerprintSetup();
void fingerprintLoop();

#endif