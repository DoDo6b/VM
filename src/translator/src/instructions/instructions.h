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
    PUSH_HASH = 6384411237,
    OUT_HASH  = 193465917,
    POP_HASH  = 193466804,
    HALT_HASH = 6384101742,

    MOV_HASH  = 193463543,

    ADD_HASH  = 193450094,
    SUB_HASH  = 193470255,
    MUL_HASH  = 193463731,
    DIV_HASH  = 193453544,

    JMP_HASH  = 193460204,
};


Erracc_t writeOPcode (Buffer* bufW, opcode_t opcode);
Erracc_t writePush   (Buffer* bufW, Buffer* bufR, size_t instrc);
Erracc_t writeMov    (Buffer* bufW, Buffer* bufR, size_t instrc);


#endif