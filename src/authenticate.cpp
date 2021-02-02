#include <my_nfc.h>
#include <utils.h>
#include <serialprint.h>

/*
    sector 0..15
    keyNumber = MIFARE_CMD_AUTH_B or MIFARE_CMD_AUTH_A
    uint8_t keyData[6]
*/
uint8_t authenticate(byte sector, uint8_t keyNumber, uint8_t *keyData) {

    uint32_t blockNumber   = getTrailerBlock(sector);
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;  // Should be 4
    // wait 10 sec for a card to be presented
    uint8_t success = nfc_pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 10000);
    
    if (success) {
        uint8_t normalizedKey = (keyNumber == MIFARE_CMD_AUTH_B) ? 1 : 0;
        serialPrintf("Authenticating with key %s normalized key: %d block %d\n", 
            (keyNumber == MIFARE_CMD_AUTH_B? "B" : "A"), 
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