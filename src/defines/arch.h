#ifndef ARCH_H
#define ARCH_H


#include <stdlib.h>
#include <stdint.h>

#define RTASM_SIGN  0X45A2833
#define RTASM_VER   0XA000812

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
    HALT =  0X00,

    IN    = 0X01,

    OUT   = 0X02,
    POP   = 0X03,
    PUSH  = 0X04,

    MOV   = 0X05,

    JMP   = 0X06,
    JNZ   = 0X07,
    JZ    = 0X08,
    JL    = 0X09,
    JLE   = 0X0A,
    JGE   = 0X0B,
    JG    = 0X0C,

    CALL  = 0X0D,
    RET   = 0X0E,

    CMP  =  0X0F,

    ADD   = 0X10,
    SUB   = 0X11,
    MUL   = 0X12,
    DIV   = 0X13,

    DMP   = 0X14,
    DRAW  = 0X15,
};

#define NUM_OPS 22

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