#ifndef ARCH_H
#define ARCH_H


#include <stdint.h>

#define RTASM_SIGN  0X45A2833
#define RTASM_VER   0XA000201

typedef struct
{
    uint64_t sign;
    uint64_t version;
    size_t   instrc; 
}Header;


#define VALUEPREFIX    '#'
#define VALUEFORMAT    "%d"
#define REGISTERPREFIX '%'
#define REGISTERFORMAT "%s"

enum OP_codes
{
    PUSH = 0X51,
    OUT  = 0XF0,
    POP  = 0X59,
    HALT = 0XFF,

    MOV  = 0X48,

    ADD  = 0XA0,
    SUB  = 0XA1,
    MUL  = 0XA2,
    DIV  = 0XA3,
};

enum Registers
{
    AAX = 1,
    ACX = 2,
    ADX = 3,
    ABX = 4,
    ASP = 5,
    ABP = 6,
    ASI = 7,
    ADI = 8,
};

#define NUM_REGS 8
#define OPCODESHIFT (sizeof (opcode_t) - 1) * 8

typedef uint16_t opcode_t;
typedef int32_t  operand_t;


#endif