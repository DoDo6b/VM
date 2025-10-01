#ifndef JMP_OP_H
#define JMP_OP_H


#include "../../../vm.h"


Erracc_t jmp (Buffer* src, VM* vm);
Erracc_t jnz (Buffer* src, VM* vm);
Erracc_t jz  (Buffer* src, VM* vm);
Erracc_t jl  (Buffer* src, VM* vm);
Erracc_t jle (Buffer* src, VM* vm);
Erracc_t jg  (Buffer* src, VM* vm);
Erracc_t jge (Buffer* src, VM* vm);


#endif