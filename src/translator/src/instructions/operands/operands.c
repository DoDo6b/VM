#include "operands.h"


operand_t translateOperand (Buffer* bufR, size_t instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    operand_t operand = 0;

    if (!bufScanf (bufR, VALUEFORMAT, &operand))
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            instrc + 1,
            "syntax error",
            "no operand found"
        );
        exit (EXIT_FAILURE);
    }
    
    return operand;
}

#define CASE_REG(reg)  case reg ## _HASH: return reg;

opcode_t translateReg (Buffer* bufR, size_t instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    instruction_t reg = {0};

    if (bufScanf (bufR, REGISTERFORMAT, reg) == 0)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            instrc + 1,
            "syntax error",
            "no operand found"
        );
        exit (EXIT_FAILURE);
    }

    hash_t hash = djb2Hash (reg, sizeof (reg));
    switch (hash)
    {
        CASE_REG (AAX)
        CASE_REG (ACX)
        CASE_REG (ADX)
        CASE_REG (ABX)
        CASE_REG (ASP)
        CASE_REG (ABP)
        CASE_REG (ASI)
        CASE_REG (ADI)

        default:
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_srcerr
            (
                bufR->name,
                instrc + 1,
                "syntax error",
                "unknown register (hash: %lu)",
                hash
            );
            exit (EXIT_FAILURE);
    }
}

#undef CASE_REG