#include "../translator.h"


static FILE* fileOpen (const char* fname, const char* attributes)
{
    assertStrict (fname,      "received NULL");
    assertStrict (attributes, "received NULL");

    FILE* stream = fopen (fname, attributes);

    if (!stream)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_FSERR);
        log_err ("fopen error", "cant open input file: %s", stream);
        return NULL;
    }

    return stream;
}


static bool sSpaceLines (Buffer* buf)
{
    assertStrict (bufVerify (buf, 0) == 0 && buf->mode == BUFREAD,  "bufR failed verification");

    while (*buf->bufpos == '\0') 
    {
        if (bufNLine (buf) == -1) return false;
    }
    return true;
}


static void parseSpecial (const char* instr, Buffer* bufR, Buffer* bufW)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    assertStrict (instr, "received NULL");

    instruction_t nothing = {};
    char key = 0;
    if (sscanf (instr, "%[^:]%c", nothing, &key) && key == ':') 
    {
        labelDecl (instr, bufW);
        return;
    }

    ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
    log_srcerr
    (
        bufR->name ? bufR->name : "*fname*",
        bufTellL (bufR) + 1,
        "syntax error",
        "unknown instruction (instr: %s hash: %lu)",
        instr,
        djb2Hash (instr, sizeof (instruction_t))
    );
}

static size_t parse (Buffer* bufR, Buffer* bufW)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    
    reginit ();
    descriptionsInit ();
    size_t instrc = 0;
    instruction_t instruction = {0};


    while (sSpaceLines (bufR) && bufScanf (bufR, "%s", instruction) > 0)
    {
        if (*instruction == ';')
        {
            bufNLine (bufR);
            continue;
        }

        hash_t hash = djb2Hash (instruction, sizeof (instruction));

        const iDescription_s* instructionDescr = descriptionSearch (hash);
        if (instructionDescr) instructionDescr->encode (bufR, bufW);
        else parseSpecial (instruction, bufR, bufW);


        instrc++;

        bufSSpaces (bufR);
        if (bufpeekc (bufR) != '\0' && bufpeekc (bufR) != ';')
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
            log_srcerr
            (
                bufR->name,
                bufTellL (bufR) + 1,
                "syntax error",
                "multiple instructions in line"
            );
        }
        
        if (ErrAcc)
        {
            log_srcerr
            (
                bufR->name,
                bufTellL (bufR) + 1,
                "runtime error",
                "aborting"
            );
            return 0;
        }
    }


    if (remUnmngldJMP () != 0)
    {
        jmpWLdump ();
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_err ("syntax error", "there are still unrelated jmp");
        return 0;
    }

    return instrc;
}



Erracc_t translate (const char* input, const char* output)
{
    assertStrict (input,  "received NULL");
    assertStrict (output, "received NULL");

    FILE* listing = fileOpen (input, "r");
    FILE* bin     = fileOpen (output, "wb+");

    if (fileSize (listing) == 0)
    {
        fclose (listing);
        fclose (bin);
        log_err ("error", "file is empty");
        return ErrAcc;
    }

    Buffer*        bufR = bufInit ((size_t)fileSize (listing));
    Buffer*        bufW = bufInit (BUFFERSIZE);
    if (!bufR || !bufW)
    {
        log_err ("init error", "buffers wasnt initialized");
        return ErrAcc;
    }

    bufSetStream (bufR, input,  listing, BUFREAD);
    bufSetStream (bufW, output, bin,     BUFWRITE);

    bufRead (bufR, 0);
    fclose (listing);

    bufLSplit (bufR);
    fseek (bin, sizeof (Header), SEEK_SET);

    Header header = {
        .sign =    RTASM_SIGN,
        .version = RTASM_VER,
        .instrc =  0,
    };


    header.instrc = parse (bufR, bufW);


    log_string ("<grn>translated %llu opcode(s)<dft>\n", header.instrc);

    bufFree (bufW);
    bufFree (bufR);

    fseek  (bin, 0, SEEK_SET);
    fwrite (&header, sizeof (Header), 1, bin);

    fclose (bin);

    return ErrAcc;
}