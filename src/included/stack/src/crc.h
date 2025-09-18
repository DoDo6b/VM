#ifndef CRC_H
#define CRC_H


#include <stdint.h>
#include <stdio.h>
#include "../../kassert/kassert.h"


void crc32Init ();

uint32_t crc32Calculate (const uint8_t* src, size_t size);


#endif