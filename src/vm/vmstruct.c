#include "vm.h"


VM* VMInit (size_t stackSize)
{
    VM* vm = (VM*)calloc(1, sizeof (VM));
    
    if (!vm)
    {
        ErrAcc |= VMINITIALIZATION;
        log_err ("runtime error", "calloc returned NULL");
        return NULL;
    }
    vm->stack = stackInit (stackSize, sizeof (operand_t));

    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    return vm;
}

void VMFree (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    stackFree (vm->stack);
    memset (vm, 0, sizeof (VM));
    
    free (vm);
}

Erracc_t VMVerify (const VM* vm)
{
    if (vm == NULL)
    {
        ErrAcc |= NULLRECEIVED;
        log_err ("verification error", "received NULL");
        return ErrAcc;
    }

    Erracc_t stackST = stackVerify (vm->stack);
    if (stackVerify (vm->stack) != 0)
    {
        ErrAcc |= STACKVERIFICATION;
        log_err ("verification error", "stack failed verification with code: %llu", stackST);
    }

    return ErrAcc;
}