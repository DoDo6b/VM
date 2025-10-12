#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H


#include <inttypes.h>
#include <ctype.h>

#include "../../../defines/settings.h"
#include "../../../included/logger/logger.h"
#include "../../../included/kassert/kassert.h"
#include "../../../included/buffer/buffer.h"
#include "../../../defines/arch.h"
#include "../translatorcodes.h"
#include "operands/operands.h"
#include "jmp.h"


enum InstructionHash
{
    HALT_HASH = 2090320366,

    IN_HASH    = 5863484,

    OUT_HASH   = 193501853,
    POP_HASH   = 193502740,
    PUSH_HASH  = 2090629861,

    MOV_HASH   = 193499479,

    JMP_HASH   = 193496140,
    JNZ_HASH   = 193496183,
    JZ_HASH    = 5863529,
    JL_HASH    = 5863515,
    JLE_HASH   = 193496096,
    JGE_HASH   = 193495931,
    JG_HASH    = 5863510,

    CALL_HASH  = 2090140673,
    RET_HASH   = 193504592,

    CMP_HASH  = 193488517,

    ADD_HASH   = 193486030,
    SUB_HASH   = 193506191,
    MUL_HASH   = 193499667,
    DIV_HASH   = 193489480,

    DMP_HASH   = 193489606,
    DRAW_HASH  = 2090194771,
};


Erracc_t writeOPcode (Buffer* bufW, opcode_t opcode);
Erracc_t writePush   (Buffer* bufW, Buffer* bufR, size_t instrc);
Erracc_t writeMov    (Buffer* bufW, Buffer* bufR, size_t instrc);


#endif