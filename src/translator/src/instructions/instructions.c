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

    writeOPcode (bufW, PUSH);

    opcode_t mod = 0;

    bufSpaces (bufR);

    char ch = bufpeekc (bufR);
    if (isalpha ((unsigned char)ch))
    {
        mod = REG << 6;
        mod += translateReg(bufR, instrc);
        writeOPcode (bufW, mod);
    }
    else if (isdigit ((int)ch))
    {
        mod = IMM << 6;
        writeOPcode (bufW, mod);
        operand_t operand = translateOperand (bufR, instrc);
        if (bufWrite (bufW, &operand, sizeof (operand_t)) == 0)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
            log_err ("writing error", "cant write into buffer");
            return ErrAcc;
        }
    }
    else if (ch == '[')
    {
        bufSeek (bufR, 1, SEEK_CUR);
        mod = MEM << 6;
        writeOPcode (bufW, mod);

        pointer_t ptr = 0;

        char* endptr = NULL;
        ptr = (pointer_t)strtoumax(bufR->bufpos, &endptr, 16);

        if (!endptr || *endptr != ']')
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_srcerr
            (
                bufR->name,
                instrc,
                "syntax error",
                "no closing bracets ]"
            );
            return ErrAcc;
        }
        bufR->bufpos = endptr + 1;

        if (bufWrite (bufW, &ptr, sizeof (pointer_t)) == 0)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
            log_err ("writing error", "cant write into buffer");
            return ErrAcc;
        }
    }
    else
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_srcerr
            (
                bufR->name,
                instrc,
                "syntax error",
                "push needs operand"
            );
            return ErrAcc;
    }

    return ErrAcc;
}

Erracc_t writeMov (Buffer* bufW, Buffer* bufR, size_t instrc)
{
    assertStrict (bufVerify (bufW, 0) == 0, "buffer failed verification");
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    writeOPcode (bufW, MOV);

    opcode_t mod = 0;
    bufSpaces (bufR);

    if (bufpeekc (bufR) == '[')
    {
        bufSeek (bufR, 1, SEEK_CUR);

        mod = MEM << 6;
        writeOPcode (bufW, mod);

        pointer_t ptr = 0;

        char* endptr = NULL;
        ptr = (pointer_t)strtoumax(bufR->bufpos, &endptr, 16);

        if (!endptr || *endptr != ']')
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_srcerr
            (
                bufR->name,
                instrc,
                "syntax error",
                "no closing bracets ]"
            );
            return ErrAcc;
        }
        bufR->bufpos = endptr+1;

        if (bufWrite (bufW, &ptr, sizeof (pointer_t)) == 0)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
            log_err ("writing error", "cant write into buffer");
            return ErrAcc;
        }
    }
    else
    {
        if (isalpha ((unsigned char)bufpeekc(bufR)))
        {
            mod = REG << 6;
            mod += translateReg(bufR, instrc);
            writeOPcode (bufW, mod);
        }
        else
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_srcerr
            (
                bufR->name,
                instrc,
                "syntax error",
                "mov needs operand"
            );
            return ErrAcc;
        }
    }

    return ErrAcc;
}