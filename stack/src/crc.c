#include "crc.h"


static uint32_t byteTable[256] = {0};

void crc32Init ()
{
    const uint32_t polynom = 0xEDB88320;
    
    for (uint16_t i = 0; i < 256; i++)
    {
        uint32_t ch = i;
        for (uint8_t b = 0; b < 8; b++)
        {
            if (ch & 1) ch = polynom ^ (ch >> 1);
            else        ch >>= 1;
        }
        byteTable[i] = ch;
    }
}

uint32_t crc32Calculate (const uint8_t* src, size_t size)
{
    assertStrict (src, "received a NULL");
    assertStrict (size > 0, "received a size equal 0");

    if (byteTable[0] == 0) crc32Init ();

    uint32_t hash = 0xFFFFFFFF;

    for (size_t i = 0; i < size; i++)
    {
        uint8_t ch = src[i];
        hash = byteTable[(hash ^ ch) & 0xFF] ^ (hash >> 8);
    }

    return hash ^ 0xFFFFFFFF;
}