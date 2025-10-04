#ifndef SEGMENTS_H
#define SEGMENTS_H


#include <stdlib.h>
#include <stdint.h>
#include "../../../included/logger/logger.h"
#include "../../../included/kassert/kassert.h"
#include "../../../included/Macro.h"
#include "../../../defines/arch.h"
#include "../vmcodes.h"

#ifndef NDEBUG
#include "../../../included/crc/crc.h"
#endif

typedef struct
{
        const char* code;
        size_t      size;
        const char* rip;
IF_DBG ( crc32_t    hash; )
}CodeSeg;

Erracc_t buildCodeseg (CodeSeg* dst, const char* bcname);
void     freeCodeseg  (CodeSeg* dst);

Erracc_t codesegVerify (const CodeSeg* seg);


typedef struct
{
    char* memory;
    size_t size;
}RAMseg;

Erracc_t buildRAMseg (RAMseg* dst, size_t size);
void     freeRAMseg  (RAMseg* dst);

Erracc_t RAMsegverify   (const RAMseg* seg);


#endif