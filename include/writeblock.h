#include <Arduino.h>

#ifndef WRITE_BLOCK_H_
#define WRITE_BLOCK_H_

uint8_t writeSectorBlock(uint8_t sector, uint8_t block_num, byte *value1Block);

#endif