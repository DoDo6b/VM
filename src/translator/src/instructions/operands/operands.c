#include "operands.h"


operand_t translateOperand (Buffer* bufR, size_t instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    operand_t operand = 0;

    if (bufScanf (bufR, VALUEFORMAT, &operand) == 0)
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

static opcode_t decomposeReg (const char* str, const char* bufName, size_t instrc)
{
    assertStrict (str, "received NULL");

    hash_t hash = djb2Hash (str, sizeof (instruction_t));
    switch (hash)
    {
        CASE_REG (RAX)
        CASE_REG (RCX)
        CASE_REG (RDX)
        CASE_REG (RBX)
        CASE_REG (RSP)
        CASE_REG (RBP)
        CASE_REG (RSI)
        CASE_REG (RDI)

        default:
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_srcerr
            (
                bufName,
                instrc + 1,
                "syntax error",
                "unknown register (reg: %s, hash: %lu)",
                str,
                hash
            );
            return UINT8_MAX;
    }
}

#undef CASE_REG

opcode_t translateReg (Buffer* bufR, size_t instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    instruction_t str = {0};

    if (bufScanf (bufR, REGISTERFORMAT, str) == 0)
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

    opcode_t reg = decomposeReg (str, bufR->name, instrc);

    if (reg == UINT8_MAX)
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

    return reg;
}


Erracc_t decomposeMemcall (Buffer* bufR, opcode_t* reg, offset_t* offset, size_t instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (reg,    "received NULL");
    assertStrict (offset, "received NULL");

    char str[32] = {0};
    size_t size = bufScanf (bufR, "[%31[^]]", str);
    if (size == 0 || bufGetc (bufR) != ']')
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            instrc + 1,
            "syntax error",
            "no operand found: %s",
            str
        );
        exit (EXIT_FAILURE);
    }

    char          sign   =  0;
    instruction_t regstr = {0};

    for (size_t i = 0; i < size; i++)
    {
        if (isspace (str[i])) continue;

        if (isdigit (str[i]))
        {
            char* nexttoken = NULL;
            *offset = strtoll (&str[i], &nexttoken, 16);
            i = (size_t)(nexttoken - str - 1);
        }

        if (str[i] == '+' || str[i] == '-') sign = str[i];

        if (isalpha (str[i]))
        {
            int nexttokenoff = 0;
            sscanf (&str[i], "%s%n", regstr, &nexttokenoff);
            i += (size_t)nexttokenoff - 1;
        }
    }

    *offset *= sign == '-' ? -1 : 1;
    if (strlen (regstr))
    {
        *reg = decomposeReg (regstr, bufR->name, instrc);
        if (*reg == DISP64)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_err ("syntax error", "you cant use RBP here");
            *reg = UINT8_MAX;
            return ErrAcc;
        }
    }
    else *reg = DISP64;

    return ErrAcc;
}