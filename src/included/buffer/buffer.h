#ifndef BUFFER_H
#define BUFFER_H


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "../logger/logger.h"
#include "../kassert/kassert.h"

#define SECURE


#ifndef _WIN32
long fileSize (FILE* handler)
{
    assertStrict (file, "received NULL");

    long position = ftell (file);
    fseek (file, 0,        SEEK_END);
    long size     = ftell (file);
    fseek (file, position, SEEK_SET);

    return size;
}
#else
#include <io.h>
#define fileSize(handler)  filelength (fileno (handler))
#endif


typedef struct
{
    char mode;
    size_t len;
    size_t size;
    char* buffer;
    char* bufpos;
    FILE* stream;
}Buffer;

Buffer* bufInit (size_t size);
void bufFree (Buffer* buf);

int bufSetStream (Buffer* buf, FILE* stream, char mode);

size_t bufRead  (Buffer* buf, size_t size);
size_t bufWrite (Buffer* buf, void* src, size_t size); 
size_t bufFlush (Buffer* buf);


char* bufSkpCh     (Buffer* buf, char ch);
char* bufSkpSpaces (Buffer* buf);
char* bufSkpL      (Buffer* buf);


#endif