#include "instructions.h"


Erracc_t writeOPcode (Buffer* bufW, opcode_t opcode)
{
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    if (bufWrite (bufW, &opcode, sizeof (opcode_t)) == 0)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
        log_err ("buffer error", "cant write into buffer");
        return ErrAcc;
    }
    return ErrAcc;
}

Erracc_t writePush (Buffer* bufW, Buffer* bufR, size_t instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    if (writeOPcode (bufW, PUSH)) return ErrAcc;

    opcode_t mod = 0;

    bufSSpaces (bufR);

    char ch = bufpeekc (bufR);
    if (isalpha ((unsigned char)ch))
    {
        mod = REG << 6;
        mod += (opcode_t)(translateReg(bufR, instrc) << 3);
                          if (ErrAcc) return ErrAcc;

        if (writeOPcode (bufW, mod)) return ErrAcc;
    }
    else if (isdigit ((int)ch))
    {
        mod = IMM << 6;
        if (writeOPcode (bufW, mod)) return ErrAcc;

        operand_t operand = translateOperand (bufR, instrc);
                            if (ErrAcc) return ErrAcc;

        if (bufWrite (bufW, &operand, sizeof (operand_t)) == 0)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
            log_err ("buffer error", "cant write into buffer");
            return ErrAcc;
        }
    }
    else if (ch == '[')
    {
        offset_t offset = INT64_MIN;
        opcode_t reg    = UINT8_MAX;
        decomposeMemcall (bufR, &reg, &offset, instrc);
        if (ErrAcc) return ErrAcc;

        if (reg == UINT8_MAX)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_UNKERR);
            log_err ("unknown error", "something went wrong");
            return ErrAcc;
        }

        if (reg != DISP64 && offset != INT64_MIN)
        {
            mod = (OFF << 6) | (opcode_t)((reg & 0x07) << 3);
            if (writeOPcode (bufW, mod)) return ErrAcc;

            if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
            {
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
                log_err ("buffer error", "cant write into buffer");
                return ErrAcc;
            }

            return ErrAcc;
        }
        else
        {
            mod = (MEM << 6) | (opcode_t)((reg & 0x07) << 3);
            if (writeOPcode (bufW, mod)) return ErrAcc;

            if (offset != INT64_MIN && reg == DISP64)
            {
                if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
                {
                    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
                    log_err ("buffer error", "cant write into buffer");
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
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    if (writeOPcode (bufW, MOV)) return ErrAcc;

    opcode_t mod = 0;
    bufSSpaces (bufR);

    if (bufpeekc (bufR) == '[')
    {
        offset_t offset = INT64_MIN;
        opcode_t reg    = UINT8_MAX;
        decomposeMemcall (bufR, &reg, &offset, instrc);
        if (ErrAcc) return ErrAcc;

        if (reg == UINT8_MAX)
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_UNKERR);
            log_err ("internal error", "cant decompose bracets");
            return ErrAcc;
        }

        if (reg != DISP64 && offset != INT64_MIN)
        {
            mod = (OFF << 6) | (reg & 0x07);
            if (writeOPcode (bufW, mod)) return ErrAcc;

            if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
            {
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
                log_err ("buffer error", "cant write into buffer");
                return ErrAcc;
            }

            return ErrAcc;
        }
        else
        {
            mod = (MEM << 6) | (reg & 0x07);
            if (writeOPcode (bufW, mod)) return ErrAcc;

            if (offset != INT64_MIN && reg == DISP64)
            {
                if (bufWrite (bufW, &offset, sizeof (offset_t)) == 0)
                {
                    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
                    log_err ("buffer error", "cant write into buffer");
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
                   if (ErrAcc) return ErrAcc;

            if (writeOPcode (bufW, mod)) return ErrAcc;
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