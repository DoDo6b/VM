#include "translator.h"


static void skipSpaces(FILE* file)
{
    char c = 0;
    do c = (char)fgetc (file);
    while (c == ' ' && c != EOF && c != '\n');

    fseek (file, -1, SEEK_CUR);
}


static operand_t translateOperand (const char* filename, size_t instructionCounter, FILE* listing)
{
    assertStrict (filename, "received NULL");
    assertStrict (listing,  "received NULL");

    operand_t operand = 0;

    if (!fscanf (listing, VALUEFORMAT, &operand))
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

static opcode_t translateReg (const char* filename, size_t instructionCounter, FILE* listing)
{
    assertStrict (filename, "received NULL");
    assertStrict (listing,  "received NULL");

    char reg[4] = {0};

    if (fscanf (listing, REGISTERFORMAT, reg) == 0)
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


static uint64_t writeOPcode (Buffer* buf, opcode_t opcode)
{
    if (bufWrite (buf, &opcode, sizeof (opcode_t)) == 0)
    {
        ErrAcc |= WRITINGERROR;
        log_err ("writing error", "cant write into buffer");
        IMSTP ( exit (EXIT_FAILURE); )
    }
    return ErrAcc;
}

static uint64_t writePush (const char* filename, size_t instructionCounter, Buffer* buf, FILE* listing)
{
    assertStrict (filename, "received NULL");
    assertStrict (listing,  "received NULL");
    assertStrict (buf,      "received NULL");

    opcode_t  opcode  = PUSH << OPCODESHIFT;
    operand_t operand = 0;

    skipSpaces (listing);

    int     prefix = fgetc (listing);
    switch (prefix)
    {
    case VALUEPREFIX:
        writeOPcode (buf, opcode);

        operand = translateOperand (filename, instructionCounter, listing);
        if (bufWrite (buf, &operand, sizeof (operand_t)) == 0)
        {
            ErrAcc |= WRITINGERROR;
            log_err ("writing error", "cant write into buffer");
            IMSTP ( exit (EXIT_FAILURE); )
        }
        break;

    case REGISTERPREFIX:
        opcode += translateReg (filename, instructionCounter, listing);
        writeOPcode (buf, opcode);
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

static uint64_t writeMov (const char* filename, size_t instructionCounter, Buffer* buf, FILE* listing)
{
    assertStrict (filename, "received NULL");
    assertStrict (listing,  "received NULL");
    assertStrict (buf,      "received NULL");

    opcode_t opcode = MOV << OPCODESHIFT;

    skipSpaces (listing);

    if (fgetc (listing) != REGISTERPREFIX)
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

    opcode += translateReg (filename, instructionCounter, listing);

    writeOPcode (buf, opcode);

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

    //Buffer*        bufR = bufInit (fileSize (listing));
    char           bufR[BUFFERSIZE] = {0};
    Buffer*        bufW = bufInit (BUFFERSIZE);
    //bufSetStream (bufR, listing, 'r');
    bufSetStream (bufW, bin,     'w');

    //bufRead (bufR, 0);

    fseek (bin, sizeof (Header), SEEK_SET);

    Header header = {
        .sign =    RTASM_SIGN,
        .version = RTASM_VER,
        .instrc = 0,
    };

    bool halt = false;
    while (fscanf (listing, "%s", bufR) > 0 && !halt)
    {
        if (*bufR == ';')
        {
            fgets (bufR, sizeof (bufR), listing);
            continue;
        }

        unsigned long hash = djb2Hash (bufR, sizeof (bufR));

        switch (hash)
        {
            CASE_SIMPLEINSTRUCTION (OUT)
            CASE_SIMPLEINSTRUCTION (POP)
            CASE_SIMPLEINSTRUCTION (ADD)
            CASE_SIMPLEINSTRUCTION (SUB)
            CASE_SIMPLEINSTRUCTION (MUL)
            CASE_SIMPLEINSTRUCTION (DIV)

            case MOV_HASH:  writeMov  (input, header.instrc, bufW, listing); break;

            case PUSH_HASH: writePush (input, header.instrc, bufW, listing); break;
            
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
    fseek  (bin, 0, SEEK_SET);
    fwrite (&header, sizeof (Header), 1, bin);
    
    fclose (listing);
    fclose (bin);

    return ErrAcc;
}

#undef CASE_SIMPLEINSTRUCTION