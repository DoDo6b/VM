#ifndef BUFFER_H
#define BUFFER_H


#include <stdlib.h>
#include <stdio.h>

#include "../Macro.h"
#include "../logger/logger.h"
#include "../kassert/kassert.h"

#include "src/bufcodes.h"

#define SECURE

typedef enum
{
    NOTSETTED    = 0,
    BUFWRITE     = 1,
    BUFREAD      = 2,
}BufMode_t;

#ifndef _WIN32
inline long fileSize (FILE* handler)
{
    assertStrict (handler, "received NULL");

    long position = ftell (handler);
    fseek (handler, 0,        SEEK_END);
    long size     = ftell (handler);
    fseek (handler, position, SEEK_SET);

    return size;
}
#else
#include <io.h>

#define fileSize(handler)  filelength (fileno (handler))
#endif


typedef struct
{
    BufMode_t mode;
    size_t len;
    size_t size;
    char* buffer;
    char* bufpos;
    FILE* stream;
}Buffer;

Buffer* bufInit (size_t size);
void bufFree (Buffer* buf);

Erracc_t bufVerify (Buffer* buf, Erracc_t ignored);

int bufSetStream (Buffer* buf, FILE*      stream,  BufMode_t  mode);
FILE* bufFOpen   (Buffer* buf, const char* fname, const char* mode);

size_t bufRead  (Buffer* buf, size_t size);
size_t bufWrite (Buffer* buf, void* src, size_t size); 
size_t bufFlush (Buffer* buf);

int bufScanf    (Buffer* buf, const char* format, void* dst);

char bufGetc (Buffer* buf);
long long bufSeek (Buffer* buf, size_t offset, char base);
char* bufGetp (Buffer* buf);
void bufCpy (Buffer* buf, void* dst, size_t size);

void bufSpaces (Buffer* buf);


#endif