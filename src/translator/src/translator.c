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


typedef struct
{
    hash_t hash;
    const char* str;
    opcode_t opcode;
    unsigned char argReq;
    void (*handler)(Buffer*, Buffer*);
}Instruction_s;
static Instruction_s Instructions[NUM_OPS] = {};

#define INSTR_DESCR(instr, argc, i)\
    Instructions[i].str = #instr;\
    Instructions[i].hash = djb2Hash (#instr, sizeof (#instr));\
    Instructions[i].opcode = OPC_ ## instr;\
    Instructions[i].argReq = argc;\
    Instructions[i].handler = handle ## instr;

void instrinit ()
{
    reginit ();

    INSTR_DESCR (HALT, 0, 0)
    INSTR_DESCR (IN,   0, 1)
    INSTR_DESCR (OUT,  0, 2)
    INSTR_DESCR (POP,  0, 3)
    INSTR_DESCR (PUSH, 1, 4)
    INSTR_DESCR (MOV,  1, 5)
    INSTR_DESCR (RET,  0, 6)
    INSTR_DESCR (CMP,  0, 7)
    INSTR_DESCR (ADD,  0, 8)
    INSTR_DESCR (SUB,  0, 9)
    INSTR_DESCR (MUL,  0, 10)
    INSTR_DESCR (DIV,  0, 11)
    INSTR_DESCR (DMP,  0, 12)
    INSTR_DESCR (DRAW, 0, 13)

    INSTR_DESCR (JMP,  1, 14)
    INSTR_DESCR (JNZ,  1, 15)
    INSTR_DESCR (JZ,   1, 16)
    INSTR_DESCR (JL,   1, 17)
    INSTR_DESCR (JLE,  1, 18)
    INSTR_DESCR (JG,   1, 19)
    INSTR_DESCR (JGE,  1, 20)
    INSTR_DESCR (CALL, 1, 21)

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


static Erracc_t decomposeSpecial (const char* instr, Buffer* bufR, Buffer* bufW, size_t instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    assertStrict (instr, "received NULL");

    instruction_t nothing = {0};
    char key = 0;
    if (sscanf (instr, "%[^:]%c", nothing, &key) && key == ':') return labelDecl (instr, bufW);

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



static Erracc_t decompose (Buffer* bufR, Buffer* bufW, size_t* instrc)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    assertStrict (instrc, "received NULL");
    
    instrinit ();
    instruction_t instruction = {0};

    while (sSpaceLines (bufR) && bufScanf (bufR, "%s", instruction) > 0)
    {
        if (*instruction == ';')
        {
            bufNLine (bufR);
            continue;
        }

        hash_t hash = djb2Hash (instruction, sizeof (instruction));

        for (size_t i = 0; i <= NUM_OPS; i++)
        {
            if (i == NUM_OPS) decomposeSpecial (instruction, bufR, bufW, *instrc);

            if (hash == Instructions[i].hash)
            {
                Instructions[i].handler (bufR, bufW);
                break;
            }
        }

        *instrc += 1;


        bufSSpaces (bufR);
        if (bufpeekc (bufR) != '\0' && bufpeekc (bufR) != ';')
        {
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);

            bufDump (bufR);

            log_srcerr
            (
                bufR->name,
                *instrc + 1,
                "syntax error",
                "multiple instructions in line"
            );
        }
        
        if (ErrAcc)
        {
            log_err ("runtime error", "aborting");
            return ErrAcc;
        }
    }

    if (remUnmngldJMP () != 0)
    {
        jmpWLdump ();
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_err ("syntax error", "missing jmptags");
        return ErrAcc;
    }


    return ErrAcc;
}

#undef CASE_SIMPLEINSTRUCTION



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
    bufLSplit (bufR);

    fseek (bin, sizeof (Header), SEEK_SET);

    Header header = {
        .sign =    RTASM_SIGN,
        .version = RTASM_VER,
        .instrc = 0,
    };


    decompose (bufR, bufW, &header.instrc);


    log_string ("<grn>translated %llu opcode(s)<dft>\n", header.instrc);

    bufFree (bufW);
    bufFree (bufR);
    fseek  (bin, 0, SEEK_SET);
    fwrite (&header, sizeof (Header), 1, bin);
    
    fclose (listing);
    fclose (bin);

    return ErrAcc;
}