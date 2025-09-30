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
    pointer_t jmptag;
}JMPopcode;


#define JMPPOINTNAME_SIZ 16
#define JMPTABLE_SIZ 256

Erracc_t decomposeChpoint (const char* str, Buffer* bufW);
Erracc_t decomposeJMP     (Buffer* bufR,    Buffer* bufW, size_t instrC);

size_t remainingUnprocJMPReq ();


#endif