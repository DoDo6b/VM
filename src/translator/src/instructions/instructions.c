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

    bufSSpaces (bufR);

    char ch = bufpeekc (bufR);
    if (isalpha ((unsigned char)ch))
    {
        encodeReg (bufR, bufW, true);
        return;
    }
    else if (isdigit ((int)ch))
    {
        encodeImm (bufR, bufW);
        return;
    }
    else if (ch == '[')
    {
        encodeBrackets (bufR, bufW, true);
        return;
    }
    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
    log_srcerr
    (
        bufR->name,
        bufTellL (bufR) + 1,
        "syntax error",
        "push needs operand"
    );
}

void handleMOV (Buffer* bufR, Buffer* bufW)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    opcode_t opc = OPC_MOV;
    bufWrite (bufW, &opc, sizeof (opc));

    bufSSpaces (bufR);

    if (bufpeekc (bufR) == '[')
    {
        encodeBrackets (bufR, bufW, false);
        return;
    }
    else if (isalpha ((unsigned char)bufpeekc (bufR)))
    {
        encodeReg (bufR, bufW, false);
        return;
    }

    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
    log_srcerr
    (
        bufR->name,
        bufTellL (bufR) + 1,
        "syntax error",
        "mov needs operand"
    );
}