#ifndef OPERANDS_H
#define OPERANDS_H


#include "../../../../included/logger/logger.h"
#include "../../../../included/kassert/kassert.h"
#include "../../../../included/buffer/buffer.h"
#include "../../../../defines/arch.h"
#include "../../translatorcodes.h"


enum RegisterHash
{
    RAX_HASH = 193504464,
    RCX_HASH = 193486017,
    RDX_HASH = 193486050,
    RBX_HASH = 193485984,
    RSP_HASH = 193486537,
    RBP_HASH = 193485976,
    RSI_HASH = 193486530,
    RDI_HASH = 193486035,
};


operand_t translateOperand (Buffer* bufR, size_t instrc);
opcode_t translateReg      (Buffer* bufR, size_t instrc);

#endif