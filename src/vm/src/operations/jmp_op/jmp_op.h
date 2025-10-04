#ifndef JMP_OP_H
#define JMP_OP_H


#include "../../vm.h"


Erracc_t jmp (VM* vm);
Erracc_t jnz (VM* vm);
Erracc_t jz  (VM* vm);
Erracc_t jl  (VM* vm);
Erracc_t jle (VM* vm);
Erracc_t jg  (VM* vm);
Erracc_t jge (VM* vm);


#endif