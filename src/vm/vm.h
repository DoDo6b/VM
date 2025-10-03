#ifndef VM_H
#define VM_H


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "../included/logger/logger.h"
#include "../defines/arch.h"
#include "src/vmcodes.h"
#include "../included/stack/stack.h"
#include "../included/buffer/buffer.h"


#define STACKSIZE 1024
#define RAMSIZE 4096


typedef struct
{
    StackHandler stack;
    struct
    {
        size_t size;
        char* data;
    }ram;
    union
    {
        struct
        {
            operand_t aax;
            operand_t acx;
            operand_t adx;
            operand_t abx;
            operand_t asp;
            operand_t abp;
            operand_t asi;
            operand_t adi;
        };

        operand_t regs[NUM_REGS];
    };
    bool zf;
    bool cf;
}VM;

VM* VMInit (size_t stackSize, size_t ramSize);

void VMFree (VM* vm);

Erracc_t VMVerify (const VM* vm);


Erracc_t run (const char* fname);


#endif