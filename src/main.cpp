#include <Arduino.h>
#include <my_nfc.h>
#include <utils.h>
#include <serialprint.h>
#include <ops.h>

#define INVALID 0xFFFF
#define BLOCK_SIZE 16


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

uint8_t *read (byte sector, uint8_t block, uint8_t *data, uint8_t keyNumber, uint8_t *keyData) {

  if (authenticate(sector, keyNumber, keyData)) {
    if (readSectorBlock(sector, block, data)) {
      serialPrintf("\ndata in sector %d block %d - ", sector, block);
      serialPrintBuffer(data, BLOCK_SIZE);
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

void setValue(byte sector, uint8_t block, uint16_t value, uint8_t keyNumber, uint8_t *keyData) {
  byte data[16];
  memset(data, 0, sizeof(data));
  memcpy(data, &value, sizeof(value));
  write(sector, block, data, keyNumber, keyData);
}

uint16_t getValue(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData) {
  uint8_t data[16];
  uint16_t value = INVALID;
  read(sector, block, data, keyNumber, keyData);
  if (data != NULL) {
    memcpy(&value, data, sizeof(uint16_t));
  }
  return value;
}


uint16_t add(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData, uint16_t inc) {
  uint16_t value = getValue(sector, block, keyNumber, keyData);
  if (value != INVALID) {
    value += inc;
    setValue(sector, block, value, keyNumber, keyData);
    // double check
    uint16_t value_check = getValue(sector, block, keyNumber, keyData);
    return value_check == value? value: INVALID;
  }
  return value;
}

uint16_t increment(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData) {
  uint16_t inc = 1;
  return add(sector, block, keyNumber, keyData, inc);
}

uint16_t decrement(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData) {
  uint16_t inc = -1;
  return add(sector, block, keyNumber, keyData, inc);
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
  // memcpy(data, &counter, sizeof(counter));
  // write(sector, block, data, keyNumber, keyData);
  // read(sector, block, data, keyNumber, keyData);
  // uint16_t val = decrement(sector, block, keyNumber, keyData);
  uint16_t val = increment(sector, block, keyNumber, keyData);
  serialPrintf("counter = %d", val);
}