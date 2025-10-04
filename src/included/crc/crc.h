#ifndef CRC_H
#define CRC_H


#include <stdint.h>
#include <stdio.h>
#include "../kassert/kassert.h"


typedef uint32_t crc32_t;

void crc32Init ();

crc32_t crc32Calculate (const uint8_t* src, size_t size);


#endif