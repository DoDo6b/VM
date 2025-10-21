#ifndef OPERATIONS_H
#define OPERATIONS_H


#include "../vm.h"


typedef struct
{
    opcode_t opcode;
    void (*exec)(VM*);
}operation_s;

extern operation_s operations[NUM_OPS];


void opADD  (VM* vm);
void opSUB  (VM* vm);
void opMUL  (VM* vm);
void opDIV  (VM* vm);
void opCMP  (VM* vm);

void opJMP  (VM* vm);
void opJNZ  (VM* vm);
void opJZ   (VM* vm);
void opJL   (VM* vm);
void opJLE  (VM* vm);
void opJG   (VM* vm);
void opJGE  (VM* vm);

void opCALL (VM* vm);
void opRET  (VM* vm);

void opMOV  (VM* vm);

void opIN   (VM* vm);

void opPUSH (VM* vm);
void opOUT  (VM* vm);
void opPOP  (VM* vm);

void opDRAW (VM* vm);
void opDMP  (VM* vm);

#endif