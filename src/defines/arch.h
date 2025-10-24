#ifndef ARCH_H
#define ARCH_H


#include <stdlib.h>
#include <stdint.h>

#define RTASM_SIGN  0X45A2833
#define RTASM_VER   0XA031543

typedef struct
{
    uint64_t sign;
    uint64_t version;
    size_t   instrc; 
}Header;


#define VALUEFORMAT    "%lld"
#define REGISTERFORMAT "%s"


enum OP_codes
{
    OPC_HALT =  0X00,

    OPC_IN    = 0X01,

    OPC_OUT   = 0X02,
    OPC_POP   = 0X03,
    OPC_PUSH  = 0X04,

    OPC_MOV   = 0X05,

    OPC_JMP   = 0X06,
    OPC_JNZ   = 0X07,
    OPC_JZ    = 0X08,
    OPC_JL    = 0X09,
    OPC_JLE   = 0X0A,
    OPC_JGE   = 0X0B,
    OPC_JG    = 0X0C,

    OPC_CALL  = 0X0D,
    OPC_RET   = 0X0E,

    OPC_CMP  =  0X0F,

    OPC_ADD   = 0X10,
    OPC_SUB   = 0X11,
    OPC_MUL   = 0X12,
    OPC_DIV   = 0X13,

    OPC_DMP   = 0X14,
    OPC_DRAW  = 0X15,
};

#define NUM_OPS 22

#define INSTRUCTIONBUF 8
typedef char        instruction_t[INSTRUCTIONBUF];
typedef uint8_t     opcode_t;
#define NULLOPC     UINT8_MAX

enum Registers
{
    OPC_RAX,
    OPC_RCX,
    OPC_RDX,
    OPC_RBX,
    OPC_RSP,
    OPC_RBP,
    OPC_RSI,
    OPC_RDI,
};

#define NUM_REGS 8

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