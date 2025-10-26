#ifndef DESCRIPTIONS_H
#define DESCRIPTIONS_H


#include <stdlib.h>

#include "../included/logger/logger.h"

#include "../defines/arch.h"
#ifdef TARGET_TRNSLT
#include "../translator/src/instructions/instructions.h"
#else
#include "../vm/src/operations/operations.h"
#endif

typedef struct
{
    opcode_t opcode;
#ifdef TARGET_TRNSLT
    hash_t hash;
    const char* str;
    void (*encode)(Buffer*, Buffer*);
#else
    void (*exec)(VM*);
#endif
}iDescription_s;

extern iDescription_s Descriptions[];

void descriptionsInit ();

#ifdef TARGET_TRNSLT
const iDescription_s* descriptionSearch (hash_t hash);
#endif

#endif