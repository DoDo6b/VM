#ifndef TRANSLATOR_H
#define TRANSLATOR_H


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "../included/logger/logger.h"
#include "../included/kassert/kassert.h"
#include "../included/buffer/buffer.h"
#include "../defines/arch.h"
#include "../defines/errcodes.h"
#include "../vm/vm.h"


#define BUFFERSIZE BUFSIZ

enum InstructionHash
{
    PUSH_HASH = 6384411237,
    OUT_HASH  = 193465917,
    POP_HASH  = 193466804,
    HALT_HASH = 6384101742,

    ADD_HASH  = 193450094,
    SUB_HASH  = 193470255,
    MUL_HASH  = 193463731,
    DIV_HASH  = 193453544,
};


uint64_t translate (const char* input, const char* output);


#endif