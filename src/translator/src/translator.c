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


static Erracc_t decomposeSpecial (const char* instr, Buffer* bufR, Buffer* bufW, size_t instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    assertStrict (instr, "received NULL");

    instruction_t nothing = {0};
    char key = 0;
    if (sscanf (instr, "%[^:]%c", nothing, &key) && key == ':') return decomposeChpoint (instr, bufW);

    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
    log_srcerr
    (
        bufR->name ? bufR->name : "*fname*",
        instrc + 1,
        "syntax error",
        "unknown instruction (instr: %s hash: %lu)",
        instr,
        djb2Hash (instr, sizeof (instruction_t))
    );
    return ErrAcc;
}


#define CASE_SIMPLEINSTRUCTION(opcode)  case opcode ## _HASH: writeOPcode (bufW, opcode); break;

static Erracc_t decompose (Buffer* bufR, Buffer* bufW, size_t* instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    assertStrict (instrc, "received NULL");
    
    instruction_t instruction = {0};

    while (bufScanf (bufR, "%s", instruction) > 0)
    {
        if (*instruction == ';')
        {
            bufR->bufpos = strchr (bufR->bufpos, '\n');
            continue;
        }

        hash_t hash = djb2Hash (instruction, sizeof (instruction));

        switch (hash)
        {
            CASE_SIMPLEINSTRUCTION (HALT)
            CASE_SIMPLEINSTRUCTION (OUT)
            CASE_SIMPLEINSTRUCTION (POP)
            CASE_SIMPLEINSTRUCTION (CMP)
            CASE_SIMPLEINSTRUCTION (ADD)
            CASE_SIMPLEINSTRUCTION (SUB)
            CASE_SIMPLEINSTRUCTION (MUL)
            CASE_SIMPLEINSTRUCTION (DIV)

            case MOV_HASH:  writeMov  (bufW, bufR, *instrc); break;

            case JMP_HASH: decomposeJMP (bufR, bufW, *instrc, JMP_NOCOND); break;
            case JNZ_HASH: decomposeJMP (bufR, bufW, *instrc, JMP_NZERO);  break;
            case JZ_HASH:  decomposeJMP (bufR, bufW, *instrc, JMP_ZERO);   break;
            case JL_HASH:  decomposeJMP (bufR, bufW, *instrc, JMP_LESS);   break;
            case JLE_HASH: decomposeJMP (bufR, bufW, *instrc, JMP_LEQ);    break;
            case JG_HASH:  decomposeJMP (bufR, bufW, *instrc, JMP_GRTR);   break;
            case JGE_HASH: decomposeJMP (bufR, bufW, *instrc, JMP_GEQ);    break;

            case PUSH_HASH: writePush (bufW, bufR, *instrc); break;
            
            default:  decomposeSpecial (instruction, bufR, bufW, *instrc);
        }
        *instrc += 1;

        if (ErrAcc)
        {
            log_err ("runtime error", "aborting");
            break;
        }
    }

    if (remainingUnprocJMPReq () != 0)
    {
        jmpWLdump ();
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_err ("syntax error", "missing jmptags");
    }

    return ErrAcc;
}

#undef CASE_SIMPLEINSTRUCTION



uint64_t translate (const char* input, const char* output)
{
    assertStrict (input,  "received NULL");
    assertStrict (output, "received NULL");

    FILE* listing = fileOpen (input, "r");
    FILE* bin     = fileOpen (output, "wb+");

    Buffer*        bufR = bufInit ((size_t)fileSize (listing));
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