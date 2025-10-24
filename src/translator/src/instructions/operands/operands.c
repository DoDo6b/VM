#include "operands.h"


typedef struct
{
    hash_t hash;
    const char* str;
    opcode_t opcode;
}RegDescr_s;

static RegDescr_s Regs[NUM_REGS] = {};

static int regHashCmp (const void* a, const void* b)
{
    const RegDescr_s* regA = (const RegDescr_s*)a;
    const RegDescr_s* regB = (const RegDescr_s*)b;

    if (regA->hash < regB->hash) return -1;
    if (regA->hash > regB->hash) return 1;
    return 0;
}

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

    qsort (Regs, NUM_REGS, sizeof (RegDescr_s), regHashCmp);
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
            bufTellL (bufR) + 1,
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
    RegDescr_s key = {
        .hash = hash,
        .opcode = NULLOPC,
        .str = NULL,
    };

    const RegDescr_s* regDescr = (const RegDescr_s*)bsearch (&key, Regs, NUM_REGS, sizeof (RegDescr_s), regHashCmp);
    if (regDescr) return regDescr->opcode;

    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
    log_err
    (
        "syntax error",
        "unknown register (reg: %s, hash: %lu)",
        str,
        hash
    );
    return NULLOPC;
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
            bufTellL (bufR) + 1,
            "syntax error",
            "no operand found"
        );
    }

    opcode_t reg = regSearch (str);

    if (reg == NULLOPC)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            bufTellL (bufR) + 1,
            "syntax error",
            "no operand found"
        );
    }

    return reg;
}


void tokBrackets (Buffer* bufR, opcode_t* reg, offset_t* offset)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (reg,    "received NULL");
    assertStrict (offset, "received NULL");

    *reg    = NULLOPC;
    *offset = INT64_MIN;

    char str[32] = {0};
    size_t size = bufScanf (bufR, "[%31[^]]", str);
    if (size == 0 || bufGetc (bufR) != ']')
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            bufTellL (bufR) + 1,
            "syntax error",
            "no operand found: %s",
            str
        );
        return;
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
            *reg = NULLOPC;
            return;
        }
    }
    else *reg = DISP64;
    
}


void encodeBrackets (Buffer* bufR, Buffer* bufW, bool modshift)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    opcode_t mod = 0;
    unsigned char shift = modshift ? 3 : 0;

    bufSSpaces (bufR);

    if (bufpeekc (bufR) != '[')
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_UNKERR);
        log_err ("unknown error", "something went wrong");
        return;
    }

    offset_t offset = INT64_MIN;
    opcode_t reg    = NULLOPC;
    tokBrackets (bufR, &reg, &offset);

    if (ErrAcc) return;

    if (reg == NULLOPC)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_UNKERR);
        log_err ("unknown error", "something went wrong");
        return;
    }

    if (reg != DISP64 && offset != INT64_MIN)
    {
        mod = (OFF << 6) | (opcode_t)((reg & 0x07) << shift);
        bufWrite (bufW, &mod, sizeof (mod));

        bufWrite (bufW, &offset, sizeof (offset_t));
    }
    else
    {
        mod = (MEM << 6) | (opcode_t)((reg & 0x07) << shift);
        bufWrite (bufW, &mod, sizeof (mod));

        if (offset != INT64_MIN && reg == DISP64)
        {
            bufWrite (bufW, &offset, sizeof (offset_t));
        }
    }
}

void encodeReg (Buffer* bufR, Buffer* bufW, bool modshift)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    opcode_t mod = 0;
    unsigned char shift = modshift ? 3 : 0;

    bufSSpaces (bufR);

    if (!isalpha ((unsigned char)bufpeekc (bufR)))
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_UNKERR);
        log_err ("unknown error", "something went wrong");
        return;
    }

    mod = REG << 6;
    mod += (opcode_t)(getReg (bufR) << shift);
                        if (ErrAcc) return;

    bufWrite (bufW, &mod, sizeof (mod));
}

void encodeImm (Buffer* bufR, Buffer* bufW)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    opcode_t mod = 0;

    bufSSpaces (bufR);

    if (!isdigit ((unsigned char)bufpeekc (bufR)))
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_UNKERR);
        log_err ("unknown error", "something went wrong");
        return;
    }

    mod = IMM << 6;
    bufWrite (bufW, &mod, sizeof (mod));

    operand_t operand = getImm (bufR);
    if (ErrAcc) return;

    bufWrite (bufW, &operand, sizeof (operand));
}