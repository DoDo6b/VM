#ifndef HANDLERS_H
#define HANDLERS_H


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "included/logger/logger.h"
#include "included/kassert/kassert.h"
#include "../Policy.h"
#include "errCodes.h"

typedef size_t StackHandler;
#define HANDLERSCAP 1024


StackHandler stackInitH (size_t numOfElem, size_t sizeOfElem);

void stackReallocH (StackHandler handle, size_t newCapacity, bool ignoreDataLoss);

void stackFreeH (StackHandler handle);

void stackTopH (StackHandler handle, void* dst);

void stackPushH (StackHandler handle, const void* src);

void stackPopH_ (StackHandler handle, void* dst);
#define stackPopH(handle) stackPopH_ (handle, NULL)

size_t stackLenH (StackHandler handle);

void    stackDumpH_ (const char* name, StackHandler handle, void (*print)(const void* obj));
#define stackDumpH(handle) stackDumpH_ (#handle, handle, NULL)

uint64_t stackVerifyH_ (const char* callerFile, unsigned int callerLine, StackHandler handle);
#define  stackVerifyH(handle) stackVerifyH_ (__FILE__, __LINE__, handle)


#endif