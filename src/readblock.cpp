#include <Arduino.h>
#include <my_nfc.h>
#include <utils.h>

/*
    Reads data from a sector block
        sector_num 0 to 14
        blocknum_in_sect 0 to 3
        data is at least 18 bytes: uint8_t data[18];
    Return 1 if success else 0
*/

uint8_t readSectorBlock(uint8_t sector_num, uint8_t blocknum_in_sect,  uint8_t data[18]) {
    uint8_t blockNumber = get_block_absolute_address(sector_num, blocknum_in_sect);
    
    return nfc_pn532.mifareclassic_ReadDataBlock (blockNumber, data);
}