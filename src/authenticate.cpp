#include <my_nfc.h>
#include <utils.h>
#include <serialprint.h>
#include <writeblock.h>
#include <constants.h>

/*
    sector 0..15
    keyNumber = MIFARE_CMD_AUTH_B or MIFARE_CMD_AUTH_A
    uint8_t keyData[6]
*/
uint8_t authenticate(byte sector, uint8_t keyNumber, uint8_t *keyData)
{

    uint32_t blockNumber = getTrailerBlock(sector);
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
    uint8_t uidLength;                     // Should be 4
    // wait 10 sec for a card to be presented
    uint8_t success = nfc_pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 10000);

    if (success)
    {
        uint8_t normalizedKey = (keyNumber == MIFARE_CMD_AUTH_B) ? 1 : 0;
        serialPrintf("Authenticating with key %s normalized key: %d block %d\n",
                     (keyNumber == MIFARE_CMD_AUTH_B ? "B" : "A"),
                     normalizedKey,
                     blockNumber);
        Serial.print("key: ");
        serialPrintBuffer(keyData, 6);
        Serial.print("uid: ");
        serialPrintBuffer(uid, uidLength);
        serialPrintf("uid sized: %d\n", uidLength);
        success = nfc_pn532.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, normalizedKey, keyData);
    }
    return success;
}

/*
    For now we are assuming that either key A or key B was left to its default value.
    In other words if we are setting key A we are assuming key B is 0xFFFFFFFFFFFF 
    and if we are setting key B we are assuming key A is 0xFFFFFFFFFFFF and

    parameters:
        uint8_t keyNumber, 
        uint8_t *keyData 6 byte array, 
        uint8_t *newKeyData 6 byte array, 
        uint8_t * access_bits 4 byte array
        uint8_t *block trailer block 16 byte array
*/
uint8_t buildKeyBlock(uint8_t keyNumber, uint8_t *keyData, uint8_t *newKeyData, uint8_t *access_bits, uint8_t *block, boolean obfuscateB=false)
{

    memset(block, 0xFF, BLOCK_SIZE);

    if (keyNumber == MIFARE_CMD_AUTH_A)
    {
        for (byte k = 0; k < 6; k++)
        {
            block[k] = newKeyData[k];
        }
        for (byte ab = 0; ab < 4; ab++)
        {
            uint8_t offset = 6;
            block[ab + offset] = access_bits[ab];
        }
        if (obfuscateB == true) {
            uint8_t offset = 10;
            for (byte k = 0; k < 6; k++) {
                block[k +offset] = newKeyData[k];
            }
        }
        return 1;
    }
    else
    {
        serialPrintf("Error: setting Key B not implemented!");
        return 0;
    }
    // else {
    //     uint8_t offset = 10;
    //     for (byte k = 0; k < 6; k++) {
    //         block[k +offset] = newKeyData[k];
    //     }
    // }
}

/*
    Mifare key is composed as follow:
    6 byte for key A
    4 byte for Access Bits
    6 byte for key B which is optional and can be set to 00 or any other value
    To change keys you have to authenticate the Sector Trailer and the write 
    the new keys + new access conditions if you want to change them too.
    Ex: New key A = 00 11 22 33 44 55 Access bits not overwritten Key B not used (so FF FF FF FF FF FF)
    Write to Sector Trailer 00 11 22 33 44 55 FF 0F 00 FF FF FF FF FF FF FF
    A default Access Bits could be FF 0F 00 that allow to write and read each block and to read and write key B
    but it prevents from overwriting the access conditions bits, unlike 0xFF, 0x07, 0x80 (the transport configuration) or
    7F 07 88 40 which could be more flexible.

    Access Bits C cond block
    FF Byte 6             ~C23 1 ~C22 1 ~C21 1 ~C20 1  ~C13 1 ~C12 1 ~C11 1 ~C10 1
    0F Byte 7              c13 0  C12 0  C11 0  C10 0  ~C33 1 ~C32 1 ~C31 1 ~C30 1
    00 Byte 8              C33 0  C32 0  C31 0  C30 0   C23 0  C22 0  C21 0  C20 0
    FF Byte 9 user data

    ~C23 1
    ~C22 1
    ~C21
    Ref: https://www.nxp.com/docs/en/data-sheet/MF1S50YYX_V1.pdf
    See: https://stackoverflow.com/questions/4842087/mifare-change-key-a-and-b
    parameters:
        sector 0..15
        keyNumber = MIFARE_CMD_AUTH_B or MIFARE_CMD_AUTH_A
        uint8_t keyData[6]
        uint8_t newKeyData[6]
        uint8_t access_bits[4]
*/
uint8_t setAuthKey(byte sector, uint8_t keyNumber, uint8_t *keyData, uint8_t *newKeyData, uint8_t *access_bits, boolean obfuscateB=false)
{
    uint8_t rc = 0;
    uint8_t key_block[BLOCK_SIZE];

    if (buildKeyBlock(keyNumber, keyData, newKeyData, access_bits, key_block, obfuscateB))
    {
        uint32_t blockNumber = getTrailerBlock(sector);

        // authenticate with the old key
        if (authenticate(sector, keyNumber, keyData))
        {
            // write the new key
            Serial.print("Writing trailer block - ");
            serialPrintBuffer(key_block, BLOCK_SIZE);
            if (writeSectorBlock(sector, RELATIVE_TRAILER_BLOCK_NUM, key_block))
            {
                serialPrintf("\nNew key written in sector %d block %d \n ", sector, blockNumber);
                rc = 1;
            }
            else
            {
                Serial.println("Couldn't write new key");
            }
        }
        else
        {
            serialPrintf("Could not authenticate with key %c sector %d block address %d key ",
                         keyNumber,
                         sector,
                         blockNumber);
            serialPrintBuffer(keyData, 6);
        }
    }
    return rc;
}
