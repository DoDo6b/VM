#include "instructions.h"


typedef struct __attribute__((packed))
{
    opcode_t opcode;
    offset_t offset;
}JMPopcode;


typedef struct
{
    hash_t hash;
    pointer_t ptr;
}Label;

typedef struct
{
    hash_t labelhash;
    opcode_t opcode;
    pointer_t ptr;
}Request;

#define JMPPOINTNAME_SIZ 16
#define JMPTABLE_SIZ 256

static struct
{
    size_t labelsTotal;
    size_t reqTotal;
    Label   labels  [JMPTABLE_SIZ];
    Request requests[JMPTABLE_SIZ];
}Unmangled = {
    .labelsTotal =  0,
    .reqTotal    =  0,
    .labels      = {},
    .requests    = {},
};

void jmpWLdump ()
{
    log_string ("<blu><b>jmp waiting list dump:</b><dft>\n");

    log_string ("<blu>jmp tag list dump(%llu in total):<dft>\n{\n", Unmangled.labelsTotal);
    for (size_t i = 0; i < Unmangled.labelsTotal; i++) log_string ("  %lu: <cyn>0x%p<dft>\n", Unmangled.labels[i].hash, Unmangled.labels[i].ptr);
    log_string ("}\n");

    log_string ("<blu>jmp requests list dump(%llu in total):<dft>\n{\n", Unmangled.reqTotal);
    for (size_t i = 0; i < JMPTABLE_SIZ; i++) if (Unmangled.requests[i].opcode != 0) 
        log_string ("  %lu: <grn>%0X<dft> <cyn>0x%p<dft>\n", Unmangled.requests[i].labelhash, Unmangled.requests[i].opcode, Unmangled.labels[i].ptr);
    
    log_string ("}\n");
}


void labelDecl (const char* str, Buffer* bufW)
{
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    assertStrict (str, "received NULL");

    instruction_t jmptag = {0};
    char key = 0;
    if (sscanf (str, "%[^:]%c", jmptag, &key) < 1 || key != ':')
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_INTERNALERROR);
        log_err ("internal error", "sscanf cant find jmptag");
        return;
    }
    hash_t hash = djb2Hash (jmptag, sizeof (jmptag));


    pointer_t absPtr = (pointer_t)(bufW->bufpos - bufW->buffer);

    for (size_t i = 0; i < JMPTABLE_SIZ; i++)
    {
        if (Unmangled.requests[i].labelhash == hash)
        {
            char* const bufWpos = bufW->bufpos;
            bufW->bufpos  = Unmangled.requests[i].ptr + bufW->buffer;

            JMPopcode opcode =
            {
                .opcode =                     Unmangled.requests[i].opcode,
                .offset = (offset_t)(absPtr - Unmangled.requests[i].ptr - 1),
            };

            bufWrite (bufW, &opcode, sizeof (JMPopcode));

            log_string
            (
                "has wrote: JMP\n"
                "{\n"
                "opcode: %0X\n"
                "offset: %lld\n"
                "}\n",
                opcode.opcode,
                opcode.offset
            );

            bufW->bufpos = bufWpos;

            Unmangled.requests[i].labelhash          = 0;
            Unmangled.requests[i].opcode             = 0;
            Unmangled.requests[i].ptr                = 0;
            Unmangled.reqTotal--;
        }
    }

    Unmangled.labels[Unmangled.labelsTotal].hash = hash;
    Unmangled.labels[Unmangled.labelsTotal].ptr  = absPtr;
    Unmangled.labelsTotal++;

    if (Unmangled.labelsTotal >= JMPTABLE_SIZ)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFOVERFLOW);
        log_err ("buffer overflow", "cant write jmptag into buffer");
    }
}

void JMPLikeMangle (Buffer* bufR, Buffer* bufW, opcode_t opcode)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    JMPopcode jmpopcode = {
        .opcode   = opcode,
        .offset = 0,
    };

    bufSSpaces (bufR);

    instruction_t jmptag = {0};
    if (!bufScanf (bufR, "%s", jmptag))
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            bufTellL (bufR),
            "syntax error",
            "no jmp tag found"
        );
        return;
    }

    hash_t hash = djb2Hash (jmptag, sizeof (jmptag));
    for (size_t i = 0; i < Unmangled.labelsTotal; i++)
    {
        if (Unmangled.labels[i].hash == hash)
        {
            jmpopcode.offset = (offset_t)(Unmangled.labels[i].ptr - (pointer_t)(bufW->bufpos - bufW->buffer) - 1);
            bufWrite (bufW, &jmpopcode, sizeof (JMPopcode));

            log_string
            (
                "  has wrote: JMP\n"
                "  {\n"
                "  opcode: %0X\n"
                "  offset: %lld\n"
                "  }\n",
                jmpopcode.opcode,
                jmpopcode.offset
            );
            
            return;
        }
    }

    bool overflow = true;
    for (size_t i = 0; i < JMPTABLE_SIZ; i++)
    {
        if (Unmangled.requests[i].labelhash == 0 && Unmangled.requests[i].ptr == 0 && Unmangled.requests[i].opcode == 0)
        {
            Unmangled.requests[i].labelhash   = hash;
            Unmangled.requests[i].opcode      = opcode;
            Unmangled.requests[i].ptr         = (pointer_t)(bufW->bufpos - bufW->buffer);
            Unmangled.reqTotal++;
            overflow = false;
            break;
        }
    }
    
    if (overflow)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFOVERFLOW);
        log_err ("buffer overflow", "cant write jmptag into buffer");
        return;
    }

    bufWrite (bufW, &opcode, sizeof (JMPopcode));
}

#define JMPPATTERN(opcode) \
void handle ## opcode (Buffer* bufR, Buffer* bufW)\
{\
    JMPLikeMangle (bufR, bufW, OPC_ ## opcode);\
}

JMPPATTERN (JMP)
JMPPATTERN (JNZ)
JMPPATTERN (JZ)
JMPPATTERN (JL)
JMPPATTERN (JLE)
JMPPATTERN (JG)
JMPPATTERN (JGE)
JMPPATTERN (CALL)


#undef JMPPATTERN


size_t remUnmngldJMP ()
{
    size_t requests = 0;
    for (size_t i = 0; i < JMPTABLE_SIZ; i++) 
    {
        if (Unmangled.requests[i].labelhash != 0 || Unmangled.requests[i].opcode) requests++;
    }
    return requests;
}