#ifndef VM_H
#define VM_H


#include <stdlib.h>
#include <stdint.h>

#include "../../included/logger/logger.h"
#include "../../defines/arch.h"
#include "vmcodes.h"
#include "segments/segments.h"
#include "../../included/stack/stack.h"

#define STACKSIZE 2048
#define RAMSIZE 4096


typedef struct
{
    CodeSeg      codeseg;
    StackHandler stack;
    StackHandler callstack;
    RAMseg       memseg;

    union
    {
        struct
        {
            operand_t rax;
            operand_t rcx;
            operand_t rdx;
            operand_t rbx;
            operand_t rsp;
            operand_t rbp;
            operand_t rsi;
            operand_t rdi;
        };

        operand_t regs[NUM_REGS];
    };
    uint64_t rflags;
}VM;

VM* VMInit (const char* bcname, size_t stackSize, size_t ramSize);

void VMFree (VM* vm);

Erracc_t VMVerify (const VM* vm);
Erracc_t VMdump   (const VM* vm);


#endif