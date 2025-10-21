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


typedef struct
{
    BufMode_t   mode;
    size_t      len;
    size_t      size;
    char*       buffer;
    char*       bufpos;
    FILE*       stream;
    char* name;   // optional, non NULL isnt garanted
}Buffer;

Buffer* bufInit (size_t size);
void bufFree (Buffer* buf);

Erracc_t bufVerify (Buffer* buf, Erracc_t ignored);
Erracc_t bufDump   (Buffer* buf);

int bufSetStream (Buffer* buf, const char* name, FILE* stream,  BufMode_t  mode);
FILE* bufFOpen   (Buffer* buf, const char* fname,              const char* mode);

size_t bufRead  (Buffer* buf, size_t size);
size_t bufLSplit(Buffer* buf);
size_t bufWrite (Buffer* buf, const void* src, size_t size); 
size_t bufFlush (Buffer* buf);

size_t bufScanf    (Buffer* buf, const char* format, void* dst);
long   bufNLine    (Buffer* buf);
char   bufGetc     (Buffer* buf);
char   bufpeekc    (Buffer* buf);

long long bufSeek (Buffer* buf, long offset, char base);
long      bufTell  (const Buffer* buf);
long      bufTellL (const Buffer* buf);
void bufCpy (Buffer* buf, void* dst, size_t size);

void bufSSpaces (Buffer* buf);


#endif