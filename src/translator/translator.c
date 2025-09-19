#include "translator.h"


static operand_t translateOperand (const char* filename, size_t instructionCounter, Buffer* bufR)
{
    assertStrict (filename, "received NULL");
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    operand_t operand = 0;

    if (!bufScanf (bufR, VALUEFORMAT, &operand))
    {
        ErrAcc |= SYNTAX;
        log_string (
            "%s:%llu: <b><red>syntax error:<dft> no operand found</b>\n",
            filename,
            instructionCounter + 1
        );
        log_string (
            "    | errAcc: %llu\n",
            ErrAcc
        );
        exit (EXIT_FAILURE);
    }
    
    return operand;
}

#define CASE_REG(reg)  case reg ## _HASH: return reg;

static opcode_t translateReg (const char* filename, size_t instructionCounter, Buffer* bufR)
{
    assertStrict (filename, "received NULL");
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    char reg[4] = {0};

    if (bufScanf (bufR, REGISTERFORMAT, reg) == 0)
    {
        ErrAcc |= SYNTAX;
        log_string (
            "%s:%llu: <b><red>syntax error:<dft> no operand found</b>\n",
            filename,
            instructionCounter + 1
        );
        log_string (
            "    | errAcc: %llu\n",
            ErrAcc
        );
        exit (EXIT_FAILURE);
    }

    unsigned long hash = djb2Hash (reg, sizeof (reg));
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
            ErrAcc |= SYNTAX;
            log_string (
                "%s:%llu: <b><red>syntax error:<dft> unknown register (hash: %lu)</b>\n",
                filename,
                instructionCounter + 1,
                hash
            );
            log_string (
                "    | errAcc: %llu\n",
                ErrAcc
            );
            exit (EXIT_FAILURE);
    }
}

#undef CASE_REG


static uint64_t writeOPcode (Buffer* bufW, opcode_t opcode)
{
    assertStrict (bufVerify (bufW, 0) == 0, "buffer failed verification");

    if (bufWrite (bufW, &opcode, sizeof (opcode_t)) == 0)
    {
        ErrAcc |= WRITINGERROR;
        log_err ("writing error", "cant write into buffer");
        IMSTP ( exit (EXIT_FAILURE); )
    }
    return ErrAcc;
}

static uint64_t writePush (const char* filename, size_t instructionCounter, Buffer* bufW, Buffer* bufR)
{
    assertStrict (filename, "received NULL");
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

        operand = translateOperand (filename, instructionCounter, bufR);
        if (bufWrite (bufW, &operand, sizeof (operand_t)) == 0)
        {
            ErrAcc |= WRITINGERROR;
            log_err ("writing error", "cant write into buffer");
            IMSTP ( exit (EXIT_FAILURE); )
        }
        break;

    case REGISTERPREFIX:
        opcode += translateReg (filename, instructionCounter, bufR);
        writeOPcode (bufW, opcode);
        break;
    
    default:
        ErrAcc |= SYNTAX;
        log_string (
            "%s:%llu: <b><red>syntax error:<dft> unknown operand type (%c)</b>\n",
            filename,
            instructionCounter + 1,
            prefix
        );
        log_string (
            "    | errAcc: %llu\n",
            ErrAcc
        );
        exit (EXIT_FAILURE);
    }

    return ErrAcc;
}

static uint64_t writeMov (const char* filename, size_t instructionCounter, Buffer* bufW, Buffer* bufR)
{
    assertStrict (filename, "received NULL");
    assertStrict (bufVerify (bufW, 0) == 0, "buffer failed verification");
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    opcode_t opcode = MOV << OPCODESHIFT;

    bufSpaces (bufR);

    if (bufGetc (bufR) != REGISTERPREFIX)
    {
        ErrAcc |= SYNTAX;
        log_string (
            "%s:%llu: <b><red>syntax error:<dft> MOV requires correct register</b>\n",
            filename,
            instructionCounter + 1
        );
        log_string (
            "    | errAcc: %llu\n",
            ErrAcc
        );
        exit (EXIT_FAILURE);
    }

    opcode += translateReg (filename, instructionCounter, bufR);

    writeOPcode (bufW, opcode);

    return ErrAcc;
}


static FILE* fileOpen (const char* fname, const char* attributes)
{
    assertStrict (fname,      "received NULL");
    assertStrict (attributes, "received NULL");

    FILE* stream = fopen (fname, attributes);

    if (!stream)
    {
        ErrAcc |= CANTOPEN;
        log_err ("fopen error", "cant open input file: %s", stream);
        return NULL;
    }

    return stream;
}

#define CASE_SIMPLEINSTRUCTION(opcode)  case opcode ## _HASH: writeOPcode (bufW, opcode << OPCODESHIFT); break;

uint64_t translate (const char* input, const char* output)
{
    assertStrict (input,  "received NULL");
    assertStrict (output, "received NULL");

    FILE* listing = fileOpen (input, "r");
    FILE* bin     = fileOpen (output, "wb+");

    Buffer*        bufR = bufInit (fileSize (listing));
    Buffer*        bufW = bufInit (BUFFERSIZE);
    bufSetStream (bufR, listing, BUFREAD);
    bufSetStream (bufW, bin,     BUFWRITE);

    bufRead (bufR, 0);

    fseek (bin, sizeof (Header), SEEK_SET);

    Header header = {
        .sign =    RTASM_SIGN,
        .version = RTASM_VER,
        .instrc = 0,
    };

    bool halt = false;
    char instruction[16];
    while (bufScanf (bufR, "%s", instruction) && !halt)
    {
        if (*instruction == ';')
        {
            bufR->bufpos = strchr (bufR->bufpos, '\n');
            continue;
        }


        unsigned long hash = djb2Hash (instruction, sizeof (instruction));

        switch (hash)
        {
            CASE_SIMPLEINSTRUCTION (OUT)
            CASE_SIMPLEINSTRUCTION (POP)
            CASE_SIMPLEINSTRUCTION (ADD)
            CASE_SIMPLEINSTRUCTION (SUB)
            CASE_SIMPLEINSTRUCTION (MUL)
            CASE_SIMPLEINSTRUCTION (DIV)

            case MOV_HASH:  writeMov  (input, header.instrc, bufW, bufR); break;

            case PUSH_HASH: writePush (input, header.instrc, bufW, bufR); break;
            
            case HALT_HASH:
                writeOPcode (bufW, HALT << OPCODESHIFT);
                halt = true;
                break;
            
            default:
                ErrAcc |= SYNTAX;
                log_string (
                    "%s:%llu: <b><red>syntax error:<dft> unknown instruction (hash: %lu)</b>\n",
                    input,
                    header.instrc + 1,
                    hash
                );
                log_string (
                    "    | errAcc: %llu\n",
                    ErrAcc
                );
                return ErrAcc;
        }
        header.instrc++;
    }

    log_string ("<grn>translated %llu opcode(s)<dft>\n", header.instrc);

    bufFree (bufW);
    bufFree (bufR);
    fseek  (bin, 0, SEEK_SET);
    fwrite (&header, sizeof (Header), 1, bin);
    
    fclose (listing);
    fclose (bin);

    return ErrAcc;
}

#undef CASE_SIMPLEINSTRUCTION