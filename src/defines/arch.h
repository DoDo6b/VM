#ifndef ARCH_H
#define ARCH_H


#include <stdint.h>

#define RTASM_VER   "A250910"
#define VALUEPREFIX '#'

enum OP_codes
{
    PUSH = 0XA0,
    OUT  = 0XA1,
    POP  = 0XA2,
    HALT = 0XAF,

    ADD  = 0XB0,
    SUB  = 0XB1,
    MUL  = 0XB2,
    DIV  = 0XB3,
};

typedef uint8_t opcode_t;


#endif