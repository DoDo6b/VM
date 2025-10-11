#ifndef ARCH_H
#define ARCH_H


#include <stdlib.h>
#include <stdint.h>

#define RTASM_SIGN  0X45A2833
#define RTASM_VER   0XA000503

typedef struct
{
    uint64_t sign;
    uint64_t version;
    size_t   instrc; 
}Header;


#define VALUEPREFIX    '#'
#define VALUEFORMAT    "%lld"
#define REGISTERPREFIX '%'
#define REGISTERFORMAT "%s"
#define RAPREFIX       "[]"


enum OP_codes
{
    HALT = 0X66,

    OUT   = 0X50,
    POP   = 0X55,
    PUSH  = 0X53,

    MOV   = 0X39,

    JMP   = 0X3F,
    JNZ   = 0X75,
    JZ    = 0X74,
    JL    = 0X7C,
    JLE   = 0X7E,
    JGE   = 0X7D,
    JG    = 0X7F,

    CALL  = 0X7A,
    RET   = 0X7B,

    CMP  = 0X60,

    ADD   = 0X01,
    SUB   = 0X02,
    MUL   = 0X03,
    DIV   = 0X04,

    DRAW  = 0X6E,
};

#define INSTRUCTIONBUF 8
typedef char        instruction_t[INSTRUCTIONBUF];
typedef uint8_t     opcode_t;


enum Registers
{
    RAX,
    RCX,
    RDX,
    RBX,
    RSP,
    RBP,
    RSI,
    RDI,
};

#define NUM_REGS 8
#define OPCODESHIFT (sizeof (opcode_t) - 1) * 8

typedef int64_t     operand_t;
typedef int64_t     offset_t;
typedef uintptr_t   pointer_t;


enum MOD
{
    MEM,
    IMM,
    OFF,
    REG,
};
#define DISP64 5

#endif