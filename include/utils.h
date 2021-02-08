#include <Arduino.h>

#ifndef UTILS_H_
#define UTILS_H_

uint32_t getTrailerBlock(byte sector);
void serialPrintBuffer(byte *buffer, byte bufferSize);
uint8_t get_block_absolute_address(uint8_t sector, uint8_t block);
uint8_t waitForUserInput(char *prompt);
uint8_t confirmEntry(char *prompt, char *ok_choice="yY");

#else

extern uint32_t getTrailerBlock(byte sector);
extern void serialPrintBuffer(byte *buffer, byte bufferSize);
extern uint8_t get_block_absolute_address(uint8_t sector, uint8_t block);
extern uint8_t waitForUserInput(char *prompt);
extern uint8_t confirmEntry(char *prompt, char *ok_choice="yY");
#endif