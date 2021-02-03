#include <Arduino.h>

#ifndef _AUTHENTICATE_H_
#define _AUTHENTICATE_H_

uint8_t authenticate(byte sector, uint8_t keyNumber, uint8_t *keyData);
uint8_t setAuthKey(byte sector, uint8_t keyNumber, uint8_t *keyData, uint8_t *newKeyData, uint8_t * access_bits);

#else

extern uint8_t authenticate(byte sector, uint8_t keyNumber, uint8_t *keyData);
extern uint8_t setAuthKey(byte sector, uint8_t keyNumber, uint8_t *keyData, uint8_t *newKeyData, uint8_t * access_bits);

#endif