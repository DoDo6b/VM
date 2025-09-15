#include "translator.h"


static void skipSpaces(FILE* file)
{
    char c = 0;
    do c = (char)fgetc (file);
    while (c == ' ' && c != EOF && c != '\n');

    fseek (file, -1, SEEK_CUR);
}

static size_t translateOperand (const char* filename, size_t instructionCounter, FILE* listing, Buffer* buf)
{
    operand_t operand = 0;

    skipSpaces (listing);

    if (!fscanf (listing, VALUEFORMAT, &operand))
    {
        log_string (
            "%s:%llu: <b><red>syntax error:<dft> no operand found</b>\n",
            filename,
            instructionCounter + 1
        );
        exit (EXIT_FAILURE);
    }

    bufWrite (buf, &operand, sizeof (operand_t));
    
    return 0;
}


static void writeOPcode (Buffer* buf, opcode_t opcode)
{
    bufWrite (buf, &opcode, sizeof (opcode_t));
}

static uint64_t writePush (const char* filename, size_t instructionCounter, Buffer* buf, FILE* listing)
{
    writeOPcode (buf, PUSH);

    uint64_t err = translateOperand (filename, instructionCounter, listing, buf);

    return err;
}


#define CASE_SIMPLEINSTRUCTION(opcode)  case opcode ## _HASH: writeOPcode (&bufW, opcode); break;

uint64_t translate (const char* input, const char* output)
{

    assertStrict (input,  "received NULL");
    assertStrict (output, "received NULL");

    uint64_t err = 0;

    FILE* listing = fopen (input, "r");
    if (!listing)
    {
        err |= CANTOPEN;
        log_err ("fopen error", "cant open input file: %s", input);
        return err;
    }

    FILE* bin = fopen (output, "wb+");
    if (!bin)
    {
        err |= CANTOPEN;
        log_err ("fopen error", "cant open output file: %s", input);
        return err;
    }

    fwrite (RTASM_VER, sizeof (RTASM_VER), 1, bin);

    size_t instructionCounter =  0;

    char           bufR[BUFFERSIZE] = {0};
    Buffer         bufW = {0};
    bufInit      (&bufW, BUFFERSIZE);
    bufSetStream (&bufW, bin);

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

            case PUSH_HASH: err |= writePush (input, instructionCounter, &bufW, listing); break;
            
            case HALT_HASH:
                writeOPcode (&bufW, HALT);
                halt = true;
                break;
            
            default:
                log_string (
                    "%s:%llu: <b><red>syntax error:<dft> unknown instruction (hash: %lu)</b>\n",
                    input,
                    instructionCounter + 1,
                    hash
                );
                err |= SYNTAX;
                return err;
        }
        instructionCounter++;
    }

    log_string ("<grn>translated %llu opcode(s)<dft>\n", instructionCounter);
    bufWrite (&bufW, &instructionCounter, sizeof (size_t));

    fclose (listing);
    bufFree (&bufW);
    fclose (bin);

    return err;
}

#undef CASE_SIMPLEINSTRUCTION