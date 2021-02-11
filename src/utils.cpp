#include <Arduino.h>

/*
    Returns the address of the trailer block based on the sector number it belongs to.
    The trailer block is the one the contains the auth key for the sector.
*/

uint32_t getTrailerBlock(byte sector) {
    // a classic card as 4 blocks per sectors
  const byte block_per_sect = 4;
  return ((sector + 1) * block_per_sect) - 1;
}

void serialPrintBuffer(byte *buffer, byte bufferSize) {
  String sbuf;
  for (int i = 0; i < bufferSize; i++) {
    sbuf.concat(String(buffer[i], HEX));
    sbuf.concat(" ");
  }
  Serial.println(sbuf.c_str());
}

uint8_t get_block_absolute_address(uint8_t sector, uint8_t block) {
    return sector * 4 + block;
}

uint8_t waitForUserInput(char *prompt) {
  int inByte = 0;
  Serial.println("");
  Serial.println(prompt);
  Serial.println("");
  
  // Wait for user input before proceeding
  while (Serial.available() < 1);
  while (Serial.available()) {
    inByte = Serial.read();
    Serial.println(inByte, DEC);
  }
  Serial.println("");
  return inByte;
}

uint8_t confirmEntry(char *prompt, const char *ok_choice="yY") {
  uint8_t action_selected = 0;
  Serial.print(prompt);
  while (Serial.available() < 1);
  while (Serial.available()) {
    action_selected = Serial.read();
  }
  return String(ok_choice).indexOf(String(char(action_selected))) < 0 ? 0 : 1;
}