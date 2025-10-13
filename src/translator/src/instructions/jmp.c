#include "jmp.h"


typedef struct
{
    unsigned long hash;
    pointer_t absptr;
}JMPtag;

typedef struct
{
    unsigned long hash;
    opcode_t opcode;
    pointer_t absBackJMPptr;
}JMPrequest;

static struct
{
    size_t jmpTagTotal;
    size_t jmpRequestsTotal;
    JMPtag    jmptable   [JMPTABLE_SIZ];
    JMPrequest jmprequests[JMPTABLE_SIZ];
}JMPWaitingList = {
    .jmpTagTotal      =  0,
    .jmpRequestsTotal =  0,
    .jmptable         = {},
    .jmprequests      = {},
};

Erracc_t jmpWLdump ()
{
    log_string ("<blu><b>jmp waiting list dump:</b><dft>\n");

    log_string ("<blu>jmp tag list dump(%llu in total):<dft>\n{\n", JMPWaitingList.jmpTagTotal);
    for (size_t i = 0; i < JMPWaitingList.jmpTagTotal; i++) log_string ("  %lu: <cyn>0x%p<dft>\n", JMPWaitingList.jmptable[i].hash, JMPWaitingList.jmptable[i].absptr);
    log_string ("}\n");

    log_string ("<blu>jmp requests list dump(%llu in total):<dft>\n{\n", JMPWaitingList.jmpRequestsTotal);
    for (size_t i = 0; i < JMPTABLE_SIZ; i++) if (JMPWaitingList.jmprequests[i].opcode != 0) 
        log_string ("  %lu: <grn>%0X<dft> <cyn>0x%p<dft>\n", JMPWaitingList.jmprequests[i].hash, JMPWaitingList.jmprequests[i].opcode, JMPWaitingList.jmptable[i].absptr);
    
    log_string ("}\n");

    return ErrAcc;
}


Erracc_t decomposeChpoint (const char* str, Buffer* bufW)
{
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    assertStrict (str, "received NULL");

    instruction_t jmptag = {0};
    char key = 0;
    if (sscanf (str, "%[^:]%c", jmptag, &key) < 1 || key != ':')
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_INTERNALERROR);
        log_err ("internal error", "sscanf cant find jmptag");
        return ErrAcc;
    }
    hash_t hash = djb2Hash (jmptag, sizeof (jmptag));


    pointer_t absPtr = (pointer_t)(bufW->bufpos - bufW->buffer);

    for (size_t i = 0; i < JMPTABLE_SIZ; i++)
    {
        if (JMPWaitingList.jmprequests[i].hash == hash)
        {
            char* const bufWpos = bufW->bufpos;
            bufW->bufpos  = JMPWaitingList.jmprequests[i].absBackJMPptr + bufW->buffer;

            JMPopcode opcode =
            {
                .opcode =                     JMPWaitingList.jmprequests[i].opcode,
                .offset = (offset_t)(absPtr - JMPWaitingList.jmprequests[i].absBackJMPptr - 1),
            };

            if (bufWrite (bufW, &opcode, sizeof (JMPopcode)) == 0)
            {
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
                log_err ("buffer error", "cant write into buffer");
                return ErrAcc;
            }

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

            JMPWaitingList.jmprequests[i].hash          = 0;
            JMPWaitingList.jmprequests[i].opcode        = 0;
            JMPWaitingList.jmprequests[i].absBackJMPptr = 0;
            JMPWaitingList.jmpRequestsTotal--;
        }
    }

    JMPWaitingList.jmptable[JMPWaitingList.jmpTagTotal].hash    = hash;
    JMPWaitingList.jmptable[JMPWaitingList.jmpTagTotal].absptr  = absPtr;
    JMPWaitingList.jmpTagTotal++;

    if (JMPWaitingList.jmpTagTotal >= JMPTABLE_SIZ)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFOVERFLOW);
        log_err ("buffer overflow", "cant write jmptag into buffer");
    }
    
    return ErrAcc;
}

Erracc_t decomposeJMP (Buffer* bufR, Buffer* bufW, size_t instrC, JMPCOND condition)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    opcode_t jmpopcode = 0;

    switch (condition)
    {
        case JMP_NOCOND: jmpopcode = JMP;  break;
        case JMP_LESS:   jmpopcode = JL;   break;
        case JMP_LEQ:    jmpopcode = JLE;  break;
        case JMP_NZERO:  jmpopcode = JNZ;  break;
        case JMP_ZERO:   jmpopcode = JZ;   break;
        case JMP_GEQ:    jmpopcode = JGE;  break;
        case JMP_GRTR:   jmpopcode = JG;   break;

        case JMP_CALL:   jmpopcode = CALL; break;
        
        default:
            ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_INTERNALERROR);
            log_err ("internal error", "unknown type of JMP");
            return ErrAcc;
    }

    JMPopcode opcode = {
        .opcode   = jmpopcode,
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
            instrC + 1,
            "syntax error",
            "no jmp tag found"
        );
        return ErrAcc;
    }

    hash_t hash = djb2Hash (jmptag, sizeof (jmptag));
    for (size_t i = 0; i < JMPWaitingList.jmpTagTotal; i++)
    {
        if (JMPWaitingList.jmptable[i].hash == hash)
        {
            opcode.offset = (offset_t)(JMPWaitingList.jmptable[i].absptr - (pointer_t)(bufW->bufpos - bufW->buffer) - 1);
            if (bufWrite (bufW, &opcode, sizeof (JMPopcode)) == 0)
            {
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
                log_err ("buffer error", "cant write into buffer");
            }

            log_string
            (
                "  has wrote: JMP\n"
                "  {\n"
                "  opcode: %0X\n"
                "  offset: %lld\n"
                "  }\n",
                opcode.opcode,
                opcode.offset
            );
            
            return ErrAcc;
        }
    }

    bool overflow = true;
    for (size_t i = 0; i < JMPTABLE_SIZ; i++)
    {
        if (JMPWaitingList.jmprequests[i].hash == 0 && JMPWaitingList.jmprequests[i].absBackJMPptr == 0 && JMPWaitingList.jmprequests[i].opcode == 0)
        {
            JMPWaitingList.jmprequests[i].hash   = hash;
            JMPWaitingList.jmprequests[i].opcode = jmpopcode;
            JMPWaitingList.jmprequests[i].absBackJMPptr = (pointer_t)(bufW->bufpos - bufW->buffer);
            JMPWaitingList.jmpRequestsTotal++;
            overflow = false;
            break;
        }
    }
    
    if (overflow)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFOVERFLOW);
        log_err ("buffer overflow", "cant write jmptag into buffer");
        return ErrAcc;
    }

    if (bufWrite (bufW, &opcode, sizeof (JMPopcode)) == 0)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFERR);
        log_err ("buffer error", "cant write into buffer");
    }

    return ErrAcc;
}

size_t remainingUnprocJMPReq ()
{
    size_t requests = 0;
    for (size_t i = 0; i < JMPTABLE_SIZ; i++) 
    {
        if (JMPWaitingList.jmprequests[i].hash != 0 || JMPWaitingList.jmprequests[i].opcode) requests++;
    }
    return requests;
}