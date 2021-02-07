#include <Arduino.h>
#include <my_nfc.h>
#include <utils.h>
#include <serialprint.h>
#include <operations.h>
#include <authenticate.h>

PN532_SPI pn532_spi(SPI, 10); // Initialize the NP532 with SPI
// NfcAdapter nfc = NfcAdapter(pn532_spi);
PN532 nfc_pn532 = PN532(pn532_spi);

void setup()
{
    Serial.begin(115200);
    Serial.println("Looking for PN532...");

    nfc_pn532.begin();

    uint32_t versiondata = nfc_pn532.getFirmwareVersion();
    if (!versiondata)
    {
        Serial.print("Didn't find PN53x board");
        while (1)
            ; // halt
    }

    // Got ok data, print it out!
    Serial.print("Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware ver. ");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.');
    Serial.println((versiondata >> 8) & 0xFF, DEC);

    // configure board to read RFID tags
    nfc_pn532.SAMConfig();
}

void sector15()
{
    // read sect 15 authenticated data
    byte sector = 15;
    uint8_t block = 1;
    uint8_t keyNumber = MIFARE_CMD_AUTH_A;
    uint8_t keyData[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

    // byte data[16];
    // uint16_t counter = 300;
    // memset(data, 0, sizeof(data));
    // memcpy(data, &counter, sizeof(counter));
    // write(sector, block, data, keyNumber, keyData);
    // read(sector, block, data, keyNumber, keyData);

    uint16_t val = decrement(sector, block, keyNumber, keyData);
    // uint16_t val = increment(sector, block, keyNumber, keyData);
    serialPrintf("counter = %d", val);
}

void sector10()
{
    uint8_t sector = 10;
    uint8_t keyNumber = MIFARE_CMD_AUTH_A;
    uint8_t oldKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t newKey[] = {'F', 'M', '2', '0', '2', '1'};
    uint8_t accessBits[4] = {0x7F, 0x07, 0x88, 0x40};
    if (setAuthKey(sector, MIFARE_CMD_AUTH_A, oldKey, newKey, accessBits))
    {
        serialPrintf("New key set");
    }
}

void readSector1Block1()
{
    byte sector = 1;
    uint8_t block = 1;
    uint8_t keyNumber = MIFARE_CMD_AUTH_A;
    uint8_t keyData[] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7};

    byte data[16];
    read(sector, block, data, keyNumber, keyData);
    for (int i = 0; i < 15; i++)
        data[i] = 65 + i;
    write(sector, block, data, keyNumber, keyData);
}

void setAllKeys(uint8_t keyNumber, uint8_t *oldKey, uint8_t *newKey, uint8_t *accessBits)
{
    for (int sector = 0; sector < 16; sector++)
    {
        if (setAuthKey(sector, MIFARE_CMD_AUTH_A, oldKey, newKey, accessBits))
        {
            serialPrintf("Key reset for sector %d", sector);
        }
        else
        {
            serialPrintf("Failed to reset key for sector %d", sector);
        }
    }
}

void resetToFactoryAllKeys()
{
    uint8_t keyNumber = MIFARE_CMD_AUTH_A;
    uint8_t oldKey[] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7};
    uint8_t newKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t accessBits[4] = {0xff, 0x07, 0x80, 0x69};
    setAllKeys(keyNumber, oldKey, newKey, accessBits);
}

void setKeySector1()
{
    uint8_t sector = 1;
    uint8_t keyNumber = MIFARE_CMD_AUTH_A;
    uint8_t oldKey[] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7};
    uint8_t newKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t accessBits[4] = {0xff, 0x07, 0x80, 0x69};
    if (setAuthKey(sector, MIFARE_CMD_AUTH_A, oldKey, newKey, accessBits))
    {
        serialPrintf("Key reset for sector %d", sector);
    }
    else
    {
        serialPrintf("Failed to reset key for sector %d", sector);
    }
}


void setKeySector0()  // actually reset all sectors
{
    // uint8_t sector = 0;
    // uint8_t keyNumber = MIFARE_CMD_AUTH_A;
    // uint8_t oldKey[] = {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5};
    // uint8_t newKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    // uint8_t accessBits[4] = {0xff, 0x07, 0x80, 0x69};
    // if (setAuthKey(sector, MIFARE_CMD_AUTH_A, oldKey, newKey, accessBits))
    // {
    //     serialPrintf("Key reset for sector %d", sector);
    // }
    // else
    // {
    //     serialPrintf("Failed to reset key for sector %d", sector);
    // }
    /*
        Brute for reset of all sector key by using key B 0xff, 0xff, 0xff, 0xff, 0xff, knowing
        the access conditions bits were set to 7F 07 88 (not sure how it was set this way, I can't
        remember doing it!).
        For some reason it worked even for sector 15 where conditions bits are set to FF 0f FF
    */
    for (uint8_t sector = 0; sector < 16; sector++) {
        uint8_t data[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x80, 0x69, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        uint8_t newKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        write(sector, 3, data, MIFARE_CMD_AUTH_B, newKey);
    }
}

void setAccessConditionsSect14KeyAAlreadySetFFFFFF() {
    // Original access bits FF 0F 00 = 0 [0, 0, 0], 1 [0, 0, 0], 2 [0, 0, 0], 3 [0, 0, 0]
    // It should not be possible! Confirming - Not possible
    uint8_t sector = 14;
    uint8_t oldKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t newKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t accessBits[4] = {0xff, 0x07, 0x80, 0x69};
    if (setAuthKey(sector, MIFARE_CMD_AUTH_A, oldKey, newKey, accessBits))
    {
        serialPrintf("Key reset for sector %d", sector);
    }
    else
    {
        serialPrintf("Failed to reset key for sector %d", sector);
    }
}

void setSect14KeyA() {
    // Original access bits FF 0F 00 = 0 [0, 0, 0], 1 [0, 0, 0], 2 [0, 0, 0], 3 [0, 0, 0]
    uint8_t sector = 14;
    uint8_t oldKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t newKey[] = {0x46, 0x4D, 0x32, 0x30, 0x32, 0x31};  // FM2021
    uint8_t accessBits[4] = {0xff, 0x07, 0x80, 0x69};
    if (setAuthKey(sector, MIFARE_CMD_AUTH_A, oldKey, newKey, accessBits))
    {
        serialPrintf("Key reset for sector %d", sector);
    }
    else
    {
        serialPrintf("Failed to reset key for sector %d", sector);
    }
}

/*
    Original config:
        Key A is {0x46, 0x4D, 0x32, 0x30, 0x32, 0x31} FM2021
        Access conditions FF 0F 00
        key B FF FF FF FF FF FF
    Trying to reset Key A with key B FF FF FF FF FF FF
    it should not be possible! But it is!!!

    Conclusion: both keys should be set, to prevent key B from doing anything
*/
void resetSect14KeyAWithKeyB_FFFFFF() {
    uint8_t sector = 14;
    uint8_t data[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x80, 0x69, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t newKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    write(sector, 3, data, MIFARE_CMD_AUTH_B, newKey);
}

/*
    This setting is ideal to hide all data on a card. The only thing I might change is 
    let KeyA to write access conditions.
*/
void setSect14KeyAObfuscateKeyB() {
    // Original access bits FF 0F 00 = 0 [0, 0, 0], 1 [0, 0, 0], 2 [0, 0, 0], 3 [0, 0, 0]
    uint8_t sector = 14;
    uint8_t oldKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t newKey[] = {0x46, 0x4D, 0x32, 0x30, 0x32, 0x31};  // FM2021
    uint8_t accessBits[4] = {0xff, 0x07, 0x80, 0x69}; // already FF 0F 00 and can't be overwritten
    boolean obfuscateB = true;
    if (setAuthKey(sector, MIFARE_CMD_AUTH_A, oldKey, newKey, accessBits, obfuscateB))
    {
        serialPrintf("Key reset for sector %d", sector);
    }
    else
    {
        serialPrintf("Failed to reset key for sector %d", sector);
    }
}

void loop()
{
    waitForUserInput("Place a card on the card reader and press any key to continue ... ");

    // sector15();
    // sector10();
    // readSector1Block1();
    // setKeySector0();  // brute force reset
    // setAccessConditionsSect14KeyAAlreadySetFFFFFF();  // Failed as anticipated
    // setSect14KeyA();  // worked even though the new access condtions can't be changed
    // resetSect14KeyAWithKeyB_FFFFFF();
    setSect14KeyAObfuscateKeyB();
}