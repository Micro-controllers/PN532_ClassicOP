#include <Arduino.h>
#include <my_nfc.h>
#include <utils.h>
#include <serialprint.h>

/*
    Write a block
    parameters:
        sector 0..15
        block_num 0..4
        byte value1Block[16]
    return 0 if failed
*/
uint8_t writeSectorBlock(uint8_t sector, uint8_t block_num, byte *value1Block) {
  Serial.println("Writing block ...");
  byte valueBlockAddr = get_block_absolute_address(sector, block_num); 
  serialPrintf("Writing block %d ...", valueBlockAddr);
   
  return nfc_pn532.mifareclassic_WriteDataBlock(valueBlockAddr, value1Block); 
}