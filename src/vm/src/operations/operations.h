#ifndef OPERATIONS_H
#define OPERATIONS_H


#include "../vm.h"


Erracc_t add  (VM* vm);
Erracc_t sub  (VM* vm);
Erracc_t mul  (VM* vm);
Erracc_t div  (VM* vm);
Erracc_t cmp  (VM* vm);

Erracc_t jmp  (VM* vm);
Erracc_t jnz  (VM* vm);
Erracc_t jz   (VM* vm);
Erracc_t jl   (VM* vm);
Erracc_t jle  (VM* vm);
Erracc_t jg   (VM* vm);
Erracc_t jge  (VM* vm);

Erracc_t mov  (VM* vm);

Erracc_t push (VM* vm);
void     out  (VM* vm);
void     pop  (VM* vm);

#endif