#include "instructions.h"


#define HANDLE_SIMPLE(name)                                                                         \
void handle ## name (Buffer* FICTIVE, Buffer* bufW)                                                 \
{                                                                                                   \
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");  \
                                                                                                    \
    opcode_t opc = OPC_ ## name;                                                                    \
    if (bufWrite (bufW, &opc, sizeof (opcode_t)) == 0)                                              \
    {                                                                                               \
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);                                                  \
        log_err ("buffer error", "cant write into buffer");                                         \
        return;                                                                                     \
    }                                                                                               \
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

HANDLE_SIMPLE (HALT)
HANDLE_SIMPLE (IN)
HANDLE_SIMPLE (OUT)
HANDLE_SIMPLE (POP)
HANDLE_SIMPLE (CMP)
HANDLE_SIMPLE (ADD)
HANDLE_SIMPLE (SUB)
HANDLE_SIMPLE (MUL)
HANDLE_SIMPLE (DIV)
HANDLE_SIMPLE (DRAW)
HANDLE_SIMPLE (DMP)
HANDLE_SIMPLE (RET)

#pragma GCC diagnostic pop

#undef HANDLE_SIMPLE


void handlePUSH (Buffer* bufR, Buffer* bufW)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    opcode_t opc = OPC_PUSH;
    bufWrite (bufW, &opc, sizeof (opc));

    opcode_t mod = 0;

    bufSSpaces (bufR);

    char ch = bufpeekc (bufR);
    if (isalpha ((unsigned char)ch))
    {
        mod = REG << 6;
        mod += (opcode_t)(getReg (bufR) << 3);
                          if (ErrAcc) return;

        bufWrite (bufW, &mod, sizeof (mod));
    }
    else if (isdigit ((int)ch))
    {
        mod = IMM << 6;
        bufWrite (bufW, &mod, sizeof (mod));

        operand_t operand = getImm (bufR);
        if (ErrAcc) return;

        bufWrite (bufW, &operand, sizeof (operand));
    }
    else if (ch == '[')
    {
        offset_t offset = INT64_MIN;
        opcode_t reg    = UINT8_MAX;
        tokBrackets (bufR, &reg, &offset);
        if (ErrAcc) return;

        if (reg == UINT8_MAX)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_UNKERR);
            log_err ("unknown error", "something went wrong");
            return;
        }

        if (reg != DISP64 && offset != INT64_MIN)
        {
            mod = (OFF << 6) | (opcode_t)((reg & 0x07) << 3);
            bufWrite (bufW, &mod, sizeof (mod));

            if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
            {
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
                log_err ("buffer error", "cant write into buffer");
                return;
            }

            return;
        }
        else
        {
            mod = (MEM << 6) | (opcode_t)((reg & 0x07) << 3);
            bufWrite (bufW, &mod, sizeof (mod));

            if (offset != INT64_MIN && reg == DISP64)
            {
                if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
                {
                    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
                    log_err ("buffer error", "cant write into buffer");
                    return;
                }
            }

            return;
        }
    }
    else
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_srcerr
            (
                bufR->name,
                bufTellL (bufR),
                "syntax error",
                "push needs operand"
            );
            return;
    }

    return;
}

void handleMOV (Buffer* bufR, Buffer* bufW)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    opcode_t opc = OPC_MOV;
    bufWrite (bufW, &opc, sizeof (opc));

    opcode_t mod = 0;
    bufSSpaces (bufR);

    if (bufpeekc (bufR) == '[')
    {
        offset_t offset = INT64_MIN;
        opcode_t reg    = UINT8_MAX;
        tokBrackets (bufR, &reg, &offset);
        if (ErrAcc) return;

        if (reg == UINT8_MAX)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_UNKERR);
            log_err ("unknown error", "something went wrong");
            return;
        }

        if (reg != DISP64 && offset != INT64_MIN)
        {
            mod = (OFF << 6) | (reg & 0x07);
            bufWrite (bufW, &mod, sizeof (mod));

            bufWrite (bufW, &offset, sizeof (offset));

            return;
        }
        else
        {
            mod = (MEM << 6) | (reg & 0x07);
            bufWrite (bufW, &mod, sizeof (mod));

            if (offset != INT64_MIN && reg == DISP64) bufWrite (bufW, &offset, sizeof (offset_t));

            return;
        }
    }
    else
    {
        if (isalpha ((unsigned char)bufpeekc (bufR)))
        {
            mod = REG << 6;
            mod += getReg (bufR);
            if (ErrAcc) return;

            bufWrite (bufW, &mod, sizeof (mod));
        }
        else
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_srcerr
            (
                bufR->name,
                bufTellL (bufR),
                "syntax error",
                "mov needs operand"
            );
            return;
        }
    }

    return;
}