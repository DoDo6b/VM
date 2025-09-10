#include "translator.h"


static void wSimpleInst (FILE* stream, uint8_t opcode)
{
    assertStrict (stream, "received NULL");

    fwrite (&opcode, sizeof (opcode), 1, stream);
}

static void wPush (FILE* listing, FILE* bin)
{
    assertStrict (listing, "received NULL");
    assertStrict (bin,     "received NULL");

    uint8_t opcode = PUSH;
    fwrite (&opcode, sizeof (opcode), 1, bin);

    char buffer[64] = {0};
    fscanf (listing, "%s", buffer);
    
    if (buffer[0] == VALUEPREFIX && '0' <= buffer[1] && buffer[1] <= '9') fwrite (buffer, sizeof (buffer), 1, bin);
    else
    {
        log_err ("syntax error", "cant find arg");
        exit (EXIT_FAILURE);
    }
}


uint64_t translate (const char* input, const char* output)
{
    assertStrict (input, "received NULL");

    uint64_t err = 0;

    FILE* listing = fopen (input, "r");
    if (!listing)
    {
        err |= CANTOPEN;
        log_err ("internal error", "cant open input file: %s", input);
        return err;
    }

    FILE* bin = fopen (output, "wb+");
    if (!bin)
    {
        err |= CANTOPEN;
        log_err ("internal error", "cant open output file: %s", input);
        return err;
    }
    fwrite (RTASM_VER, sizeof (RTASM_VER), 1, bin);

    char buffer[BUFSIZ];
    while (fscanf (listing, "%s", buffer) == 1)
    {
        switch (djb2Hash (buffer, sizeof (buffer)))
        {
            case PUSH_HASH: wPush (listing, bin);     break;
            case OUT_HASH:  wSimpleInst (bin, OUT);   break;
            case POP_HASH:  wSimpleInst (bin, POP);   break;
            case HALT_HASH: wSimpleInst (bin, HALT);  break;
            case ADD_HASH:  wSimpleInst (bin, ADD);   break;
            case SUB_HASH:  wSimpleInst (bin, SUB);   break;
            case MUL_HASH:  wSimpleInst (bin, MUL);   break;
            case DIV_HASH:  wSimpleInst (bin, DIV);   break;
            
            default:
                log_err ("syntax error", "unknown instruction");
        }
    }
    
    
    
    fclose (listing);
    fclose (bin);
    return err;
}