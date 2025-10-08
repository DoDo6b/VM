#ifndef OPERANDS_H
#define OPERANDS_H


#include <ctype.h>

#include "../../../../included/logger/logger.h"
#include "../../../../included/kassert/kassert.h"
#include "../../../../included/buffer/buffer.h"
#include "../../../../defines/arch.h"
#include "../../translatorcodes.h"


enum RegisterHash
{
    RAX_HASH = 193504464,
    RCX_HASH = 193504530,
    RDX_HASH = 193504563,
    RBX_HASH = 193504497,
    RSP_HASH = 193505050,
    RBP_HASH = 193504489,
    RSI_HASH = 193505043,
    RDI_HASH = 193504548,
};


operand_t translateOperand (Buffer* bufR,                                  size_t instrc);
opcode_t translateReg      (Buffer* bufR,                                  size_t instrc);
Erracc_t decomposeMemcall  (Buffer* bufR, opcode_t* reg, offset_t* offset, size_t instrc);

#endif