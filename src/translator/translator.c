#include "translator.h"


static void skipSpaces(FILE* file)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

    char c = fgetc (file);
    while (c == ' ' && c != EOF && c != '\n') c = fgetc (file);
    fseek (file, -1, SEEK_CUR);

    log_string ("<grn>%s has finished<dft>\n", __func__);
}

static size_t translateOperand (const char* filename, size_t instructionCounter, FILE* listing, Buffer* buf)
{
    log_string ("<grn>%s was called<dft>\n", __func__);
    operand_t operand = 0;

    skipSpaces (listing);

    if (!fscanf (listing, "#%d", &operand))
    {
        log_string (
            "%s:%llu: <b><red>syntax error:<dft> no operand found</b>\n",
            filename,
            instructionCounter + 1
        );
        exit (EXIT_FAILURE);
    }
    log_string ("  <grn>scanned operand: %d<dft>\n", operand);

    bufWrite (buf, &operand, sizeof (operand_t));
    log_string ("  <grn>wrote operand: %0X<dft>\n", operand);
    
    log_string ("<grn>%s has finished<dft>\n", __func__);
    return 0;
}


static void writeOPcode (Buffer* buf, opcode_t opcode)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

    bufWrite (buf, &opcode, sizeof (opcode_t));
    log_string ("  <grn>wrote opcode: %0X<dft>\n", opcode);

    log_string ("<grn>%s has finished<dft>\n", __func__);
}

static uint64_t writePush (const char* filename, size_t instructionCounter, Buffer* buf, FILE* listing)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

    writeOPcode (buf, PUSH);
    log_string ("  <grn>wrote opcode: %0X<dft>\n", PUSH);

    uint64_t err = translateOperand (filename, instructionCounter, listing, buf);

    log_string ("<grn>%s has finished<dft>\n", __func__);
    return err;
}


#define CASE_SIMPLEINSTRUCTION(opcode)  case opcode ## _HASH: writeOPcode (&bufW, opcode); break;

uint64_t translate (const char* input, const char* output)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

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
    log_string ("  <grn>succesfully opened(created) %s<dft>\n", input);

    FILE* bin = fopen (output, "wb+");
    if (!bin)
    {
        err |= CANTOPEN;
        log_err ("fopen error", "cant open output file: %s", input);
        return err;
    }
    log_string ("  <grn>succesfully opened(created) %s<dft>\n", output);

    fwrite (RTASM_VER, sizeof (RTASM_VER), 1, bin);

    size_t instructionCounter =  0;

    char           bufR[BUFFERSIZE] = {0};

    Buffer         bufW = {0};
    bufInit      (&bufW, BUFFERSIZE);
    bufSetStream (&bufW, bin);

    bool halt = false;
    while (fscanf (listing, "%s", bufR) > 0 && !halt)
    {
        unsigned long hash = djb2Hash (bufR, sizeof (bufR));
        log_string ("  <ylw>scanned: %s, hashed: %lu<dft>\n", bufR, hash);

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
                    "%s:%llu: <b><red>syntax error:<dft> unknown instruction</b>\n",
                    input,
                    instructionCounter + 1
                );
                err |= SYNTAX;
                return err;
        }
        instructionCounter++;
    }

    log_string ("  <grn>translated %llu opcode(s)<dft>\n", instructionCounter);
    bufWrite (&bufW, &instructionCounter, sizeof (size_t));

    fclose (listing);
    bufFree (&bufW);
    fclose (bin);

    log_string ("<grn>%s has finished<dft>\n", __func__);
    return err;
}

#undef CASE_SIMPLEINSTRUCTION