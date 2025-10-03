#include "../vm.h"


VM* VMInit (size_t stackSize, size_t ramSize)
{
    VM* vm = (VM*)calloc(1, sizeof (VM));
    
    if (!vm)
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("internal error", "calloc returned NULL");
        return NULL;
    }
    vm->stack = stackInit (stackSize, sizeof (operand_t));

    vm->ram.data = (char*)calloc (ramSize, sizeof (char));
    if (!vm->ram.data)
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("internal error", "calloc returned NULL");
        return NULL;
    }
    vm->ram.size = ramSize;

    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    return vm;
}

void VMFree (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    if (vm)
    {
        stackFree (vm->stack);
        free (vm->ram.data);
        memset (vm, 0, sizeof (VM));
        
        free (vm);
    }
    else
    {
        ErrAcc |= VM_NULLRECEIVED;
        log_err ("runtime error", "received NULL");
    }
}

Erracc_t VMVerify (const VM* vm)
{
    if (vm == NULL)
    {
        ErrAcc |= VM_ERRCODE (VM_NULLRECEIVED);
        log_err ("verification error", "received NULL");
        return ErrAcc;
    }

    bool aax = &vm->aax == &vm->regs[0];
    bool acx = &vm->acx == &vm->regs[1];
    bool adx = &vm->adx == &vm->regs[2];
    bool abx = &vm->abx == &vm->regs[3];
    bool asp = &vm->asp == &vm->regs[4];
    bool abp = &vm->abp == &vm->regs[5];
    bool asi = &vm->asi == &vm->regs[6];
    bool adi = &vm->adi == &vm->regs[7];

    if(!aax | !acx | !adx | !abx | !asp | !abp | !asi | !adi)
    {
        ErrAcc |= VM_ERRCODE (VM_REGMISSADDRESSING);
        log_err ("verification error", "discrepancy in the addresses");
    }

    if (!vm->ram.data)
    {
        ErrAcc |= VM_ERRCODE (VM_RAMVERIFICATION);
        log_err ("verification error", "RAM is a NULL");
    }

    Erracc_t stackST = stackVerify (vm->stack);
    if (stackVerify (vm->stack) != 0)
    {
        ErrAcc |= VM_ERRCODE (VM_STACKVERIFICATION);
        log_err ("verification error", "stack failed verification with code: %llu", stackST);
    }

    return ErrAcc;
}