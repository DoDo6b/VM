#include "../stack_.h"
#include "handlers.h"

static Stack* Pointers[HANDLERSCAP] = {NULL};
static StackHandler FreeHandler = 0;


static StackHandler handlerAdd (Stack* ptr)
{
    assertStrict (ptr, "NULL received");
    assertStrict (FreeHandler < HANDLERSCAP, "HANDLERSCAP has been reached");
    assertStrict (Pointers[FreeHandler] == NULL, "internal error");

    StackHandler handler = FreeHandler;
    while (Pointers[++FreeHandler]);

    Pointers[handler] = ptr;

    return handler;
}

static Stack* handlerFree (StackHandler handler)
{
    assertStrict (handler < HANDLERSCAP, "invalid handler");

    if (Pointers[handler])
    {
        Stack* ptr = Pointers[handler];
        Pointers[handler] = NULL;
        if (FreeHandler > handler) FreeHandler = handler;
        return ptr;
    }
    else return NULL;
}

StackHandler stackInitH (size_t numOfElem, ssize_t sizeOfElem)
{
    Stack* ptr = stackInitD (numOfElem, sizeOfElem);
    return handlerAdd (ptr);
}

void stackReallocH (StackHandler handle, size_t newCapacity, bool ignoreDataLoss)
{
    assertStrict (handle < HANDLERSCAP && Pointers[handle], "invalid handler");
    stackReallocD (Pointers[handle], newCapacity, ignoreDataLoss);
}

void stackFreeH (StackHandler handle)
{
    assertStrict (handle < HANDLERSCAP && Pointers[handle], "invalid handler");
    stackFreeD ( handlerFree (handle) );
}

void stackTopH (StackHandler handle, void* dst)
{
    assertStrict (handle < HANDLERSCAP && handle < HANDLERSCAP && Pointers[handle], "invalid handler");
    stackTopD (Pointers[handle], dst);
}

void stackPushH (StackHandler handle, const void* src)
{
    assertStrict (handle < HANDLERSCAP && Pointers[handle], "invalid handler");
    stackPushD (Pointers[handle], src);
}

void stackPopH_ (StackHandler handle, void* dst)
{
    assertStrict (handle < HANDLERSCAP && Pointers[handle], "invalid handler");
    stackPopD_ (Pointers[handle], dst);
}

size_t stackLenH (StackHandler handle)
{
    assertStrict (handle < HANDLERSCAP && Pointers[handle], "invalid handler");
    return stackLenD (Pointers[handle]);
}

void stackDumpH_ (const char* name, StackHandler handle, void (*print)(const void* obj))
{
    assertStrict (handle < HANDLERSCAP && Pointers[handle], "invalid handler");
    stackDumpD_ (name, Pointers[handle], print);
}

Erracc_t stackVerifyH_ (const char* callerFile, unsigned int callerLine, StackHandler handle)
{
    assertStrict (handle < HANDLERSCAP && Pointers[handle], "invalid handler");
    return stackVerifyD_ (callerFile, callerLine, Pointers[handle]);
}