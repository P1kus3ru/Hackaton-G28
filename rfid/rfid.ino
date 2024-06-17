#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

#define PN532_IRQ   (2)
#define PN532_RESET (3)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  delay(100);

  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);
  }
}

String getCardId() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  String id = "";

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (!success) return "";
  for (int i = 0; i < uidLength; i++) {
    id += uid[i];
  }

  return id;
}

void sendMessage(String value, int address = 100) {
  delay(1000);
  Serial.println("message|" + value);
  delay(1000);
}




void loop(void) {
  String id = getCardId();

  if (id.length()) {
    sendMessage(id);
  }
}

