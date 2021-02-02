#include <Arduino.h>

#ifndef _AUTHENTICATE_H_
#define _AUTHENTICATE_H_

uint8_t authenticate(byte sector, uint8_t keyNumber, uint8_t *keyData);

#else

extern uint8_t authenticate(byte sector, uint8_t keyNumber, uint8_t *keyData);

#endif