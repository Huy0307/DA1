#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#ifndef FINGERPRINT_H
#define FINGERPRINT_H
uint8_t getFingerprintEnroll(uint8_t &id, byte &s);
uint8_t getFingerprintID(uint8_t &finger_id);
uint8_t deleteFingerprint(uint8_t &id);
void fingerprintSetup();
#endif