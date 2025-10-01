#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H


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
    HALT_HASH = 6384101742,

    OUT_HASH   = 193465917,
    POP_HASH   = 193466804,
    PUSH_HASH  = 6384411237,

    MOV_HASH   = 193463543,

    JMP_HASH   = 193460204,
    JNZ_HASH   = 193460247,
    JZ_HASH    = 5862441,
    JL_HASH    = 5862427,
    JLE_HASH   = 193460160,
    JGE_HASH   = 193459995,
    JG_HASH    = 5862422,

    CMP_HASH  = 193452581,

    ADD_HASH   = 193450094,
    SUB_HASH   = 193470255,
    MUL_HASH   = 193463731,
    DIV_HASH   = 193453544,
};


Erracc_t writeOPcode (Buffer* bufW, opcode_t opcode);
Erracc_t writePush   (Buffer* bufW, Buffer* bufR, size_t instrc);
Erracc_t writeMov    (Buffer* bufW, Buffer* bufR, size_t instrc);


#endif