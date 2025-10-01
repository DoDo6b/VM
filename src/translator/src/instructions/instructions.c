#include "instructions.h"


Erracc_t writeOPcode (Buffer* bufW, opcode_t opcode)
{
    assertStrict (bufVerify (bufW, 0) == 0, "buffer failed verification");

    if (bufWrite (bufW, &opcode, sizeof (opcode_t)) == 0)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
        log_err ("writing error", "cant write into buffer");
        IMSTP ( exit (EXIT_FAILURE); )
    }
    return ErrAcc;
}

Erracc_t writePush (Buffer* bufW, Buffer* bufR, size_t instrc)
{
    assertStrict (bufVerify (bufW, 0) == 0, "buffer failed verification");
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    opcode_t  opcode  = PUSH << OPCODESHIFT;
    operand_t operand = 0;

    bufSpaces (bufR);

    int     prefix = bufGetc (bufR);
    switch (prefix)
    {
    case VALUEPREFIX:
        writeOPcode (bufW, opcode);

        operand = translateOperand (bufR, instrc);
        if (bufWrite (bufW, &operand, sizeof (operand_t)) == 0)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
            log_err ("writing error", "cant write into buffer");
            IMSTP ( exit (EXIT_FAILURE); )
        }
        break;

    case REGISTERPREFIX:
        opcode += translateReg (bufR, instrc);
        writeOPcode (bufW, opcode);
        break;
    
    default:
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            instrc + 1,
            "syntax error",
            "unknown operand type (%c)",
            prefix
        );
        exit (EXIT_FAILURE);
    }

    return ErrAcc;
}

Erracc_t writeMov (Buffer* bufW, Buffer* bufR, size_t instrc)
{
    assertStrict (bufVerify (bufW, 0) == 0, "buffer failed verification");
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    opcode_t opcode = MOV << OPCODESHIFT;

    bufSpaces (bufR);

    if (bufGetc (bufR) != REGISTERPREFIX)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            instrc + 1,
            "syntax error",
            "MOV requires correct register"
        );
        exit (EXIT_FAILURE);
    }

    opcode += translateReg (bufR, instrc);

    writeOPcode (bufW, opcode);

    return ErrAcc;
}