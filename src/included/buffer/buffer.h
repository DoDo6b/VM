#ifndef BUFFER_H
#define BUFFER_H


#include <stdlib.h>
#include <stdio.h>

#include "included/logger/logger.h"
#include "included/kassert/kassert.h"

#define SECURE

typedef struct
{
    char mode;
    size_t len;
    size_t size;
    char* buffer;
    FILE* stream;
}Buffer;

Buffer* bufInit (size_t size);
void bufFree (Buffer* buf);

int bufSetStream (Buffer* buf, FILE* stream, char mode);

size_t bufWrite (Buffer* buf, void* src, size_t size);
size_t bufFlush (Buffer* buf);


#endif