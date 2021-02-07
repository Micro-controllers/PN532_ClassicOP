# PN532_CLASSICOP

DO I NEED TO DESELECT A CARD AFTER ACCESSING IT!!!!!!!!!!!!

## Purpose

This sketch implements the basic operations for Mifare RFID cards thru PN532 nfc reader.

It replicates what was done with the RC522 in (CardReader)[https://github.com/Micro-controllers/CardReader]
and add convenient functions like:

* setValue: sets a block to an int16
* getValue: gets an in16 from a block
* increment: increments a  block by 1
* decrement: decrements a  block by 1
* read: reads a block
* write: writes a block

## Translating access bits conditions to trailer bytes

This python [tool](https://github.com/PyPajmd/mifare_access_conditions) helps translating access bits conditions to trailer bytes and vice versa.

## Mifare key is composed as follow:

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