#ifndef ARCH_H
#define ARCH_H


#include <stdint.h>

#define RTASM_SIGN  0X45A2833
#define RTASM_VER   0XA000303

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
    HALT = 0XFF,

    OUT   = 0X50,
    POP   = 0X55,
    PUSH  = 0X53,

    MOV   = 0X80,

    JMP   = 0X3F,
    JNZ   = 0X3E,
    JZ    = 0X31,
    JL    = 0X36,
    JLE   = 0X37,
    JGE   = 0X39,
    JG    = 0X3A,

    CMP  = 0XB0,

    ADD   = 0XA0,
    SUB   = 0XA1,
    MUL   = 0XA2,
    DIV   = 0XA3,
};

#define INSTRUCTIONBUF 16
typedef char instruction_t[INSTRUCTIONBUF];

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

typedef uint16_t  opcode_t;
typedef int32_t   operand_t;
typedef size_t pointer_t;


#endif