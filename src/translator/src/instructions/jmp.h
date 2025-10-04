#ifndef JMP_H
#define JMP_H


#include "../../../defines/settings.h"
#include "../../../included/logger/logger.h"
#include "../../../included/kassert/kassert.h"
#include "../../../included/buffer/buffer.h"
#include "../../../defines/arch.h"
#include "../translatorcodes.h"
#include "operands/operands.h"
#include "instructions.h"


typedef struct __attribute__((packed))
{
    opcode_t opcode;
    offset_t offset;
}JMPopcode;


#define JMPPOINTNAME_SIZ 16
#define JMPTABLE_SIZ 256

typedef enum
{
    JMP_NOCOND = 0,
    JMP_LESS   = 1,
    JMP_LEQ    = 2,
    JMP_NZERO  = 3,
    JMP_ZERO   = 4,
    JMP_GEQ    = 5,
    JMP_GRTR   = 6,
}JMPCOND;

Erracc_t decomposeChpoint (const char* str, Buffer* bufW);
Erracc_t decomposeJMP     (Buffer* bufR,    Buffer* bufW, size_t instrC, JMPCOND condition);

size_t remainingUnprocJMPReq ();


#endif