#ifndef TRANSLATOR_H
#define TRANSLATOR_H


#include <stdio.h>
#include <stdint.h>

#include "../logger/logger.h"
#include "../kassert/kassert.h"
#include "errcodes.h"


#define RTASM_VER "A250910"

enum OP_codes
{
    PUSH = 0XA0,
    OUT  = 0XA1,
    POP  = 0XA2,
    HALT = 0XAF,

    ADD  = 0XB0,
    SUB  = 0XB1,
    MUL  = 0XB2,
    DIV  = 0XB3,
};

enum InstructionHash
{
    PUSH_HASH = 2089443941,
    OUT_HASH  = 193465917,
    POP_HASH  = 193466804,
    HALT_HASH = 2089134446,

    ADD_HASH  = 193450094,
    SUB_HASH  = 193470255,
    MUL_HASH  = 193463731,
    DIV_HASH  = 193453544,
};

#define VALUEPREFIX '#'


uint64_t translate (const char* input, const char* output);


#endif