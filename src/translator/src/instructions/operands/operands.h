#ifndef OPERANDS_H
#define OPERANDS_H


#include "../../../../included/logger/logger.h"
#include "../../../../included/kassert/kassert.h"
#include "../../../../included/buffer/buffer.h"
#include "../../../../defines/arch.h"
#include "../../translatorcodes.h"


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


operand_t translateOperand (Buffer* bufR, size_t instrc);
opcode_t translateReg      (Buffer* bufR, size_t instrc);

#endif