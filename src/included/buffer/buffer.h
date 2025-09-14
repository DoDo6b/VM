#ifndef BUFFER_H
#define BUFFER_H


#include <stdlib.h>
#include <stdio.h>

#include "included/logger/logger.h"

//#define SECURE

typedef struct
{
    size_t len;
    size_t size;
    char* buffer;
    FILE* stream;
}Buffer;

void bufInit (Buffer* buf, size_t size);
void bufSetStream (Buffer* buf, FILE* stream);
void bufFree (Buffer* buf);

size_t bufWrite (Buffer* buf, void* src, size_t size);
size_t bufFlush (Buffer* buf);


#endif