#ifndef STACKOP_H
#define STACKOP_H


#include "../../../vm.h"


Erracc_t push (opcode_t opcode, Buffer* src, const VM* vm);

void out (const VM* vm);
void pop (      VM* vm);


#endif