#include "operands.h"


typedef struct
{
    hash_t hash;
    const char* str;
    opcode_t opcode;
}RegDescr_s;

static RegDescr_s Regs[NUM_REGS] = {};

#define REG_DESCR(reg, i) \
    Regs[i].str = #reg;\
    Regs[i].hash = djb2Hash (#reg, sizeof (#reg));\
    Regs[i].opcode = OPC_ ## reg;

void reginit ()
{
    REG_DESCR (RAX, 0)
    REG_DESCR (RCX, 1)
    REG_DESCR (RDX, 2)
    REG_DESCR (RBX, 3)
    REG_DESCR (RSP, 4)
    REG_DESCR (RBP, 5)
    REG_DESCR (RSI, 6)
    REG_DESCR (RDI, 7)
}


operand_t getImm (Buffer* bufR)
{
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    operand_t operand = 0;

    if (bufScanf (bufR, VALUEFORMAT, &operand) == 0)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            bufTellL (bufR),
            "syntax error",
            "no operand found"
        );
    }
    
    return operand;
}


static opcode_t regSearch (const char* str)
{
    assertStrict (str, "received NULL");

    hash_t hash = djb2Hash (str, sizeof (instruction_t));
    for (size_t i = 0; i < NUM_REGS; i++)
    {
        if (Regs[i].hash == hash) return Regs[i].opcode;
    }
    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
    log_err
    (
        "syntax error",
        "unknown register (reg: %s, hash: %lu)",
        str,
        hash
    );
    return UINT8_MAX;
}

opcode_t getReg (Buffer* bufR)
{
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    instruction_t str = {0};

    if (bufScanf (bufR, REGISTERFORMAT, str) == 0)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            bufTellL (bufR),
            "syntax error",
            "no operand found"
        );
    }

    opcode_t reg = regSearch (str);

    if (reg == UINT8_MAX)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            bufTellL (bufR),
            "syntax error",
            "no operand found"
        );
    }

    return reg;
}


Erracc_t tokBrackets (Buffer* bufR, opcode_t* reg, offset_t* offset)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (reg,    "received NULL");
    assertStrict (offset, "received NULL");

    *reg    = UINT8_MAX;
    *offset = INT64_MIN;

    char str[32] = {0};
    size_t size = bufScanf (bufR, "[%31[^]]", str);
    if (size == 0 || bufGetc (bufR) != ']')
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            bufTellL (bufR),
            "syntax error",
            "no operand found: %s",
            str
        );
        return ErrAcc;
    }

    char          sign   =  0;
    instruction_t regstr = {0};

    for (size_t i = 0; i < size; i++)
    {
        if (isspace (str[i])) continue;

        if (isdigit (str[i]))
        {
            int skipped = 0;
            sscanf (&str[i], "%lld%n", offset, &skipped);

            i += (size_t)skipped - 1;
        }

        if (str[i] == '+' || str[i] == '-') sign = str[i];

        if (isalpha (str[i]))
        {
            int skipped = 0;

            sscanf (&str[i], "%s%n", regstr, &skipped);
            
            i += (size_t)skipped - 1;
        }
    }

    *offset *= sign == '-' ? -1 : 1;
    if (*regstr)
    {
        *reg = regSearch (regstr);
        if (*reg == DISP64)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_err ("syntax error", "you cant use RBP with offset");
            *reg = UINT8_MAX;
            return ErrAcc;
        }
    }
    else *reg = DISP64;

    return ErrAcc;
}