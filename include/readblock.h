#include <Arduino.h>

#ifndef READBLOCK_H_
#define READBLOCK_H_

uint8_t readSectorBlock(uint8_t sector_num, uint8_t blocknum_in_sect,  uint8_t *data);

#else

extern uint8_t readSectorBlock(uint8_t sector_num, uint8_t blocknum_in_sect,  uint8_t *data);

#endif