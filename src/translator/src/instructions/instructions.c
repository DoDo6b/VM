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
    log_string ("has wrote: %0X\n", opcode);
    return ErrAcc;
}

Erracc_t writePush (Buffer* bufW, Buffer* bufR, size_t instrc)
{
    assertStrict (bufVerify (bufW, 0) == 0, "buffer failed verification");
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    writeOPcode (bufW, PUSH);

    opcode_t mod = 0;

    bufSSpaces (bufR);

    char ch = bufpeekc (bufR);
    if (isalpha ((unsigned char)ch))
    {
        mod = REG << 6;
        mod += (opcode_t)(translateReg(bufR, instrc) << 3);
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
        offset_t offset = INT64_MIN;
        opcode_t reg    = UINT8_MAX;
        decomposeMemcall (bufR, &reg, &offset, instrc);

        if (reg == UINT8_MAX)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_INTERNALERROR);
            log_err ("internal error", "cant decompose bracets");
            return ErrAcc;
        }

        if (reg != DISP64 && offset != INT64_MIN)
        {
            mod = (OFF << 6) | (opcode_t)((reg & 0x07) << 3);
            writeOPcode (bufW, mod);

            if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
            {
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
                log_err ("writing error", "cant write into buffer");
                return ErrAcc;
            }

            return ErrAcc;
        }
        else
        {
            mod = (MEM << 6) | (opcode_t)((reg & 0x07) << 3);
            writeOPcode (bufW, mod);

            if (offset != INT64_MIN && reg == DISP64)
            {
                if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
                {
                    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
                    log_err ("writing error", "cant write into buffer");
                    return ErrAcc;
                }
            }

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
    bufSSpaces (bufR);

    if (bufpeekc (bufR) == '[')
    {
        offset_t offset = INT64_MIN;
        opcode_t reg    = UINT8_MAX;
        decomposeMemcall (bufR, &reg, &offset, instrc);

        if (reg == UINT8_MAX)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_INTERNALERROR);
            log_err ("internal error", "cant decompose bracets");
            return ErrAcc;
        }

        if (reg != DISP64 && offset != INT64_MIN)
        {
            mod = (OFF << 6) | (reg & 0x07);
            writeOPcode (bufW, mod);

            if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
            {
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
                log_err ("writing error", "cant write into buffer");
                return ErrAcc;
            }

            return ErrAcc;
        }
        else
        {
            mod = (MEM << 6) | (reg & 0x07);
            writeOPcode (bufW, mod);

            if (offset != INT64_MIN && reg == DISP64)
            {
                if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
                {
                    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
                    log_err ("writing error", "cant write into buffer");
                    return ErrAcc;
                }
            }

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