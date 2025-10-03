#ifndef STACK_H
#define STACK_H


#include "Policy.h"
#if TOLERANCEPOLICY_LVL < 3

    #include "src/stack_.h"

    typedef Stack* StackHandler;

    #define stackInit(numOfElem, sizeOfElem)                 stackInitD   (numOfElem, sizeOfElem)
    #define stackRealloc(stack, newCapacity, ignoreDataLoss) stackReallocD (stack, newCapacity, ignoreDataLoss)
    #define stackFree(stack)                                 stackFreeD   (stack)
    #define stackTop(stack, dst)                             stackTopD    (stack, dst)
    #define stackPush(stack, src)                            stackPushD   (stack, src)
    #define stackPop(stack, dst)                             stackPopD_   (stack, dst)
    #define stackLen(stack)                                  stackLenD    (stack)
    #define stackDump(stack)                                 stackDumpD   (stack)
    #define stackVerify(stack)                               stackVerifyD (stack)

#else
    #include "src/handlers/handlers.h"

    #define stackInit(numOfElem, sizeOfElem)                 stackInitH   (numOfElem, sizeOfElem)
    #define stackRealloc(stack, newCapacity, ignoreDataLoss) stackReallocH (stack, newCapacity, ignoreDataLoss)
    #define stackFree(stack)                                 stackFreeH   (stack)
    #define stackTop(stack, dst)                             stackTopH    (stack, dst)
    #define stackPush(stack, src)                            stackPushH   (stack, src)
    #define stackPop(stack, dst)                             stackPopH_   (stack, dst)
    #define stackLen(stack)                                  stackLenH    (stack)
    #define stackDump(stack)                                 stackDumpH   (stack)
    #define stackVerify(stack)                               stackVerifyH (stack)
#endif


#endif