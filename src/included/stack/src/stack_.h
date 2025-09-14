#ifndef STACK__H
#define STACK__H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "included/logger/logger.h"
#include "included/kassert/kassert.h"
#include "included/Macro.h"

#include "../Policy.h"
#if TOLERANCEPOLICY_LVL >= 2
    #include "crc.h"
#endif

#include "errCodes.h"


const uint32_t FRONTCANARY = 0X8BADF00D;
const uint32_t TAILCANARY  = 0XDEFEC10D;
const uintptr_t HEXSPEAK   = 0XBADA110CBADFEE11;

typedef struct
{
T1( uint32_t frontCanary; )
    char* top;
    char* data;
    size_t capacity;
    size_t sizeOfElem;
T2( uint32_t crc32Data; )
T2( uint32_t crc32; )
T1( uint32_t tailCanary; )
}Stack;

Stack* stackInitD (size_t numOfElem, size_t sizeOfElem);

const unsigned int GrowthRate = 200;
const unsigned int ReductionRate = 66;

void stackReallocD (Stack* stack, size_t newCapacity, bool ignoreDataLoss);

void stackFreeD (Stack* stack);

void stackTopD (const Stack* stack, void* dst);

void stackPushD (Stack* stack, const void* src);

void stackPopD_ (Stack* stack, void* dst);
#define stackPopD(stack) stackPopD_ (stack, NULL);

size_t stackLenD (const Stack* stack);

void    stackDumpD_ (const char* name, const Stack* stack, void (*print)(const void* obj));
#define stackDumpD(stack) stackDumpD_ (#stack, stack, NULL)

uint64_t stackVerifyD_ (const char* callerFile, unsigned int callerLine, const Stack* stack);
#define  stackVerifyD(stack) stackVerifyD_ (__FILE__, __LINE__, stack)


#endif