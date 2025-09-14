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
typedef int32_t operand_t;


uint64_t run (const char* fname);


#endif