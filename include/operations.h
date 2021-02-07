#include <Arduino.h>
#include <basic_ops.h>
#include <constants.h>

#ifndef OPERATIONS_H_
#define OPERATIONS_H_

uint8_t *read(byte sector, uint8_t block, uint8_t *data, uint8_t keyNumber, uint8_t *keyData);
void write(byte sector, uint8_t block, byte *data, uint8_t keyNumber, uint8_t *keyData);
void setValue(byte sector, uint8_t block, uint16_t value, uint8_t keyNumber, uint8_t *keyData);
uint16_t getValue(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData);
uint16_t add(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData, uint16_t inc);
uint16_t increment(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData);
uint16_t decrement(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData);

#endif