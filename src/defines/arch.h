#ifndef ARCH_H
#define ARCH_H


#include <stdint.h>

#define RTASM_VER   "A000102"
#define VALUEFORMAT    "#%d"
#define REGISTERFORMAT "%%%s"

enum OP_codes
{
    PUSH = 0X50,
    OUT  = 0XF0,
    POP  = 0X58,
    HALT = 0XFF,

    MOV  = 0X48,

    ADD  = 0XA0,
    SUB  = 0XA1,
    MUL  = 0XA2,
    DIV  = 0XA3,
};

enum Registers
{
    AAX,
    ACX,
    ADX,
    ABX,
    ASP,
    ABP,
    ASI,
    ADI,
};

typedef uint8_t opcode_t;
typedef int32_t operand_t;


#endif