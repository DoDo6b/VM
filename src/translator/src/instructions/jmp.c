#include "jmp.h"


typedef struct
{
    unsigned long hash;
    pointer_t jmptr;
}JMPtag;

typedef struct
{
    unsigned long hash;
    pointer_t backjmpptr;
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
    .jmptable         = {0},
    .jmprequests      = {0},
};

Erracc_t decomposeChpoint (const char* str, Buffer* bufW)
{
    assertStrict (bufVerify (bufW, 0) == 0, "buffer failed verification");
    assertStrict (str, "received NULL");

    instruction_t jmptag = {0};
    char key = 0;
    if (sscanf (str, "%[^:]%c", jmptag, &key) < 1 || key != ':')
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_err ("internal error", "sscanf cant find jmptag");
        return ErrAcc;
    }
    hash_t hash = djb2Hash (jmptag, sizeof (jmptag));

    JMPopcode opcode = {
        .opcode   = JMP << OPCODESHIFT,
        .jmptag = (pointer_t)(bufW->bufpos - bufW->buffer),
    };

    for (size_t i = 0; i < JMPTABLE_SIZ; i++)
    {
        if (JMPWaitingList.jmprequests[i].hash == hash)
        {
            char* const bufWpos = bufW->bufpos;
            bufW->bufpos  = (char*)JMPWaitingList.jmprequests[i].backjmpptr;

            if (bufWrite (bufW, &opcode, sizeof (JMPopcode)) == 0)
            {
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
                log_err ("writing error", "cant write into buffer");
                return ErrAcc;
            }
            bufW->bufpos = bufWpos;

            JMPWaitingList.jmprequests[i].hash       = 0;
            JMPWaitingList.jmprequests[i].backjmpptr = 0;
        }
    }

    JMPWaitingList.jmptable[JMPWaitingList.jmpTagTotal].hash    = hash;
    JMPWaitingList.jmptable[JMPWaitingList.jmpTagTotal].jmptr = opcode.jmptag;
    JMPWaitingList.jmpTagTotal++;

    if (JMPWaitingList.jmpTagTotal >= JMPTABLE_SIZ)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_BUFOVERFLOW);
        log_err ("buffer overflow", "cant write jmptag into buffer");
    }
    
    return ErrAcc;
}

Erracc_t decomposeJMP (Buffer* bufR, Buffer* bufW, size_t instrC)
{
    assertStrict (bufVerify (bufW, 0) == 0, "buffer failed verification");
    assertStrict (bufVerify (bufR, 0) == 0, "buffer failed verification");

    JMPopcode opcode = {
        .opcode   = JMP << OPCODESHIFT,
        .jmptag = 0,
    };

    bufSpaces (bufR);

    instruction_t jmptag = {0};
    if (!bufScanf (bufR, "%s", jmptag))
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_SYNTAX);
        log_srcerr
        (
            bufR->name,
            instrC + 1,
            "syntax error",
            "no jmp point found"
        );
    }

    hash_t hash = djb2Hash (jmptag, sizeof (jmptag));
    for (size_t i = 0; i < JMPWaitingList.jmpTagTotal; i++)
    {
        if (JMPWaitingList.jmptable[i].hash == hash)
        {
            opcode.jmptag = JMPWaitingList.jmptable[i].jmptr;
            if (bufWrite (bufW, &opcode, sizeof (JMPopcode)) == 0)
            {
                ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
                log_err ("writing error", "cant write into buffer");
                IMSTP ( exit (EXIT_FAILURE); )
            }
            return ErrAcc;
        }
    }

    for (size_t i = 0; i < JMPTABLE_SIZ; i++)
    {
        if (JMPWaitingList.jmprequests[i].hash == 0 && JMPWaitingList.jmprequests[i].backjmpptr == 0)
        {
            JMPWaitingList.jmprequests[i].hash = hash;
            JMPWaitingList.jmprequests[i].backjmpptr = (pointer_t)bufW->bufpos;
            break;
        }
    }

    if (bufWrite (bufW, &opcode, sizeof (JMPopcode)) == 0)
    {
        ErrAcc |= TRNSLT_ERRCODE (TRNSLTR_WRITINGERROR);
        log_err ("writing error", "cant write into buffer");
    }

    return ErrAcc;
}

size_t remainingUnprocJMPReq ()
{
    size_t requests = 0;
    for (size_t i = 0; i < JMPTABLE_SIZ; i++) 
    {
        if (JMPWaitingList.jmprequests[i].hash != 0 && JMPWaitingList.jmprequests[i].backjmpptr != 0) requests++;
    }
    return requests;
}