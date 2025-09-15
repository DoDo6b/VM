#ifndef VM_H
#define VM_H


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "../included/logger/logger.h"
#include "../defines/arch.h"
#include "../defines/errcodes.h"
#include "../included/stack/stack.h"


#define STACKSIZE 1024

typedef struct
{
    StackHandler stack;
    register_t aax;
    register_t acx;
    register_t adx;
    register_t abx;
    register_t asp;
    register_t abp;
    register_t asi;
    register_t adi;
}VM;

uint64_t run (const char* fname);


#endif