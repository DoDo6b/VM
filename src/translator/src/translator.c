#include "../translator.h"


static FILE* fileOpen (const char* fname, const char* attributes)
{
    assertStrict (fname,      "received NULL");
    assertStrict (attributes, "received NULL");

    FILE* stream = fopen (fname, attributes);

    if (!stream)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_FOPENERR);
        log_err ("fopen error", "cant open input file: %s", stream);
        return NULL;
    }

    return stream;
}


#define CASE_SIMPLEINSTRUCTION(opcode)  case opcode ## _HASH: writeOPcode (bufW, opcode); break;

static Erracc_t decompose (Buffer* bufR, Buffer* bufW, size_t* instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    
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

            case MOV_HASH:  writeMov  (bufW, bufR, *instrc); break;

            case PUSH_HASH: writePush (bufW, bufR, *instrc); break;
            
            case HALT_HASH:
                writeOPcode (bufW, HALT);
                halt = true;
                break;
            
            default:
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
                log_srcerr
                (
                    bufR->name ? bufR->name : "*fname*",
                    *instrc + 1,
                    "syntax error",
                    "unknown instruction (hash: %lu)",
                    hash
                );
                return ErrAcc;
        }
        *instrc += 1;
    }

    return ErrAcc;
}




uint64_t translate (const char* input, const char* output)
{
    assertStrict (input,  "received NULL");
    assertStrict (output, "received NULL");

    FILE* listing = fileOpen (input, "r");
    FILE* bin     = fileOpen (output, "wb+");

    Buffer*        bufR = bufInit (fileSize (listing));
    Buffer*        bufW = bufInit (BUFFERSIZE);
    bufSetStream (bufR, input,  listing, BUFREAD);
    bufSetStream (bufW, output, bin,     BUFWRITE);

    bufRead (bufR, 0);

    fseek (bin, sizeof (Header), SEEK_SET);

    Header header = {
        .sign =    RTASM_SIGN,
        .version = RTASM_VER,
        .instrc = 0,
    };

    if (decompose (bufR, bufW, &header.instrc))
    {
        log_err ("translation error", "translation has ended with code: %llu", ErrAcc);
        exit (EXIT_FAILURE);
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