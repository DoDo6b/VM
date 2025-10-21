#include "operations.h"


#define OP_DESCR(opc)  \
    [OPC_ ## opc] = { \
        .opcode = OPC_ ## opc, \
        .exec   = op ## opc\
    },

operation_s operations[NUM_OPS] = {
    OP_DESCR (IN)

    OP_DESCR (OUT)
    OP_DESCR (POP)
    OP_DESCR (PUSH)

    OP_DESCR (MOV)

    OP_DESCR (JMP)
    OP_DESCR (JNZ)
    OP_DESCR (JZ)
    OP_DESCR (JL)
    OP_DESCR (JLE)
    OP_DESCR (JGE)
    OP_DESCR (JG)

    OP_DESCR (CALL)
    OP_DESCR (RET)

    OP_DESCR (CMP)

    OP_DESCR (ADD)
    OP_DESCR (SUB)
    OP_DESCR (MUL)
    OP_DESCR (DIV)

    OP_DESCR (DMP)
    OP_DESCR (DRAW)
};

#undef OP_DESCR