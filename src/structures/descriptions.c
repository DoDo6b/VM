#include "descriptions.h"


iDescription_s Descriptions[NUM_OPS] = {};

#ifdef TARGET_TRNSLT
static int instrHashCmp (const void* a, const void* b)
{
    const iDescription_s* instrA = (const iDescription_s*)a;
    const iDescription_s* instrB = (const iDescription_s*)b;

    if (instrA->hash < instrB->hash) return -1;
    if (instrA->hash > instrB->hash) return 1;
    return 0;
}

const iDescription_s* descriptionSearch (hash_t hash)
{
    iDescription_s key = {
        .opcode = NULLOPC,
        .hash = hash,
        .str = NULL,
        .encode = NULL
    };

    return (const iDescription_s*)bsearch (&key, Descriptions, NUM_OPS, sizeof (iDescription_s), instrHashCmp);
}
#endif

#ifdef TARGET_TRNSLT
#define ADDDESCRIPTION(instr)\
    Descriptions[OPC_ ## instr].opcode = OPC_ ## instr;\
    Descriptions[OPC_ ## instr].str = #instr;\
    Descriptions[OPC_ ## instr].hash = djb2Hash (#instr, sizeof (#instr));\
    Descriptions[OPC_ ## instr].encode = handle ## instr;
#else
#define ADDDESCRIPTION(instr)\
    Descriptions[OPC_ ## instr].opcode = OPC_ ## instr;\
    Descriptions[OPC_ ## instr].exec = op ## instr;
#endif

void descriptionsInit ()
{
#ifdef TARGET_TRNSLT
    ADDDESCRIPTION (HALT)
#endif
    ADDDESCRIPTION (IN)
    ADDDESCRIPTION (OUT)
    ADDDESCRIPTION (POP)
    ADDDESCRIPTION (PUSH)
    ADDDESCRIPTION (MOV)
    ADDDESCRIPTION (RET)
    ADDDESCRIPTION (CMP)
    ADDDESCRIPTION (ADD)
    ADDDESCRIPTION (SUB)
    ADDDESCRIPTION (MUL)
    ADDDESCRIPTION (DIV)
    ADDDESCRIPTION (DMP)
    ADDDESCRIPTION (DRAW)

    ADDDESCRIPTION (JMP)
    ADDDESCRIPTION (JNZ)
    ADDDESCRIPTION (JZ)
    ADDDESCRIPTION (JL)
    ADDDESCRIPTION (JLE)
    ADDDESCRIPTION (JG)
    ADDDESCRIPTION (JGE)
    ADDDESCRIPTION (CALL)

#ifdef TARGET_TRNSLT
    qsort (Descriptions, NUM_OPS, sizeof (iDescription_s), instrHashCmp);
#endif
}
#undef ADDDESCRIPTION