#ifndef JMP_H
#define JMP_H


#include "../../../included/logger/logger.h"
#include "../../../included/kassert/kassert.h"
#include "../../../included/buffer/buffer.h"
#include "../../../defines/arch.h"
#include "../translatorcodes.h"
#include "operands/operands.h"


typedef struct __attribute__((packed))
{
    opcode_t opcode;
    offset_t offset;
}JMPopcode;


#define JMPPOINTNAME_SIZ 16
#define JMPTABLE_SIZ 256


Erracc_t labelDecl  (const char* str, Buffer* bufW);

void handleJMP  (Buffer* bufR, Buffer* bufW);
void handleJNZ  (Buffer* bufR, Buffer* bufW);
void handleJZ   (Buffer* bufR, Buffer* bufW);
void handleJL   (Buffer* bufR, Buffer* bufW);
void handleJLE  (Buffer* bufR, Buffer* bufW);
void handleJG   (Buffer* bufR, Buffer* bufW);
void handleJGE  (Buffer* bufR, Buffer* bufW);
void handleCALL (Buffer* bufR, Buffer* bufW);


Erracc_t jmpWLdump ();
size_t remUnmngldJMP ();


#endif