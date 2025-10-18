#ifndef OPERATIONS_H
#define OPERATIONS_H


#include "../vm.h"


typedef struct
{
    opcode_t opcode;
    void (*exec)(VM*);
}operation_s;

extern operation_s operations[NUM_OPS];


void op_ADD  (VM* vm);
void op_SUB  (VM* vm);
void op_MUL  (VM* vm);
void op_DIV  (VM* vm);
void op_CMP  (VM* vm);

void op_JMP  (VM* vm);
void op_JNZ  (VM* vm);
void op_JZ   (VM* vm);
void op_JL   (VM* vm);
void op_JLE  (VM* vm);
void op_JG   (VM* vm);
void op_JGE  (VM* vm);

void op_CALL (VM* vm);
void op_RET  (VM* vm);

void op_MOV  (VM* vm);

void op_IN   (VM* vm);

void op_PUSH (VM* vm);
void op_OUT  (VM* vm);
void op_POP  (VM* vm);

void op_DRAW (VM* vm);
void op_DMP  (VM* vm);

#endif