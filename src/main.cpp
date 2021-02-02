#include <Arduino.h>
#include <my_nfc.h>
#include <utils.h>
#include <serialprint.h>
#include <ops.h>

#define INVALID 0xFFFF

PN532_SPI pn532_spi(SPI, 10); // Initialize the NP532 with SPI
// NfcAdapter nfc = NfcAdapter(pn532_spi);
PN532 nfc_pn532 = PN532(pn532_spi);

void setup() {
  Serial.begin(115200);
  Serial.println("Looking for PN532...");

  nfc_pn532.begin();

  uint32_t versiondata = nfc_pn532.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc_pn532.SAMConfig();
}

uint8_t *read (byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData) {

  if (authenticate(sector, keyNumber, keyData)) {
    uint8_t data[18];
    if (readSectorBlock(sector, block, data)) {
      serialPrintf("\ndata in sector %d block %d - ", sector, block);
      serialPrintBuffer(data, sizeof(data) - 2);
      return data;
    }
    else {
      Serial.println("Couldn't read any data");
    }
  }
  else {
    serialPrintf("Couldn't authenticate for reading sector %d", sector);
  }
  return NULL;
}

void write(byte sector, uint8_t block, byte *data, uint8_t keyNumber, uint8_t *keyData) {

  if (authenticate(sector, keyNumber, keyData)) {
    if (writeSectorBlock(sector, block, data)) {
      serialPrintf("\ndata written in sector %d block %d \n ", sector, block);
    }
    else {
      Serial.println("Couldn't write any data");
    }
  }
  else {
    serialPrintf("Couldn't authenticate for writing sector %d", sector);
  }
}

void increment() {
  byte bytes[16];
  int value;
  memcpy(&value, bytes, sizeof(int));
}

uint16_t decrement(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData) {
  uint8_t *data = read(sector, block, keyNumber, keyData);
  uint16_t value;

  if (data != NULL) {
    memcpy(&value, data, sizeof(uint16_t));
    value = value -= 1;
    write(sector, block, data, keyNumber, keyData);

    // check
    data = read(sector, block, keyNumber, keyData);
    if (data != NULL) {
      uint16_t value_check;
      memcpy(&value_check, data, sizeof(uint16_t));
      return value_check == value? value: INVALID;
    }
  }
  return INVALID; // Failed
}

void setAuthKey() {

}

void loop() {
  // read sect 15 authenticated data
  byte sector = 15;
  uint8_t block = 1;
  uint8_t keyNumber = MIFARE_CMD_AUTH_A;
  uint8_t keyData[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

  waitForUserInput("Place a card on the card reader and press any key to continue ... ");

  byte data[16];
  uint16_t counter = 300;
  memset(data, 0, sizeof(data));
  memcpy(data, &counter, sizeof(counter));
  // write(sector, block, data, keyNumber, keyData);
  // read(sector, block, keyNumber, keyData);
  if (decrement(sector, block, keyNumber, keyData) == INVALID) {
    Serial.println("Failed to decrement");
  }
}