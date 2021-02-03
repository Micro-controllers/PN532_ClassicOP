#include <operations.h>
#include <serialprint.h>
#include <utils.h>

uint8_t *read(byte sector, uint8_t block, uint8_t *data, uint8_t keyNumber, uint8_t *keyData)
{

    if (authenticate(sector, keyNumber, keyData))
    {
        if (readSectorBlock(sector, block, data))
        {
            serialPrintf("\ndata in sector %d block %d - ", sector, block);
            serialPrintBuffer(data, BLOCK_SIZE);
            return data;
        }
        else
        {
            Serial.println("Couldn't read any data");
        }
    }
    else
    {
        serialPrintf("Couldn't authenticate for reading sector %d", sector);
    }
    return NULL;
}

void write(byte sector, uint8_t block, byte *data, uint8_t keyNumber, uint8_t *keyData)
{

    if (authenticate(sector, keyNumber, keyData))
    {
        if (writeSectorBlock(sector, block, data))
        {
            serialPrintf("\ndata written in sector %d block %d \n ", sector, block);
        }
        else
        {
            Serial.println("Couldn't write any data");
        }
    }
    else
    {
        serialPrintf("Couldn't authenticate for writing sector %d", sector);
    }
}

void setValue(byte sector, uint8_t block, uint16_t value, uint8_t keyNumber, uint8_t *keyData)
{
    byte data[16];
    memset(data, 0, sizeof(data));
    memcpy(data, &value, sizeof(value));
    write(sector, block, data, keyNumber, keyData);
}

uint16_t getValue(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData)
{
    uint8_t data[16];
    uint16_t value = INVALID;
    read(sector, block, data, keyNumber, keyData);
    if (data != NULL)
    {
        memcpy(&value, data, sizeof(uint16_t));
    }
    return value;
}

uint16_t add(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData, uint16_t inc)
{
    uint16_t value = getValue(sector, block, keyNumber, keyData);
    if (value != INVALID)
    {
        value += inc;
        setValue(sector, block, value, keyNumber, keyData);
        // double check
        uint16_t value_check = getValue(sector, block, keyNumber, keyData);
        return value_check == value ? value : INVALID;
    }
    return value;
}

uint16_t increment(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData)
{
    uint16_t inc = 1;
    return add(sector, block, keyNumber, keyData, inc);
}

uint16_t decrement(byte sector, uint8_t block, uint8_t keyNumber, uint8_t *keyData)
{
    uint16_t inc = -1;
    return add(sector, block, keyNumber, keyData, inc);
}
