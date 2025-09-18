#ifndef TRANSLATOR_H
#define TRANSLATOR_H


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "../defines/settings.h"
#include "../included/logger/logger.h"
#include "../included/kassert/kassert.h"
#include "../included/buffer/buffer.h"
#include "../defines/arch.h"
#include "../defines/errcodes.h"
#include "../vm/vm.h"


#define BUFFERSIZE BUFSIZ

enum InstructionHash
{
    PUSH_HASH = 2089443941,
    OUT_HASH  = 193465917,
    POP_HASH  = 193466804,
    HALT_HASH = 2089134446,

    MOV_HASH  = 193463543,

    ADD_HASH  = 193450094,
    SUB_HASH  = 193470255,
    MUL_HASH  = 193463731,
    DIV_HASH  = 193453544,
};

enum RegisterHash
{
    AAX_HASH = 193485951,
    ACX_HASH = 193486017,
    ADX_HASH = 193486050,
    ABX_HASH = 193485984,
    ASP_HASH = 193486537,
    ABP_HASH = 193485976,
    ASI_HASH = 193486530,
    ADI_HASH = 193486035,
};


uint64_t translate (const char* input, const char* output);


#endif